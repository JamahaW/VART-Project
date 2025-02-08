#include <Arduino.h>
#include "vart/Devices.hpp"
#include "misc/Macro.hpp"
#include "VartUI.hpp"

#include "EncButton.h"
#include "vart/util/Pins.hpp"

#include "ui2/impl/FileWidget.hpp"


using vart::Pins;

static ui2::Page *printing_page = nullptr;

static ui2::Page *after_print_page = nullptr;

[[noreturn]] static void bytecodeExecuteTask(void *v) {
    auto stream = static_cast<Stream *>(v);

    vart::context.progress = 0;

    vart::window.setPage(printing_page);

    vart::context.quit_code = vart::interpreter.run(*stream, vart::context);

    vart::window.setPage(after_print_page);

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

void selectFilePage(
    ui2::Page *p,
    fs::FS &file_sys,
    const std::function<void()> &on_reload
) {
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
        if (SD.begin(vart::Pins::SdCs)) { return; }
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

    printing_page = new ui2::Page(vart::window, "Printing...");
    printingPage(printing_page);

    after_print_page = new ui2::Page(vart::window, "Printing End");
    afterPrint(after_print_page);
}

[[noreturn]] static void uiInputTask(void *) {
    using ui2::Event;

    auto eb = EncButton(Pins::UserEncoderA, Pins::UserEncoderB, Pins::UserEncoderButton);
    auto &w = vart::window;

    w.addEvent(ui2::Event::ForceUpdate);

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

[[noreturn]] static void uiDisplayTask(void *) {
    vart::window.display.init();
    buildUI(vart::window.root);

    while (true) {
        vart::window.pull();
        taskYIELD();
    }
}

[[noreturn]] static void servoTask(void *) {
    auto &controller = vart::device.planner.getController();
    const auto update_period_ms = controller.getUpdatePeriodMs();

    auto &servo = vart::device.tool.servo;

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
    SPI.begin(Pins::SdClk, Pins::SdMiso, Pins::SdMosi, Pins::SdCs);
    createStaticTask(uiDisplayTask, 4096, 1)
    createStaticTask(uiInputTask, 4096, 1)
    createStaticTask(servoTask, 4096, 1)

    Serial.begin(115200);
}

void loop() {}

#pragma clang diagnostic pop