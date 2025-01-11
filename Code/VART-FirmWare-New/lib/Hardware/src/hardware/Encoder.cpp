#include <esp_attr.h>
#include <esp32-hal-gpio.h>

#include <hardware/Encoder.hpp>


hardware::Encoder::Encoder(uint8_t pin_phase_a, uint8_t pin_phase_b) :
    pin_phase_b(pin_phase_b), pin_phase_a(pin_phase_a) {
    pinMode(pin_phase_a, INPUT);
    pinMode(pin_phase_b, INPUT);
}

static void IRAM_ATTR on_encoder_phase_a_rising(void *v) {
    ((hardware::Encoder *) v)->onRisingPhaseA();
}

void hardware::Encoder::enable() const {
    attachInterruptArg(pin_phase_a, on_encoder_phase_a_rising, (void *) this, RISING);
}

void hardware::Encoder::disable() const {
    detachInterrupt(pin_phase_a);
}

int32_t hardware::Encoder::getPosition() const {
    return current_position_ticks;
}

float hardware::Encoder::calcSpeed(float delta_seconds) const {
    const auto ret = float(getPosition() - last_position_ticks) / delta_seconds;
    last_position_ticks = getPosition();
    return ret;
}
