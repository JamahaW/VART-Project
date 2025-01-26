//#define  FS_NO_GLOBALS
//
#include <Arduino.h>
#include "vart/Devices.hpp"
#include "vart/util/Pins.hpp"
#include "misc/Macro.hpp"
#include "VartUI.hpp"
//
//
//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
//
//
//static ui::Page *printing_page = nullptr;
//
//static ui::Page *after_print_page = nullptr;
//
//[[noreturn]] static void bytecodeExecuteTask(void *v) {
//    auto &stream = *(Stream *) v;
//
//    vart::window.setPage(printing_page);
//
//    vart::context.quit_code = vart::interpreter.run(stream, vart::context);
//
//    vart::window.setPage(after_print_page);
//
//    vTaskDelete(nullptr);
//    while (true) { delay(1); }
//}
//
//static void startPrintingTask(Stream &stream) {
//    xTaskCreate(
//        bytecodeExecuteTask,
//        "BL",
//        4096,
//        &stream,
//        1,
//        nullptr
//    );
//};
//
//void selectFilePage(
//    ui::Page *p,
//    fs::FS &file_sys,
//    const std::function<void()> &on_reload
//) {
//    static fs::File bytecode_stream;
//
//    p->addItem(ui::button("Reload", [p, &file_sys, on_reload](ui::Widget *w) {
//
//        on_reload();
//
//        p->clearItems();
//        p->addItem(w);
//
//        fs::File root = file_sys.open("/");
//        fs::File file = root.openNextFile();
//
//        while (file) {
//            if (not file.isDirectory()) {
//                p->addItem(new ui::FileWidget(file, [&file_sys](ui::Widget *w) {
//                    bytecode_stream = file_sys.open(w->asString());
//                    logMsg("bytecode_stream\n");
//                    startPrintingTask(bytecode_stream);
//                }));
//            }
//
//            file.close();
//            file = root.openNextFile();
//        }
//
//        root.close();
//    }));
//}
//
//void testsPage(ui::Page *p) {
//    static bytelang::primitive::u8 buf[] = {
//        0x01, 0x01, 0xE8, 0x03, 0x02, 0x64, 0x03, 0x4B, 0x07, 0x00, 0x07, 0x01, 0x07, 0x02, 0x07, 0x01, 0x07,
//        0x00, 0x04, 0x02, 0x05, 0x64, 0x00, 0x00, 0x00, 0x05, 0x9C, 0xFF, 0x00, 0x00, 0x05, 0x00, 0x00,
//        0x00, 0x00, 0x05, 0x00, 0x00, 0x64, 0x00, 0x05, 0x00, 0x00, 0x9C, 0xFF, 0x05, 0x00, 0x00, 0x00,
//        0x00, 0x01, 0xE8, 0x03, 0x00
//    };
//    static bytelang::test::MockStream mock_stream(bytelang::core::MemIO(buf, sizeof(buf)));
//
//    p->addItem(ui::button("MockStream", [](ui::Widget *) {
//        mock_stream.reset();
//        startPrintingTask(mock_stream);
//    }));
//
//    auto spiffs_reload = []() {
//        if (SPIFFS.begin(false)) { return; }
//        Serial.println("An Error has occurred while mounting SPIFFS");
//    };
//
//    selectFilePage(p->addPage("SPIFFS"), SPIFFS, spiffs_reload);
//
//    auto sd_reload = []() {
//        if (SD.begin(vart::Pins::SdCs)) { return; }
//        Serial.println("An Error has occurred while mounting SD");
//    };
//
//    selectFilePage(p->addPage("SD"), SD, sd_reload);
//}
//
//static void buildUI(ui::Page &p) {
//    testsPage(p.addPage("- MEDIA -"));
//    workAreaPage(p.addPage("Area"));
//    servicePage(p.addPage("Service"));
//    movementPage(p.addPage("Movement"));
//    markerToolPage(p.addPage("MarkerTool"));
//
//    printing_page = new ui::Page(vart::window, "Printing...");
//    printingPage(printing_page);
//
//    after_print_page = new ui::Page(vart::window, "Printing End");
//    afterPrint(after_print_page);
//}
//
//[[noreturn]] static void uiTask(void *) {
//    vart::window.display.init();
//
//    buildUI(vart::window.main_page);
//
//    vart::window.render();
//
//    while (true) {
//        vart::window.update();
//        vTaskDelay(1);
//        taskYIELD();
//    }
//}
//
//[[noreturn]] static void posTask(void *) {
//    auto &controller = vart::context.planner.getController();
//    const auto update_period_ms = controller.getUpdatePeriodMs();
//
//    analogWriteFrequency(30000);
//
//    while (true) {
//        controller.update();
//        vTaskDelay(update_period_ms);
//        taskYIELD();
//    }
//}
//
//void setup() {
//    using vart::Pins;
//    SPI.begin(Pins::SdClk, Pins::SdMiso, Pins::SdMosi, Pins::SdCs);
//    Serial.begin(115200);
//
//    createStaticTask(uiTask, 4096, 1)
//    createStaticTask(posTask, 4096, 1)
//}
//
//void loop() {}
//
//#pragma clang diagnostic pop



#include "EncButton.h"

#include "gfx/OLED.hpp"
#include "ui2/abc/Display.hpp"
#include "ui2/Window.hpp"
#include "ui2/Page.hpp"
#include "ui2/impl/Label.hpp"
#include "ui2/impl/Button.hpp"
#include "ui2/impl/SpinBox.hpp"



using vart::Pins;
using ui2::impl::Label;
using ui2::impl::SpinBox;
using uButton = ui2::impl::Button;
using ui2::Event;

static EncButton eb(Pins::UserEncoderA, Pins::UserEncoderB, Pins::UserEncoderButton);


struct OledDisplay : ui2::abc::Display {
    gfx::OLED oled;

    size_t write(uint8_t uint_8) override { return oled.write(uint_8); }

    void setCursor(PixelPosition x, PixelPosition y) override { oled.setCursor(x, y); }

    void clear() override { oled.clear(); }

    void setTextInverted(bool is_inverted) override { oled.setInvertText(is_inverted); }
};

static OledDisplay display;


void cb() {
    Serial.println("Click!");
}

void cbs(int v) {
    Serial.println(v);
}

const SpinBox<int>::Settings s = {
    100,
    200,
    5,
    150
};

static ui2::Page p = {
    .title = "Test Page",
    .cursor = 0,
    .widgets = {
        new Label("Text"),
        new Label("Label"),
        new uButton("Button 1", cb),
        new uButton("Button 2", cb),
        new SpinBox<int>("SpinInt", s, cbs),
        nullptr
    }
};

ui2::Window window = {
    .display = display,
    .current_page = &p
};

void setup() {
    display.oled.init();
    window.onEvent(Event::ForceUpdate);
    Serial.begin(115200);
}

void loop() {
    eb.tick();
    if (eb.left()) { window.onEvent(Event::NextWidget); }
    if (eb.right()) { window.onEvent(Event::PreviousWidget); }
    if (eb.rightH()) { window.onEvent(Event::StepUp); }
    if (eb.leftH()) { window.onEvent(Event::StepDown); }
    if (eb.click()) { window.onEvent(Event::Click); }
}
