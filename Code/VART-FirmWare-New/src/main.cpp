#include <Arduino.h>

#include <TunePID.hpp>
#include "vart/Pins.hpp"

#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/ServoMotor.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace hardware;

/// Максимальный вывод ШИМ
const int max_pwm_out = 255;

/// @brief период Обновления регулятора в микросекундах
const uint32_t update_period_us = 128;

/// @brief Настройки регулятора скорости
const PID::Config speed_config = {
    .kp = 7.7843203545,
    .ki = 4.1978969574,
    .kd = 3.6650085449,
    .max_out = max_pwm_out,
    .max_i = max_pwm_out,
};

const tuner::PidTuner speed_pid_tuner = {
    .min_output = max_pwm_out,
    .max_output = max_pwm_out,
    .mode = PIDAutotuner::Mode::ZNModeBasicPID,
    .loop_period_us = update_period_us,
    .cycles = 10,
};

/// Максимальная скорость в тиках в секунду
const float max_speed_ticks_per_second = 200;

const tuner::PidTuner position_pid_tuner{
    .min_output = -max_speed_ticks_per_second,
    .max_output = max_speed_ticks_per_second,
    .mode = PIDAutotuner::Mode::ZNModeNoOvershoot,
    .loop_period_us = update_period_us,
    .cycles = 50,
};

/// @brief Настройки регулятора позиции
const PID::Config position_config = {
    .kp = 8.4882631302,
    .ki = 0.0355594568,
    .kd = 1337.7160644531,
    .max_out = max_speed_ticks_per_second,
    .max_i = max_speed_ticks_per_second,
};

const ServoMotor::Config servo_config = {
    .update_period_seconds = update_period_us * 1e-6,
    .ready_max_abs_error = 1,
};

auto left_servo = ServoMotor(
    servo_config,
    speed_config,
    position_config,
    MotorDriverL293(vart::Pins::left_driver_a, vart::Pins::left_driver_b),
    Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b));

#define logMsg(msg) Serial.print(msg)

#define logFloat(x)   \
  logMsg(#x " = "); \
  Serial.println(x, 10)

#define logPid(pid)      \
  logMsg(#pid "\n");     \
  logFloat(pid.kp);      \
  logFloat(pid.ki);      \
  logFloat(pid.kd);      \
  logFloat(pid.max_out); \
  logFloat(pid.max_i)

void tunePosition(ServoMotor &servo_motor, float target_position, float speed_limit) {
    servo_motor.setSpeedLimit(speed_limit);

    auto getInput = [&servo_motor]() -> float {
        servo_motor.update();
        return servo_motor.getCurrentSpeed();
    };

    auto setOutput = [&servo_motor](float new_target) {
        servo_motor.setTargetPosition(int32_t(new_target));
    };

    auto position_pid = position_pid_tuner.tune(target_position, getInput, setOutput);
    logPid(position_pid);
}

void tuneSpeed(ServoMotor &servo_motor, float target_speed) {
    const Encoder &encoder = servo_motor.getEncoder();
    const auto dt = servo_motor.getUpdatePeriodSeconds();

    auto getInput = [&encoder, dt]() -> float {
        return encoder.calcSpeed(dt);
    };

    const MotorDriverL293 &driver = servo_motor.getDriver();

    auto setOutput = [&driver](float pwm_dir_set) {
        driver.setPower(int32_t(pwm_dir_set));
    };


    auto speed_pid = speed_pid_tuner.tune(target_speed, getInput, setOutput);


    logPid(speed_pid);
}

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


    left_servo.disable();

}

void loop() {}

#pragma clang diagnostic pop