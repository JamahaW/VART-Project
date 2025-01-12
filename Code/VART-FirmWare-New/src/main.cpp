#include <Arduino.h>

#include "vart/Pins.hpp"

#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/ServoMotor.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace pid;


ServoMotor::Settings servo_config = {
    .update_period_seconds = 128 * 1e-6,
    .ready_max_abs_error = 5,
    .speed = {
        .pid = {
            .kp = 5.3517932892,
            .ki = 2,
            .kd = 12,
            .abs_max_i = 255
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 100,
        },
        .abs_max_out = 255
    },
    .position = {
        .pid = {
            .kp = 9,
            .ki = 0.04,
            .kd = 1385,
            .abs_max_i = 75
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 10,
        },
        .abs_max_out = 200
    },
};


auto left_servo = ServoMotor(
    servo_config,
    MotorDriverL293(vart::Pins::left_driver_a, vart::Pins::left_driver_b),
    Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b)
);

//auto right_servo = ServoMotor(
//    servo_config,
//    MotorDriverL293(vart::Pins::right_driver_a, vart::Pins::right_driver_b),
//    Encoder(vart::Pins::right_encoder_a, vart::Pins::right_encoder_b)
//);

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


void goToPosition(int32_t position, float speed) {
    const auto update_period = left_servo.getUpdatePeriodUs();

    logMsg("\ngoToPosition\n");
    logFloat(position);
    logFloat(speed);

    left_servo.enable();

    left_servo.setTargetPosition(position);
    left_servo.setSpeedLimit(speed);

    while (not left_servo.isReady()) {
        left_servo.update();
        delayMicroseconds(update_period);
    }

    logMsg(left_servo.getCurrentPosition());
    logMsg("Done\n\n");

    left_servo.disable();
}

void setup() {
    analogWriteFrequency(30000);
    Serial.begin(9600);
    left_servo.enable();

    delay(2000);

//    left_servo.tuneSpeedRegulator(150);
//    logPid(left_servo.settings.speed.pid);

    goToPosition(2000, 60);
    goToPosition(-2000, 120);

    left_servo.disable();
}

void loop() {}

#pragma clang diagnostic pop