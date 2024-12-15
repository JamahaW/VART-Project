#include <Arduino.h>

#include <hardware/MotorDriver.hpp>
#include <constants/Pins.hpp>


hardware::MotorDriverL293::MotorDriverL293(uint8_t dir_a, uint8_t dir_b) :
    DIR_A(dir_a), DIR_B(dir_b) {
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
}

void hardware::MotorDriverL293::set(int pwm_dir) const {
    const int pwm = abs(pwm_dir);
    pwm_dir = constrain(pwm_dir, -255, 255);
    analogWrite(DIR_A, (pwm_dir > 0) * pwm);
    analogWrite(DIR_B, (pwm_dir < 0) * pwm);
}

hardware::MotorDriverL293 hardware::MotorDriverL293::makeLeft() {
    return MotorDriverL293(constants::Pins::MOTOR_LEFT_DRIVER_A, constants::Pins::MOTOR_LEFT_DRIVER_B);;
}

hardware::MotorDriverL293 hardware::MotorDriverL293::makeRight() {
    return MotorDriverL293(constants::Pins::MOTOR_RIGHT_DRIVER_A, constants::Pins::MOTOR_RIGHT_DRIVER_B);;
}
