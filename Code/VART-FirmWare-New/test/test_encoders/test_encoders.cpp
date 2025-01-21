#include <Arduino.h>

#include "hardware/Encoder.hpp"
#include "vart/util/Pins.hpp"


const auto left_encoder = pid::Encoder(vart::Pins::LeftEncoderA, vart::Pins::LeftEncoderB);
const auto right_encoder = pid::Encoder(vart::Pins::RightEncoderA, vart::Pins::RightEncoderB);


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
