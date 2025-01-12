#include <Arduino.h>
#include <hal/timer_types.h>
#include <driver/timer.h>

#include "vart/Devices.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


#define logMsg(msg) Serial.print(msg)

#define logFloat(x)   \
  logMsg(#x " = "); \
  Serial.println(x, 10)

#define logPid(pid)      \
  logMsg(#pid "\n");     \
  logFloat(pid.kp);      \
  logFloat(pid.ki);      \
  logFloat(pid.kd);      \
  logFloat(pid.abs_max_i)


void waitToMove(const vart::Pulley &pulley) {
    while (!pulley.isReady()) {
        delay(10);
    }
}

[[noreturn]] void pulleysTask(void *) {
    analogWriteFrequency(30000);
//    const uint32_t us = vart::left_pulley.getUpdatePeriodUs();

    while (true) {
        vart::left_pulley.update();
        vart::right_pulley.update();
        delay(1);
    }
}

// Функция тестирования регулятора
void test_pulley(vart::Pulley &pulley) {
    pulley.setEnabled(true);
    pulley.setSpeedLimit(40);

    logMsg("go 50\n");
    pulley.setTargetRopeLength(50);
    waitToMove(pulley);

    logMsg("go 0");
    pulley.setTargetRopeLength(0);
    waitToMove(pulley);

    pulley.setEnabled(false);
}

#define makeTask(fn, stack_size) do {                                                       \
    static StackType_t stack[stack_size];                                                   \
    static StaticTask_t task_data;                                                          \
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, 10, stack, &task_data, 1);  \
} while(false)

void setup() {
    makeTask(pulleysTask, 4096);

    Serial.begin(115200);

    delay(1000);

    test_pulley(vart::left_pulley);
}

void loop() {
    delay(10);
}

#pragma clang diagnostic pop