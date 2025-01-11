#include <hardware/ServoController.hpp>


hardware::ServoController::ServoController(uint8_t pin) : pin{pin} {
}

void hardware::ServoController::enable() const {
    servo.attach(pin);
}

void hardware::ServoController::disable() const {
    servo.detach();
}

void hardware::ServoController::setPosition(uint8_t angle) const {
    servo.write(angle);
}
