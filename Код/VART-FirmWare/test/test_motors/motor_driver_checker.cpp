#include <Arduino.h>


#include "hardware/MotorDriver.hpp"
#include "constants/Pins.hpp"


#define cls_str(__class__) "<" #__class__ ">"

static auto left_driver = hardware::MotorDriverL293(constants::MOTOR_LEFT_DRIVER_A, constants::MOTOR_LEFT_DRIVER_B);

static auto right_driver = hardware::MotorDriverL293(constants::MOTOR_RIGHT_DRIVER_A, constants::MOTOR_RIGHT_DRIVER_B);


static void set_driver_time(const hardware::MotorDriverL293 &driver, int pwm) {
    Serial.printf("Setting PWM %d for 1000 ms\n", pwm);
    driver.set(pwm);
    delay(1000);
    driver.set(0);
    Serial.printf("Stopping\n");
}

static void test_driver(const hardware::MotorDriverL293 &driver) {
    Serial.printf("Testing" cls_str(hardware::MotorDriverL293) " at: %p\n", &driver);
    set_driver_time(driver, 255);
    set_driver_time(driver, -255);
}

void setup() {
    Serial.begin(9600);
    test_driver(left_driver);
    test_driver(right_driver);
}

void loop() {
}
