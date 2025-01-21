#include <Arduino.h>

#include "vart/util/Pins.hpp"

#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/ServoMotor.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace pid;


ServoMotor::Settings servo_config = {
    .update_period_seconds = 128 * 1e-6,
    .delta_regulator_update_period_seconds = 0.010,
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
        .abs_max_out = 800
    },
};


auto left_servo = ServoMotor(
    servo_config,
    MotorDriverL293(vart::Pins::LeftDriverA, vart::Pins::LeftDriverB),
    Encoder(vart::Pins::LeftEncoderA, vart::Pins::LeftEncoderB)
);

//auto right_servo = ServoMotor(
//    servo_config,
//    MotorDriverL293(vart::Pins::RightDriverA, vart::Pins::RightDriverB),
//    Encoder(vart::Pins::RightEncoderA, vart::Pins::RightEncoderB)
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
    const auto update_period = left_servo.getUpdatePeriodMs();

    logMsg("\ngoToPosition\n");
    logFloat(position);
    logFloat(speed);

    left_servo.setTargetPosition(position);
    left_servo.setSpeedLimit(speed);

    left_servo.enable();

    while (not left_servo.isReady()) {
        left_servo.update();
        delayMicroseconds(update_period);
    }

    left_servo.disable();

    logMsg(left_servo.getCurrentPosition());
    logMsg("Done\n\n");
}

void goSpeed(ServoMotor &motor, float speed) {
    const auto update_period = motor.getUpdatePeriodMs();

    logMsg("\ngoSpeed\n");
    logFloat(speed);

    uint32_t end_time_ms = millis() + 6000;

    const float dt = motor.getUpdatePeriodSeconds();
    const double position_update_period_seconds = 0.010;
    const auto pos_update_period_ms = uint32_t(position_update_period_seconds * 1e3);
    const double delta = speed * position_update_period_seconds;

    double target = motor.getCurrentPosition();

    uint32_t next = millis() + pos_update_period_ms;

    while (millis() < end_time_ms) {
        const int32_t current_position = motor.getCurrentPosition();
        const double error = target - current_position;

        if (millis() > next) {
            next = millis() + pos_update_period_ms;
            target += delta;
        }

        motor.driver.setPower(int32_t(motor.delta_position_regulator.calc(error, 0, dt)));

        delayMicroseconds(update_period);
    }

    Serial.printf("%f\t%d\n", target, motor.getCurrentPosition());

    logMsg("Done\n\n");
}

void goSpeedReg(ServoMotor &motor, float speed) {
    const auto update_period = motor.getUpdatePeriodMs();

    logMsg("\ngoSpeed\n");
    logFloat(speed);

    uint32_t end_time_ms = millis() + 6000;

    motor.delta_position_regulator_target = motor.getCurrentPosition();

    while (millis() < end_time_ms) {
        motor.setDriverPowerBySpeed(speed);
        delayMicroseconds(update_period);
    }

    Serial.printf("%f\t%d\n", motor.delta_position_regulator_target, motor.getCurrentPosition());

    logMsg("Done\n\n");
}

void testSpeed(ServoMotor &motor) {
    motor.driver.setPower(255);

    uint32_t end_time_ms = millis() + 6000;

    const double position_update_period_seconds = 0.010;
    const auto pos_update_period_ms = uint32_t(position_update_period_seconds * 1e3);

    uint32_t current_position, last_position = 0, i = 1;
    double avg_speed = 0;

    while (millis() < end_time_ms) {
        current_position = motor.getCurrentPosition();

        avg_speed += double(current_position - last_position);

        last_position = current_position;

        i++;
        delay(pos_update_period_ms);
    }

    avg_speed /= i;

    logFloat(avg_speed);
}

void setup() {
    analogWriteFrequency(30000);
    Serial.begin(115200);

    delay(2000);

    left_servo.enable();

    goSpeedReg(left_servo, 50);
    goSpeedReg(left_servo, -50);
    goSpeedReg(left_servo, -100);
    goSpeedReg(left_servo, 200);
    goSpeedReg(left_servo, -400);
    goSpeedReg(left_servo, 800);

    left_servo.disable();
}

void loop() {}

#pragma clang diagnostic pop