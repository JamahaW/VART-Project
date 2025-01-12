#include <hardware/ServoController.hpp>


pid::ServoController::ServoController(uint8_t pin) : pin{pin} {
}

void pid::ServoController::enable() const {
    servo.attach(pin);
}

void pid::ServoController::disable() const {
    servo.detach();
}

void pid::ServoController::setPosition(uint8_t angle) const {
    servo.write(angle);
}
