#include <Arduino.h>
#include <cstdint>
#include "vart/Devices.hpp"

#include "ui/Factory.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


void goTo(int16_t x, int16_t y) {
    auto &controller = vart::position_controller;

    controller.setTargetPosition(x, y);

    while (not controller.isReady()) {
        delay(1);
    }
}

[[noreturn]] static void execDemoTask(void *) {
    const auto len = 100;
    float rad;

    for (int angle = 0; angle < 360; angle += 5) {
        rad = radians(angle);
        goTo(int16_t(len * cos(rad)), int16_t(len * sin(rad)));
    }

    goTo(0, 0);

    vTaskDelete(nullptr);

    while (true) { delay(1); }
}

static void startDemoTask() {
    xTaskCreate(execDemoTask, "demo", 4096, nullptr, 2, nullptr);
}

static void uiMedia(ui::Page *p) {
    p->addItem(ui::button("DEMO", [](ui::Widget *) { startDemoTask(); }));
}

static void uiArea(ui::Page *p) {
    static int32_t w, h;

    vart::position_controller.getAreaSize(w, h);

    p->addItem(ui::label("Width mm"));
    p->addItem(ui::spinBox(&w, 25, nullptr, 2000, 100));

    p->addItem(ui::label("Height mm"));
    p->addItem(ui::spinBox(&h, 25, nullptr, 2000, 100));

    p->addItem(ui::button("Apply", [](ui::Widget *) {
        vart::position_controller.setAreaSize(w, h);
    }));
}

static void uiMovement(ui::Page *p) {
    static int32_t x = 0, y = 0;
    static float speed = 16;

    auto controller = vart::position_controller;

//    p->addItem(ui_checkbox("Enable", [&controller](bool enabled) { controller.setEnabled(enabled); }, true));

    p->addItem(ui::button("disable", [&controller](ui::Widget *) { controller.setEnabled(false); }));
    p->addItem(ui::button("enable", [&controller](ui::Widget *) { controller.setEnabled(true); }));

    auto pos_sub = p->addPage("Position");
    pos_sub->addItem(ui::label("X mm"));
    pos_sub->addItem(ui::spinBox(&x, 10, nullptr, 2000, -2000));
    pos_sub->addItem(ui::label("Y mm"));
    pos_sub->addItem(ui::spinBox(&y, 10, nullptr, 2000, -2000));
    pos_sub->addItem(ui::button("Move", [&controller](ui::Widget *) {
        controller.setTargetPosition(int16_t(x), int16_t(y));
    }));

    p->addItem(ui::label("Speed"));
    p->addItem(ui::spinBoxF(&speed, 5, 50, 5, [&controller](ui::Widget *) {
        controller.setSpeedLimit(speed);
    }));

    p->addItem(ui::button("Set Home", [&controller](ui::Widget *) {
        controller.setCurrentPositionAsHome();
        controller.setTargetPosition(0, 0);
    }));

    auto offset_sub = p->addPage("Offset");
    offset_sub->addItem(ui::label("Offset Left"));
    offset_sub->addItem(ui::spinBox(&controller.left_pulley.rope_offset_mm, 5, nullptr, 150, -150));
    offset_sub->addItem(ui::label("Offset Right"));
    offset_sub->addItem(ui::spinBox(&controller.right_pulley.rope_offset_mm, 5, nullptr, 150, -150));
}

static void uiTool(ui::Page *p) {
    p->addItem(ui_checkbox("Enable", nullptr, true));
}

static void uiSettings(ui::Page *) {}

static void uiAbout(ui::Page *) {}

static void buildUI(ui::Page &p) {
    uiMedia(p.addPage("Media"));
    uiMovement(p.addPage("Movement"));
    uiArea(p.addPage("Area"));
    uiTool(p.addPage("Tool"));
    uiSettings(p.addPage("Settings"));
    uiAbout(p.addPage("About"));
}

[[noreturn]] static void uiTask(void *) {
    vart::window.display.init();

    ui::Page &page = vart::window.main_page;
    buildUI(page);

    while (true) {
        vart::window.update();
        vTaskDelay(5);
        taskYIELD();
    }
}

[[noreturn]] static void posTask(void *) {
    vart::PositionController &pos = vart::position_controller;

    analogWriteFrequency(30000);

    const uint32_t period_us = pos.getUpdatePeriodUs();
    pos.setEnabled(true);

    pos.setSpeedLimit(16);

    while (true) {
        pos.update();
//        delayMicroseconds(period_us);
        vTaskDelay(1);
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

//    vart::position_controller.left_pulley.servo.tuneDeltaPositionRegulator(10);
//    vart::position_controller.left_pulley.servo.tunePositionRegulator(200);
}

void loop() {}

#pragma clang diagnostic pop