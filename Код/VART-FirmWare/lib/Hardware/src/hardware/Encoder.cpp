#include <esp_attr.h>
#include <esp32-hal-gpio.h>

#include <hardware/Encoder.hpp>
#include "constants/Pins.hpp"


hardware::Encoder::Encoder(uint8_t a, uint8_t b) :
    PIN_B(b), PIN_A(a) {
    pinMode(a, INPUT);
    pinMode(b, INPUT);
}

static void IRAM_ATTR encoder_process(void *v) {
    auto *e = (hardware::Encoder *) v;

    if (digitalRead(e->PIN_B)) {
        e->ticks++;
    } else {
        e->ticks--;
    }
}

void hardware::Encoder::attach() const {
    attachInterruptArg(PIN_A, encoder_process, (void *) this, RISING);
}

hardware::Encoder hardware::Encoder::makeLeft() {
    return Encoder(constants::Pins::MOTOR_LEFT_ENCODER_A, constants::Pins::MOTOR_LEFT_ENCODER_B);;
}

hardware::Encoder hardware::Encoder::makeRight() {
    return Encoder(constants::Pins::MOTOR_RIGHT_ENCODER_A, constants::Pins::MOTOR_RIGHT_ENCODER_B);;
}
