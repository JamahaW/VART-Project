#include <Arduino.h>

#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "vart/util/Pins.hpp"


using namespace hardware;

const auto left_driver = MotorDriverL293(vart::Pins::LeftDriverA, vart::Pins::LeftDriverB);

const auto right_driver = MotorDriverL293(vart::Pins::RightDriverA, vart::Pins::RightDriverB);

const auto left_encoder = Encoder(vart::Pins::LeftEncoderA, vart::Pins::LeftEncoderB);

const auto right_encoder = Encoder(vart::Pins::RightEncoderA, vart::Pins::RightEncoderB);


void move(const MotorDriverL293 &driver, const Encoder &encoder, int power) {
    driver.setPower(power);
    delay(1000);
    Serial.println(encoder.getPosition());
    driver.setPower(0);
}

void setup() {
    left_encoder.enable();
    right_encoder.enable();

    Serial.begin(9600);

    move(left_driver, left_encoder, 255);
    move(left_driver, left_encoder, -255);
    move(right_driver, right_encoder, 255);
    move(right_driver, right_encoder, -255);
}

void loop() {}
