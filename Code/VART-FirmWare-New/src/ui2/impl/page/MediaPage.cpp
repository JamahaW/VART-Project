#include "VartPages.hpp"


#define FS_NO_GLOBALS

#include <SPIFFS.h>
#include <SD.h>

#include "ui2/Window.hpp"
#include "ui2/impl/widget/Builtin.hpp"
#include "ui2/impl/widget/VartWidgets.hpp"

#include "vart/Device.hpp"
#include "vart/util/Pins.hpp"

#include "bytelang/test/MockStream.hpp"
#include "bytelang/impl/VartInterpreter.hpp"


using namespace ui2::impl::page;
using ui2::Window;
using ui2::impl::widget::FileWidget;
using ui2::impl::widget::Button;
using ui2::impl::widget::Text;

using bytelang::primitive::u8;
using bytelang::core::MemIO;
using bytelang::impl::VartInterpreter;
using bytelang::test::MockStream;

using vart::Device;
using vart::Pins;

static PrintingPage printing_page;

static WorkOverPage work_over_page;

static u8 demo[] = {
    0x01, 0x01, 0xE8, 0x03, 0x02, 0x64, 0x03, 0x4B,
    0x07, 0x00, 0x07, 0x01, 0x07, 0x02, 0x07, 0x01,
    0x07, 0x00, 0x04, 0x02, 0x05, 0x64, 0x00, 0x00,
    0x00, 0x05, 0x9C, 0xFF, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x64, 0x00,
    0x05, 0x00, 0x00, 0x9C, 0xFF, 0x05, 0x00, 0x00,
    0x00, 0x00, 0x01, 0xE8, 0x03, 0x00
};

static MockStream mock_stream(MemIO(demo, sizeof(demo)));

static void bytecodeExecuteTask(void *v) {
    auto stream = static_cast<Stream *>(v);

    auto &context = Device::getInstance().context;
    context.progress = 0;
    context.quit_code = VartInterpreter::getInstance().run(*stream);

    Window::getInstance().setPage(work_over_page);

    vTaskDelete(nullptr);
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
    mock_stream.reset();
    startPrintingTask(mock_stream);
}

static void reloadFileList(Page &page, FS &file_sys) {
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

ui2::impl::page::MediaPage::MediaPage() :
    Page("Media") {
    add(new Button(Text("MockStream"), startPrintingFromMockStream));
    add(new Page("SD", reloadSdPage));
    add(new Page("SPIFFS", reloadSpiffsPage));
}
