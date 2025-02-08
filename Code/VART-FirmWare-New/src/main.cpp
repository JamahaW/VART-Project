#define  FS_NO_GLOBALS

#include <Arduino.h>
#include <SPIFFS.h>
#include <SD.h>
#include <EncButton.h>

#include "VartUI.hpp"
#include "misc/Macro.hpp"

#include "vart/Device.hpp"
#include "vart/util/Pins.hpp"

#include "ui2/Window.hpp"
#include "ui2/impl/FileWidget.hpp"
#include "ui2/impl/OledDisplay.hpp"

#include "bytelang/impl/VartInterpreter.hpp"
#include "bytelang/test/MockStream.hpp"
#include "bytelang/core/MemIO.hpp"


using vart::Pins;

static ui2::Page *printing_page = nullptr;

static ui2::Page *after_print_page = nullptr;

[[noreturn]] static void bytecodeExecuteTask(void *v) {
    auto stream = static_cast<Stream *>(v);

    auto &w = ui2::Window::getInstance();
    auto &dev = vart::Device::getInstance();

    dev.context.progress = 0;

    w.setPage(printing_page);
    dev.context.quit_code = bytelang::impl::VartInterpreter::getInstance().run(*stream);
    w.setPage(after_print_page);

    vTaskDelete(nullptr);
    while (true) { delay(1); }
}

static void startPrintingTask(Stream &stream) {
    xTaskCreate(
        bytecodeExecuteTask,
        "BL",
        4096,
        &stream,
        1,
        nullptr
    );
};

void selectFilePage(ui2::Page *p, fs::FS &file_sys, const std::function<void()> &on_reload) {
    using ui2::impl::Button;
    using ui2::impl::FileWidget;

    auto reload_func = [p, &file_sys, on_reload]() {
        p->clearWidgets(1);
        on_reload();

        fs::File root = file_sys.open("/");
        fs::File file = root.openNextFile();

        while (file) {
            if (not file.isDirectory()) {
                p->add(new FileWidget(file, startPrintingTask));
            }
            file = root.openNextFile();
        }

        root.close();
    };

    p->add(new Button("Reload", reload_func));
}

void testsPage(ui2::Page *p) {
    using ui2::impl::Button;

    static bytelang::primitive::u8 buf[] = {
        0x01, 0x01, 0xE8, 0x03, 0x02, 0x64, 0x03, 0x4B, 0x07, 0x00, 0x07, 0x01, 0x07, 0x02, 0x07, 0x01, 0x07,
        0x00, 0x04, 0x02, 0x05, 0x64, 0x00, 0x00, 0x00, 0x05, 0x9C, 0xFF, 0x00, 0x00, 0x05, 0x00, 0x00,
        0x00, 0x00, 0x05, 0x00, 0x00, 0x64, 0x00, 0x05, 0x00, 0x00, 0x9C, 0xFF, 0x05, 0x00, 0x00, 0x00,
        0x00, 0x01, 0xE8, 0x03, 0x00
    };
    static bytelang::test::MockStream mock_stream(bytelang::core::MemIO(buf, sizeof(buf)));

    static Button start_mock_task("MockStream", []() {
        mock_stream.reset();
        startPrintingTask(mock_stream);
    });

    p->add(&start_mock_task);

    auto spiffs_reload = []() {
        if (SPIFFS.begin(false)) { return; }
        Serial.println("An Error has occurred while mounting SPIFFS");
    };

    selectFilePage(p->add("SPIFFS"), SPIFFS, spiffs_reload);

    auto sd_reload = []() {
        if (SD.begin(Pins::SdCs)) { return; }
        Serial.println("An Error has occurred while mounting SD");
    };

    selectFilePage(p->add("SD"), SD, sd_reload);
}

static void buildUI(ui2::Page &p) {
    testsPage(p.add("- MEDIA -"));
    workAreaPage(p.add("Work Area"));
    servicePage(p.add("Service"));
    movementPage(p.add("Movement"));
    markerToolPage(p.add("Marker Tool"));

    auto &w = ui2::Window::getInstance();

    printing_page = new ui2::Page(w, "Printing...");
    printingPage(printing_page);

    after_print_page = new ui2::Page(w, "Printing End");
    afterPrint(after_print_page);
}

[[noreturn]] static void uiTaskI(void *) {
    using ui2::Event;

    auto eb = EncButton(Pins::UserEncoderA, Pins::UserEncoderB, Pins::UserEncoderButton);
    auto &w = ui2::Window::getInstance();

    w.addEvent(Event::ForceUpdate);

    while (true) {
        eb.tick();
        if (eb.left()) { w.addEvent(Event::NextWidget); }
        if (eb.right()) { w.addEvent(Event::PreviousWidget); }
        if (eb.rightH()) { w.addEvent(Event::StepUp); }
        if (eb.leftH()) { w.addEvent(Event::StepDown); }
        if (eb.click()) { w.addEvent(Event::Click); }
        taskYIELD();
    }
}

[[noreturn]] static void uiTaskD(void *) {
    auto &display = ui2::impl::OledDisplay::getInstance();
    display.oled.init();
    auto &w = ui2::Window::getInstance(display);

    buildUI(w.root);

    while (true) {
        w.pull();
        taskYIELD();
    }
}

[[noreturn]] static void servoTask(void *) {
    auto &dev = vart::Device::getInstance();
    auto &servo = dev.tool.servo;
    auto &controller = dev.planner.getController();
    const auto update_period_ms = controller.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        controller.update();
        servo.update();
        vTaskDelay(update_period_ms);
        taskYIELD();
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void setup() {
    createStaticTask(uiTaskD, 4096, 1)
    createStaticTask(uiTaskI, 4096, 1)
    createStaticTask(servoTask, 4096, 1)

    SPI.begin(Pins::SdClk, Pins::SdMiso, Pins::SdMosi, Pins::SdCs);
    Serial.begin(115200);
}

void loop() {}

#pragma clang diagnostic pop