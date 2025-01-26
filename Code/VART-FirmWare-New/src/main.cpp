#define  FS_NO_GLOBALS

#include <Arduino.h>
#include "vart/Devices.hpp"
#include "VartUI.hpp"

#include "vart/util/Pins.hpp"
#include "misc/Macro.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


static ui::Page *printing_page = nullptr;

static ui::Page *after_print_page = nullptr;

[[noreturn]] static void bytecodeExecuteTask(void *v) {
    auto &stream = *(Stream *) v;

    vart::window.setPage(printing_page);

    vart::context.quit_code = vart::interpreter.run(stream, vart::context);

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
    ui::Page *p,
    fs::FS &file_sys,
    const std::function<void()> &on_reload
) {
    static fs::File bytecode_stream;

    p->addItem(ui::button("Reload", [p, &file_sys, on_reload](ui::Widget *w) {

        on_reload();

        p->clearItems();
        p->addItem(w);

        fs::File root = file_sys.open("/");
        fs::File file = root.openNextFile();

        while (file) {
            if (not file.isDirectory()) {
                p->addItem(new ui::FileWidget(file, [&file_sys](ui::Widget *w) {
                    bytecode_stream = file_sys.open(w->asString());
                    logMsg("bytecode_stream\n");
                    startPrintingTask(bytecode_stream);
                }));
            }

            file.close();
            file = root.openNextFile();
        }

        root.close();
    }));
}

void testsPage(ui::Page *p) {
    static bytelang::primitive::u8 buf[] = {
        0x01, 0x01, 0xE8, 0x03, 0x02, 0x64, 0x03, 0x4B, 0x07, 0x00, 0x07, 0x01, 0x07, 0x02, 0x07, 0x01, 0x07,
        0x00, 0x04, 0x02, 0x05, 0x64, 0x00, 0x00, 0x00, 0x05, 0x9C, 0xFF, 0x00, 0x00, 0x05, 0x00, 0x00,
        0x00, 0x00, 0x05, 0x00, 0x00, 0x64, 0x00, 0x05, 0x00, 0x00, 0x9C, 0xFF, 0x05, 0x00, 0x00, 0x00,
        0x00, 0x01, 0xE8, 0x03, 0x00
    };
    static bytelang::test::MockStream mock_stream(bytelang::core::MemIO(buf, sizeof(buf)));

    p->addItem(ui::button("MockStream", [](ui::Widget *) {
        mock_stream.reset();
        startPrintingTask(mock_stream);
    }));

    auto spiffs_reload = []() {
        if (SPIFFS.begin(false)) { return; }
        Serial.println("An Error has occurred while mounting SPIFFS");
    };

    selectFilePage(p->addPage("SPIFFS"), SPIFFS, spiffs_reload);

    auto sd_reload = []() {
        if (SD.begin(vart::Pins::SdCs)) { return; }
        Serial.println("An Error has occurred while mounting SD");
    };

    selectFilePage(p->addPage("SD"), SD, sd_reload);
}

static void buildUI(ui::Page &p) {
    testsPage(p.addPage("- MEDIA -"));
    workAreaPage(p.addPage("Area"));
    servicePage(p.addPage("Service"));
    movementPage(p.addPage("Movement"));
    markerToolPage(p.addPage("MarkerTool"));

    printing_page = new ui::Page(vart::window, "Printing...");
    printingPage(printing_page);

    after_print_page = new ui::Page(vart::window, "Printing End");
    afterPrint(after_print_page);
}

[[noreturn]] static void uiTask(void *) {
    vart::window.display.init();

    buildUI(vart::window.main_page);

    vart::window.render();

    while (true) {
        vart::window.update();
        vTaskDelay(1);
        taskYIELD();
    }
}

[[noreturn]] static void posTask(void *) {
    auto &controller = vart::context.planner.getController();
    const auto update_period_ms = controller.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        controller.update();
        vTaskDelay(update_period_ms);
        taskYIELD();
    }
}

void setup() {
    using vart::Pins;
    SPI.begin(Pins::SdClk, Pins::SdMiso, Pins::SdMosi, Pins::SdCs);
    Serial.begin(115200);

    createStaticTask(uiTask, 4096, 1)
    createStaticTask(posTask, 4096, 1)
}

void loop() {}

#pragma clang diagnostic pop