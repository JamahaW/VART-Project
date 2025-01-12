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
    .delta_position = {
        .pid = {
            .kp = 16.1296730042,
            .ki = 0.0492058247,
            .kd = 2244.5698242188,
            .abs_max_i = 204
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 20,
        },
        .abs_max_out = 255
    },
    .position = {
        .pid = {
            .kp = 12.5555553436,
            .ki = 0.0536988042,
            .kd = 1938.7520751953,
            .abs_max_i = 75
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 10,
        },
        .abs_max_out = 100
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

    delay(2000);

    for (int target = 100; target < 2000; target *= -2) {
        logFloat(target);
        left_servo.enable();
        const PidSettings &settings = left_servo.tunePositionRegulator(target);
        logPid(settings);
        left_servo.disable();
        delay(5000);
    }

}

void loop() {}

#pragma clang diagnostic pop