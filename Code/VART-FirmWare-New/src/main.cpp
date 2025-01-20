#include <Arduino.h>
#include "vart/Devices.hpp"

#include "ui/Factory.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using vart::Vector2D;


static void goTo(const Vector2D target) { vart::planner.moveTo(target); }

using M = vart::Planner::Mode;

#define addModeButton(page, mode)\
    page->addItem(ui::button( #mode, [](ui::Widget *) { vart::planner.setMode(mode); }))

static void movementPage(ui::Page *p) {
    static int tx = 0, ty = 0;
    p->addItem(ui::button("Move", [](ui::Widget *) {
        goTo({double(tx), double(ty)});
    }));

    p->addItem(ui::spinBox(&tx, 50, nullptr, 600, -600));
    p->addItem(ui::spinBox(&ty, 50, nullptr, 600, -600));

    addModeButton(p, M::Immediate);
    addModeButton(p, M::Speed);
    addModeButton(p, M::Accel);

    static int accel = int(vart::planner.getAccel());
    static int speed = int(vart::planner.getSpeed());

    void (*spin)(ui::Widget *) = [](ui::Widget *) {
        vart::planner.setSpeed(speed);
        vart::planner.setAccel(accel);
    };
    p->addItem(ui::spinBox(&speed, 25, spin, 1000, 0));
    p->addItem(ui::spinBox(&accel, 5, spin, 1000, 0));
}

static void ServicePage(ui::Page *p) {
    p->addItem(ui::button("enable", [](ui::Widget *) { vart::planner.controller.setEnabled(true); }));
    p->addItem(ui::button("disable", [](ui::Widget *) { vart::planner.controller.setEnabled(false); }));
    p->addItem(ui::button("setHome", [](ui::Widget *) { vart::planner.controller.setCurrentPositionAsHome(); }));
    p->addItem(ui::button("pullOut", [](ui::Widget *) { vart::planner.controller.pullRopesOut(); }));
    p->addItem(ui::button("pullIn", [](ui::Widget *) { vart::planner.controller.pullRopesIn(); }));
}


static void TestsPage(ui::Page *p) {
    static int size = 200;
    p->addItem(ui::spinBox(&size, 25, nullptr, 500, 0));

    p->addItem(ui::button("DemoRect", [](ui::Widget *) {
        const auto s = double(size);
//        vart::planner.setMode(M::Accel);

        goTo({s, s});
        goTo({-s, s});
        goTo({-s, -s});
        goTo({s, -s});
        goTo({s, s});
        goTo({0, 0});
    }));

    p->addItem(ui::button("DemoCircle", [](ui::Widget *) {
        const auto r = double(size);
        double x, y;

//        vart::planner.setMode(M::Immediate);

        for (int deg = 0; deg <= 360; deg += 1) {
            x = r * std::cos(radians(deg));
            y = r * std::sin(radians(deg));
            goTo({x, y});
        }

        goTo({0, 0});
    }));
}

static void buildUI(ui::Page &p) {
    ServicePage(p.addPage("Service"));
    movementPage(p.addPage("Movement"));
    TestsPage(p.addPage("Tests"));
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
    const auto update_period_ms = vart::planner.controller.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        vart::planner.controller.update();
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
}

void loop() {}

#pragma clang diagnostic pop