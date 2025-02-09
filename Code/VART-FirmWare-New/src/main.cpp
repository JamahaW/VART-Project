#define FS_NO_GLOBALS

#include <Arduino.h>
#include <SPIFFS.h>
#include <SD.h>
#include <EncButton.h>

#include "misc/Macro.hpp"

#include "vart/Device.hpp"
#include "vart/util/Pins.hpp"

#include "ui2/Window.hpp"
#include "ui2/abc/Page.hpp"
#include "ui2/impl/Page.hpp"
#include "ui2/impl/widget/FileWidget.hpp"
#include "ui2/impl/display/OledDisplay.hpp"

#include "bytelang/impl/VartInterpreter.hpp"
#include "bytelang/test/MockStream.hpp"
#include "bytelang/core/MemIO.hpp"
#include "ui2/impl/widget/Button.hpp"


using vart::Pins;
using fs::File;

using ui2::abc::Page;
using ui2::Window;
using ui2::impl::widget::FileWidget;
using ui2::impl::display::OledDisplay;

using bytelang::primitive::u8;
using bytelang::core::MemIO;
using bytelang::impl::VartInterpreter;
using bytelang::test::MockStream;

static ui2::impl::page::PrintingPage printing_page;

static ui2::impl::page::WorkOverPage work_over_page;

[[noreturn]] static void bytecodeExecuteTask(void *v) {
    auto stream = static_cast<Stream *>(v);

    auto &dev = vart::Device::getInstance();
    dev.context.progress = 0;
    dev.context.quit_code = VartInterpreter::getInstance().run(*stream);

    Window::getInstance().setPage(work_over_page);

    vTaskDelete(nullptr);
    while (true) { delay(1); }
}

static void startPrintingTask(Stream &stream) {
    Window::getInstance().setPage(printing_page);

    xTaskCreate(
        bytecodeExecuteTask,
        "BL",
        4096,
        &stream,
        1,
        nullptr
    );
};

static void startPrintingFromFile(const fs::File &open_file) {
    static fs::File file;
    file = open_file;
    startPrintingTask(file);
}

static void startPrintingFromMockStream() {
    static u8 buf[] = {
        0x01, 0x01, 0xE8, 0x03, 0x02, 0x64, 0x03, 0x4B, 0x07, 0x00, 0x07, 0x01, 0x07, 0x02, 0x07, 0x01, 0x07,
        0x00, 0x04, 0x02, 0x05, 0x64, 0x00, 0x00, 0x00, 0x05, 0x9C, 0xFF, 0x00, 0x00, 0x05, 0x00, 0x00,
        0x00, 0x00, 0x05, 0x00, 0x00, 0x64, 0x00, 0x05, 0x00, 0x00, 0x9C, 0xFF, 0x05, 0x00, 0x00, 0x00,
        0x00, 0x01, 0xE8, 0x03, 0x00
    };
    static MockStream mock_stream(MemIO(buf, sizeof(buf)));

    mock_stream.reset();
    startPrintingTask(mock_stream);
}

static void reloadFileList(Page &page, fs::FS &file_sys) {
    page.clearWidgets();

    File root = file_sys.open("/");
    File file = root.openNextFile();

    while (file) {
        if (not file.isDirectory()) {
            page.add(new FileWidget(file_sys, file, startPrintingFromFile));
        }
        file.close();
        file = root.openNextFile();
    }

    file.close();
    root.close();
}

static void reloadSpiffsPage(Page &p) {
    if (not SPIFFS.begin(false)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    reloadFileList(p, SPIFFS);
};

static void reloadSdPage(Page &p) {
    if (not SD.begin(Pins::SdCs)) {
        Serial.println("An Error has occurred while mounting SD");
        return;
    }

    reloadFileList(p, SD);
};

void mediaPage(Page *p) {
    using ui2::impl::widget::Button;

    static Button start_mock_task("MockStream", startPrintingFromMockStream);
    p->add(&start_mock_task);

    p->add(new Page("SD", reloadSdPage));
    p->add(new Page("SPIFFS", reloadSpiffsPage));
}

static void buildUI(Page &p) {
    mediaPage(p.add(new Page("- MEDIA -")));
    p.add(new ui2::impl::page::WorkAreaPage());
    p.add(new ui2::impl::page::MovementServicePage());
    p.add(new ui2::impl::page::ToolServicePage());

    p.add(new Page("Dev", [](Page &p) { Serial.println(p.title.text); }));
}

[[noreturn]] static void uiTaskI(void *) {
    using ui2::Event;

    auto eb = EncButton(Pins::UserEncoderA, Pins::UserEncoderB, Pins::UserEncoderButton);
    auto &w = Window::getInstance();

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
    auto &display = OledDisplay::getInstance();
    display.oled.init();
    auto &w = Window::getInstance(display);

    buildUI(w.root);

    while (true) {
        w.pull();
        taskYIELD();
    }
}

[[noreturn]] static void servoTask(void *) {
    auto &dev = vart::Device::getInstance();
    auto &controller = dev.planner.getController();
    const auto update_period_ms = controller.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        controller.update();
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