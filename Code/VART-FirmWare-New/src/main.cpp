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


void goToPosition(hardware::ServoMotor &servo, int32_t position, float speed) {
    const auto update_period = servo.getUpdatePeriodUs();

    logMsg("\ngoToPosition\n");
    logFloat(position);
    logFloat(speed);

    servo.enable();

    servo.setTargetPosition(position);
    servo.setSpeedLimit(speed);

    servo.enable();
    servo.beginMove();

    while (not servo.isReady()) {
        servo.update();
        delayMicroseconds(update_period);
    }

    servo.disable();

    logMsg(servo.getCurrentPosition());
    logMsg("Done\n\n");

    delay(100);
}

void goSpeedReg(hardware::ServoMotor &motor, float speed) {
    const auto update_period = motor.getUpdatePeriodUs();

    logMsg("\ngoSpeed\n");
    logFloat(speed);

    uint32_t end_time_ms = millis() + 4000;

    motor.beginMove();

    while (millis() < end_time_ms) {
        motor.setDriverPowerBySpeed(speed);
        delayMicroseconds(update_period);
    }

    logMsg("Done\n\n");
}

void setup() {
    analogWriteFrequency(30000);
    Serial.begin(115200);

    delay(2000);

    goToPosition(vart::left_servo, 500, 50);
    goToPosition(vart::left_servo, -500, 100);
    goToPosition(vart::left_servo, 500, 200);
    goToPosition(vart::right_servo, 500, 400);
    goToPosition(vart::right_servo, -500, 800);
}

void loop() {}

#pragma clang diagnostic pop