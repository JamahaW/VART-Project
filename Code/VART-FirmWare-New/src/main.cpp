#include <Arduino.h>
#include <cstdint>
#include "vart/Devices.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


void goTo(vart::PositionController &controller, int16_t x, int16_t y) {
    controller.setTargetPosition(x, y);
    const uint32_t us = vart::position_controller.getUpdatePeriodUs();

    while (not controller.isReady()) {
        vart::position_controller.update();
        delayMicroseconds(us);
    }
}

#define makeTask(fn, stack_size) do {                                                       \
    static StackType_t stack[stack_size];                                                   \
    static StaticTask_t task_data;                                                          \
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, 10, stack, &task_data, 1);  \
} while(false)

void setup() {
//    makeTask(pulleysTask, 4096);
    analogWriteFrequency(30000);
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

    controller.setEnabled(false);
}

void loop() {
    delay(10);
}

#pragma clang diagnostic pop