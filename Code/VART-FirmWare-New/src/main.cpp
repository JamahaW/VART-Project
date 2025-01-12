#include <Arduino.h>
#include "vart/Devices.hpp"
#include "vart/Macro.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


[[noreturn]] void pulleysTask(void *) {
    analogWriteFrequency(30000);
//    const uint32_t us = vart::left_pulley.getUpdatePeriodUs();

    while (true) {
        vart::position_controller.update();
        delay(1);
    }
}

void waitMove(const vart::PositionController &controller) {
    while (not controller.isReady()) { delay(10); }
}

void goTo(vart::PositionController &controller, int16_t x, int16_t y) {
    controller.setTargetPosition(x, y);
    waitMove(controller);
}

#define makeTask(fn, stack_size) do {                                                       \
    static StackType_t stack[stack_size];                                                   \
    static StaticTask_t task_data;                                                          \
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, 10, stack, &task_data, 1);  \
} while(false)

void setup() {
    makeTask(pulleysTask, 4096);
    Serial.begin(115200);

    delay(5000);

    vart::PositionController &controller = vart::position_controller;
    controller.setEnabled(true);
    controller.setCurrentPositionAsHome();

    controller.setSpeedLimit(5);
    goTo(controller, 0, 100);

    controller.setSpeedLimit(10);
    goTo(controller, 0, -100);

    controller.setSpeedLimit(32);
    goTo(controller, 0, 0);

    delay(2000);

    controller.setSpeedLimit(4);
    goTo(controller, -100, 0);

    controller.setSpeedLimit(16);
    goTo(controller, 0, 0);

    controller.setSpeedLimit(4);
    goTo(controller, 100, 0);

    controller.setSpeedLimit(32);
    goTo(controller, 0, 0);

    controller.setEnabled(false);
}

void loop() {
    delay(10);
}

#pragma clang diagnostic pop