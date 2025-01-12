#include <Arduino.h>

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


void sweep(vart::Pulley &pulley) {
    pulley.update();

    if (pulley.isReady()) {
        pulley.setTargetRopeLength(-pulley.getCurrentRopeLength());
    }
}

void setup() {
    analogWriteFrequency(30000);
    Serial.begin(115200);

    delay(2000);

    auto &left = vart::left_pulley;
    auto &right = vart::right_pulley;
    static const uint32_t period_us = left.getUpdatePeriodUs();

    left.setTargetRopeLength(50);
    left.setEnabled(true);
    left.setSpeedLimit(20);

    right.setTargetRopeLength(30);
    right.setEnabled(true);
    right.setSpeedLimit(50);

    while (millis() < 20000) {
        sweep(left);
        sweep(right);
        delayMicroseconds(period_us);
    }

    left.setEnabled(false);
    right.setEnabled(false);
}

void loop() {
}

#pragma clang diagnostic pop