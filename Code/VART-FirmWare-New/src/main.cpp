#include <Arduino.h>
#include "vart/Devices.hpp"

#include "ui/Factory.hpp"

#include "bytelang/test/MockStream.hpp"


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

[[noreturn]] static void bytecodeExecute(void *) {
    using bytelang::core::MemIO;

    logMsg("bytecodeExecute begin\n");

    static bytelang::primitive::u8 buf[] = {
        0x01, 0x01, 0xE8, 0x03, 0x02, 0x64, 0x03, 0xC8, 0x04, 0x00, 0x04, 0x01, 0x04, 0x02, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0xE8, 0x03, 0xE8, 0x03, 0x05, 0x18, 0xFC, 0x18, 0xFC, 0x06, 0x00, 0x06,
        0x32, 0x06, 0x64, 0x07, 0x00, 0x07, 0x01, 0x07, 0x02, 0x00
    };
    logMsg("Buffer created\n");

    auto stream = bytelang::test::MockStream(MemIO(buf, sizeof(buf)));
    logMsg("Stream created\n");

    logMsg("BEGIN vart::interpreter.run\n");
    auto ret = vart::interpreter.run(stream, vart::context);
    logInt(ret);
    logMsg("END vart::interpreter.run\n");

    logMsg("bytecodeExecute end\n");

    vTaskDelete(nullptr);
    while (true) { delay(1); }
}

static void execStart(ui::Widget *) {
    xTaskCreate(bytecodeExecute, "BL", 4096, nullptr, 1, nullptr);
};

static void testsPage(ui::Page *p) {
    static int size = 200;
    auto &planner = vart::context.planner;
    auto &tool = vart::context.tool;

    p->addItem(ui::spinBox(&size, 25, nullptr, 500, 0));
    p->addItem(ui::button("DemoRect", [&planner, &tool](ui::Widget *) {
        const auto s = double(size);

        planner.setMode(M::Accel);

        tool.setActiveTool(Marker::None);
        planner.moveTo((const vart::Vector2D) {s, s});

        tool.setActiveTool(Marker::Left);
        planner.moveTo((const vart::Vector2D) {-s, s});
        planner.moveTo((const vart::Vector2D) {-s, -s});
        planner.moveTo((const vart::Vector2D) {s, -s});
        planner.moveTo((const vart::Vector2D) {s, s});

        tool.setActiveTool(Marker::None);
        planner.moveTo((const vart::Vector2D) {0, 0});
    }));
    p->addItem(ui::button("DemoCircle", [&planner, &tool](ui::Widget *) {
        const auto r = double(size);
        double x, y;

        planner.setMode(M::Accel);
        tool.setActiveTool(Marker::None);
        planner.moveTo((const vart::Vector2D) {r, 0});

        planner.setMode(M::Speed);
        tool.setActiveTool(Marker::Left);

        for (int deg = 1; deg <= 360; deg += 1) {
            x = r * std::cos(radians(deg));
            y = r * std::sin(radians(deg));
            planner.moveTo((const vart::Vector2D) {x, y});
        }

        planner.setMode(M::Accel);
        tool.setActiveTool(Marker::None);
        planner.moveTo((const vart::Vector2D) {0, 0});
    }));

    p->addItem(ui::button("Execute", execStart));
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
}

[[noreturn]] static void uiTask(void *) {
    vart::window.display.init();

    ui::Page &page = vart::window.main_page;
    buildUI(page);

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
//    createStaticTask(uiTask, 4096, 1)
//    createStaticTask(posTask, 4096, 1)

    execStart(nullptr);
}

void loop() {}

#pragma clang diagnostic pop