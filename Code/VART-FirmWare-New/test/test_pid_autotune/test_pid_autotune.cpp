#include <Arduino.h>

#include "vart/Pins.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"
#include <pidautotuner.h>


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using namespace hardware;

auto driver = MotorDriverL293(vart::left_driver_a, vart::left_driver_b);

auto encoder = Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b);


#define log(x) Serial.print(#x "="); Serial.print(x); Serial.print('\t')


void tune(float target_input_value) {
    const auto loopInterval = 1000;

    auto tuner = PIDAutotuner();

    uint32_t next_update_us;

    log(target_input_value);
    Serial.println("BEGIN");

    tuner.setTargetInputValue(target_input_value);
    tuner.setLoopInterval(loopInterval);
    tuner.setOutputRange(-255, 255);
    tuner.setZNMode(PIDAutotuner::ZNModeLessOvershoot);

    tuner.startTuningLoop(micros());
    while (!tuner.isFinished()) {
        auto input = float(encoder.current_position_ticks);

        double output = tuner.tunePID(input, micros());
        driver.setPower(int32_t(output));

        next_update_us = micros() + loopInterval;
        while (micros() < next_update_us) { delayMicroseconds(1); }
    }

    driver.setPower(0);

    log(target_input_value);
    log(tuner.getKp());
    log(tuner.getKi());
    log(tuner.getKd());

    delay(100);
}

void setup() {
    encoder.enable();
    Serial.begin(9600);

    tune(10000);
    tune(-10000);

    encoder.disable();
}

// tuner.getKp()=15.82     tuner.getKi()=0.41      tuner.getKd()=404.66

void loop() {}

#pragma clang diagnostic pop