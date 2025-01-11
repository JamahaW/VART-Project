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

PID::Config position_config = {
    .kp = 1,
    .ki = 0,
    .kd = 0,
    .max_out = 1873.10,
    .max_i = 1000,
};

ServoMotor::Config servo_config = {
    .update_period_seconds = 400 * 1e-6,
    .ready_max_abs_error = 2,
    .max_speed = 1894.0F,
};

auto left_servo = ServoMotor(
    servo_config,
    speed_config,
    position_config,
    MotorDriverL293(vart::Pins::left_driver_a, vart::Pins::left_driver_b),
    Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b)
);


#define log(x) Serial.print(#x " = "); Serial.println(x, 10)


void tune(float target_input_value) {
    const auto loop_period_us = 400;
    const auto loop_period_seconds = float(loop_period_us * 1e-6);

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

void goTime(double target_speed) {
    const auto update_period = 400;
    const float dt = update_period * 1e-6;

    log(target_speed);

    uint32_t end_time = millis() + 5000;

    float avg_speed = 0;
    uint32_t it = 0;

    while (millis() < end_time) {
        float current_speed = left_servo.encoder.calcSpeed(dt);
        avg_speed += current_speed;
        it++;

        double speed_error = target_speed - current_speed;

        const auto power_u = int32_t(left_servo.speed_pid.calc(speed_error, dt));
        left_servo.driver.setPower(power_u);

        delayMicroseconds(update_period);
    }

    left_servo.driver.setPower(0);
    avg_speed /= float(it);
    log(avg_speed);
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

//    log(calcSpeed(255));

//    const auto speed = 1873.10F;

    delay(1000);

//    tune(50);
    for (float speed = 60; speed < 160;) {
        goTime(speed);
        speed += 5.0F;
    }

//    goTime(100);

    left_servo.encoder.disable();
}


void loop() {}

#pragma clang diagnostic pop