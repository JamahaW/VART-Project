#include <Arduino.h>

#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "vart/Pins.hpp"


const auto left_driver = pid::MotorDriverL293(vart::Pins::left_driver_a, vart::Pins::left_driver_b);

const auto right_driver = pid::MotorDriverL293(vart::Pins::right_driver_a, vart::Pins::right_driver_b);

const auto left_encoder = pid::Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b);

const auto right_encoder = pid::Encoder(vart::Pins::right_encoder_a, vart::Pins::right_encoder_b);


void move(const pid::MotorDriverL293 &driver, int power) {
    driver.setPower(power);
    delay(1000);
    Serial.print(left_servo.encoder.current_position_ticks);
    Serial.print('\t');
    Serial.println(left_servo.target_position_tick);
    driver.setPower(0);
}

void setup() {
    left_encoder.enable();
    right_encoder.enable();

    Serial.begin(9600);

    move(pid::MotorDriverL293(vart::left_driver_a, vart::left_driver_b), 255);
    move(pid::MotorDriverL293(vart::left_driver_a, vart::left_driver_b), -255);
    move(right_driver, 255);
    move(right_driver, -255);
}

void loop() {}
