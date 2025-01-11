#include <Arduino.h>

#include <hardware/MotorDriver.hpp>


hardware::MotorDriverL293::MotorDriverL293(uint8_t dir_a, uint8_t dir_b) :
    dir_a(dir_a), dir_b(dir_b) {
    pinMode(dir_a, OUTPUT);
    pinMode(dir_b, OUTPUT);
}

void hardware::MotorDriverL293::setPower(int pwm_dir) const {
    const int pwm = abs(pwm_dir);
    pwm_dir = constrain(pwm_dir, -255, 255);
    analogWrite(dir_a, (pwm_dir > 0) * pwm);
    analogWrite(dir_b, (pwm_dir < 0) * pwm);
}
