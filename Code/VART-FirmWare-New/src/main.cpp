#include <Arduino.h>

#include <pidautotuner.h>

#include "vart/Pins.hpp"

#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"
#include "hardware/ServoMotor.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace hardware;


PID::Config speed_config = {
    .kp = 7.7843203545,
    .ki = 4.1978969574,
    .kd = 3.6650085449,
    .max_out = 255,
    .max_i = 255,
};

// tuner.getKp() = 8.4882631302
//tuner.getKi() = 0.0355594568
//tuner.getKd() = 1337.7160644531

PID::Config position_config = {
    .kp = 8.4882631302,
    .ki = 0.0355594568,
    .kd = 1337.7160644531,
    .max_out = 150,
    .max_i = 150,
};

ServoMotor::Config servo_config = {
    .update_period_seconds = 400 * 1e-6,
    .ready_max_abs_error = 1,
    .max_speed = 150,
};

auto left_servo = ServoMotor(
    servo_config,
    speed_config,
    position_config,
    MotorDriverL293(vart::Pins::left_driver_a, vart::Pins::left_driver_b),
    Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b)
);


#define log(x) Serial.print(#x " = "); Serial.println(x, 10)


void tunePosition(float target_position, int32_t loop_period_us) {
    const auto max_speed = 150;
    uint32_t next_update_us;

    auto tuner = PIDAutotuner();

    log(target_position);

    tuner.setTargetInputValue(target_position);
    tuner.setLoopInterval(loop_period_us);
    tuner.setOutputRange(-max_speed, max_speed);
    tuner.setZNMode(PIDAutotuner::ZNModeNoOvershoot);
//    tuner.setTuningCycles(1000);

    tuner.startTuningLoop(micros());

    while (!tuner.isFinished()) {
        auto input = float(left_servo.encoder.getPosition());

        float output = tuner.tunePID(input, micros());
        left_servo.setSpeedLimit(output);
        left_servo.update();

        next_update_us = micros() + loop_period_us;
        while (micros() < next_update_us) { delayMicroseconds(1); }
    }

    left_servo.driver.setPower(0);

    log(tuner.getKp());
    log(tuner.getKi());
    log(tuner.getKd());

    delay(100);
}

void tuneSpeed(float target_input_value) {
    const auto loop_period_us = 400;

    auto tuner = PIDAutotuner();

    uint32_t next_update_us;

    log(target_input_value);

    tuner.setTargetInputValue(target_input_value);
    tuner.setLoopInterval(loop_period_us);
    tuner.setOutputRange(-255, 255);
    tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);
    tuner.setTuningCycles(1000);

    tuner.startTuningLoop(micros());

    uint32_t last = micros();

    while (!tuner.isFinished()) {
        float dt = float(micros() - last) * 1e-6F;

        auto input = left_servo.encoder.calcSpeed(dt);
        last = micros();

        double output = tuner.tunePID(input, micros());
        left_servo.driver.setPower(int32_t(output));

        next_update_us = micros() + loop_period_us;
        while (micros() < next_update_us) { delayMicroseconds(1); }
    }

    left_servo.driver.setPower(0);

    log(tuner.getKp());
    log(tuner.getKi());
    log(tuner.getKd());

    delay(100);
}

void goTimeReg(float target_speed) {
    const auto update_period = 400;

    log(target_speed);
    left_servo.setSpeedLimit(target_speed);

    uint32_t end_time = millis() + 5000;
    while (millis() < end_time) {
        left_servo.update();
        delayMicroseconds(update_period);
    }

    left_servo.driver.setPower(0);
}

void goDist(int32_t target) {
    while (not left_servo.isReady()) {

    }
}

float calcSpeed(int pwm) {
    const uint32_t period_ms = 50;
    const float period_seconds = period_ms * 0.001;

    const uint32_t run_time_ms = 10000;
    const float iterations = float(run_time_ms) / period_ms;

    uint32_t end_time = millis() + run_time_ms;

    float sum = 0;

    left_servo.driver.setPower(pwm);
    /// MAX SPEED = 1894
    while (millis() < end_time) {
        delay(period_ms);
        const float speed = left_servo.encoder.calcSpeed(period_seconds);
        sum += speed;
        log(speed);
    }

    left_servo.driver.setPower(0);

    float result_speed = sum / iterations;

    return result_speed;
}

void setup() {
    left_servo.encoder.enable();
    analogWriteFrequency(30000);
    Serial.begin(9600);

    delay(1000);

    const auto loop_period_us = 128;

    for (int target = 0, delta = 1; delta < 200; delta += 40, target += delta) {
        log(delta);
        tunePosition(float(target), loop_period_us);
    }

    left_servo.encoder.disable();
}


void loop() {}

#pragma clang diagnostic pop