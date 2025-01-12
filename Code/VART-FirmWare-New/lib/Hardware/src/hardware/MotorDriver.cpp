#include <Arduino.h>

#include <hardware/MotorDriver.hpp>


hardware::MotorDriverL293::MotorDriverL293(uint8_t dir_a, uint8_t dir_b) :
    dir_a(dir_a), dir_b(dir_b) {
    pinMode(dir_a, OUTPUT);
    pinMode(dir_b, OUTPUT);
}

void hardware::MotorDriverL293::setPower(int power) const {
    const int pwm = abs(power);
    power = constrain(power, -255, 255);
    analogWrite(dir_a, (power > 0) * pwm);
    analogWrite(dir_b, (power < 0) * pwm);
}
