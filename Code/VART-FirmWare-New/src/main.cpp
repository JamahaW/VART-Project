#include <Arduino.h>


#define  FS_NO_GLOBALS

#include <SPIFFS.h>
#include "vart/Devices.hpp"

#include "ui/Factory.hpp"

#include "bytelang/test/MockStream.hpp"
#include "ui/FileWidget.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


using M = vart::Planner::Mode;

using Marker = vart::MarkerPrintTool::Marker;

static void movementPage(ui::Page *p) {
    static int tx = 0, ty = 0;
#define addModeButton(page, mode)\
    page->addItem(ui::button( #mode, [](ui::Widget *) { vart::context.planner.setMode(mode); }))

    p->addItem(ui::button("Move", [](ui::Widget *) {
        vart::context.planner.moveTo((const vart::Vector2D) {double(tx), double(ty)});
    }));

    p->addItem(ui::spinBox(&tx, 50, nullptr, 600, -600));
    p->addItem(ui::spinBox(&ty, 50, nullptr, 600, -600));

    addModeButton(p, M::Position);
    addModeButton(p, M::Speed);
    addModeButton(p, M::Accel);

    static int accel = int(vart::context.planner.getAccel());
    static int speed = int(vart::context.planner.getSpeed());

    void (*spin)(ui::Widget *) = [](ui::Widget *) {
        vart::context.planner.setSpeed(speed);
        vart::context.planner.setAccel(accel);
    };

    p->addItem(ui::spinBox(&speed, 25, spin, 1000, 0));
    p->addItem(ui::spinBox(&accel, 5, spin, 1000, 0));
}

static void servicePage(ui::Page *p) {
    auto &controller = vart::context.planner.getController();
    p->addItem(ui::button("enable", [&controller](ui::Widget *) { controller.setEnabled(true); }));
    p->addItem(ui::button("disable", [&controller](ui::Widget *) { controller.setEnabled(false); }));
    p->addItem(ui::button("setHome", [&controller](ui::Widget *) { controller.setCurrentPositionAsHome(); }));
    p->addItem(ui::button("pullOut", [&controller](ui::Widget *) { controller.pullRopesOut(); }));
    p->addItem(ui::button("pullIn", [&controller](ui::Widget *) { controller.pullRopesIn(); }));

    static int l, r;

    auto on_spin = [&controller](ui::Widget *) {
        controller.setOffsets(l, r);
        controller.setTargetPosition(controller.getCurrentPosition());
    };
    p->addItem(ui::spinBox(&l, 5, on_spin, 100, -100));
    p->addItem(ui::spinBox(&r, 5, on_spin, 100, -100));
}

static ui::Page *printing_page = nullptr;

static ui::Page *after_print = nullptr;

static void startPrintingTask(Stream &stream);

[[noreturn]] static void bytecodeExecuteTask(void *v) {
    auto &stream = *(bytelang::test::MockStream *) v;

    vart::window.setPage(printing_page);

    vart::context.quit_code = vart::interpreter.run(stream, vart::context);

    vart::window.setPage(after_print);

    vTaskDelete(nullptr);
    while (true) { delay(1); }
}

static void startPrintingTask(Stream &stream) { xTaskCreate(bytecodeExecuteTask, "BL", 4096, &stream, 1, nullptr); };

static void uiSelectFile(ui::Page *p, fs::FS &file_sys) {
    p->addItem(ui::button("RELOAD LIST", [p, &file_sys](ui::Widget *w) {
        p->clearItems();
        p->addItem(w);

        fs::File root = file_sys.open("/");

        if (not root) {
            return;
        }

        fs::File file = root.openNextFile();

        while (file) {
            if (not file.isDirectory()) {
                p->addItem(new ui::FileWidget(file, [](ui::Widget *w) {
                    Serial.println((const char *) w->value);
                }));
            }

            file.close();
            file = root.openNextFile();
        }

        file.close();
        root.close();
    }));
}

static void testsPage(ui::Page *p) {
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

    uiSelectFile(p->addPage("SPIFFS"), SPIFFS);
}

static void printingPage(ui::Page *p) {
    p->addItem(ui::button("ABORT", [](ui::Widget *) { vart::interpreter.abort(); }));
    p->addItem(ui::button("PAUSE", [](ui::Widget *) { vart::interpreter.setPaused(true); }));
    p->addItem(ui::button("RESUME", [](ui::Widget *) { vart::interpreter.setPaused(false); }));

    p->addItem(ui::label("Progress"));
    p->addItem(ui::display(&vart::context.progress, ui::ValueType::Integer));
}

static void afterPrint(ui::Page *p) {
    p->addItem(ui::button("TO MAIN", [](ui::Widget *) { vart::window.setPage(&vart::window.main_page); }));

    p->addItem(ui::label("QUIT_CODE"));
    p->addItem(ui::display(&vart::context.quit_code, ui::ValueType::Integer));
}

static void markerToolPage(ui::Page *p) {

#define makeAngleWidget(m)  {\
    static int v = vart::context.tool.getToolAngle(m);\
    p->addItem(ui::label(#m));\
    p->addItem(ui::spinBox(&v, 2, [](ui::Widget *) {vart::context.tool.updateToolAngle(m, v);vart::context.tool.setActiveTool(m);}, 180));\
}

    static int d = int(vart::context.tool.getChangeDuration());

    p->addItem(ui::button("enable", [](ui::Widget *) { vart::context.tool.setEnabled(true); }));
    p->addItem(ui::button("disable", [](ui::Widget *) { vart::context.tool.setEnabled(false); }));
    makeAngleWidget(Marker::Left)
    makeAngleWidget(Marker::Right)
    makeAngleWidget(Marker::None)

    p->addItem(ui::spinBox(&d, 50, [](ui::Widget *) { vart::context.tool.setChangeDuration(d); }, 10000));
}

static void buildUI(ui::Page &p) {
    servicePage(p.addPage("Service"));
    movementPage(p.addPage("Movement"));
    markerToolPage(p.addPage("MarkerTool"));
    testsPage(p.addPage("Tests"));

    printing_page = new ui::Page(vart::window, "Printing...");
    printingPage(printing_page);

    after_print = new ui::Page(vart::window, "Printing End");
    afterPrint(after_print);
}

[[noreturn]] static void uiTask(void *) {
    vart::window.display.init();

    ui::Page &page = vart::window.main_page;
    buildUI(page);

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

#define createStaticTask(fn, stack_size, priority) { \
    static StackType_t stack[stack_size];\
    static StaticTask_t task_data;\
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, priority, stack, &task_data, 1);\
}

void setup() {
    Serial.begin(115200);
    createStaticTask(uiTask, 4096, 1)
    createStaticTask(posTask, 4096, 1)

    if (not SPIFFS.begin(false)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

void loop() {}

#pragma clang diagnostic pop