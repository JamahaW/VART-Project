#include <hardware/ServoMG90S.hpp>


void hardware::ServoMG90S::enable() const { servo.attach(pin); }

void hardware::ServoMG90S::disable() const { servo.detach(); }

void hardware::ServoMG90S::setAngle(uint8_t angle) const { servo.write(angle); }
