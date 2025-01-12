#include <Arduino.h>

#include "hardware/Encoder.hpp"
#include "vart/Pins.hpp"


const auto left_encoder = pid::Encoder(vart::Pins::left_encoder_a, vart::Pins::left_encoder_b);
const auto right_encoder = pid::Encoder(vart::Pins::right_encoder_a, vart::Pins::right_encoder_b);


void setup() {
    left_encoder.enable();
    right_encoder.enable();

    Serial.begin(9600);
}

void loop() {
    Serial.print(left_encoder.current_position_ticks);
    Serial.print('\t');
    Serial.println(right_encoder.current_position_ticks);
    delay(100);
}
