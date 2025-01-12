#include <hardware/ServoController.hpp>


void hardware::ServoController::enable() const {
    servo.attach(pin);
}

void hardware::ServoController::disable() const {
    servo.detach();
}

void hardware::ServoController::setPosition(uint8_t angle) const {
    servo.write(angle);
}
