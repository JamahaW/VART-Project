#pragma once

#include <cstdint>

#include "ESP32Servo.h"


namespace hardware {
    class ServoMG90S {

    private:

        uint8_t pin;
        Servo backend;

    public:

        /// Примитив для передачи угла
        using Angle = uint8_t;

        explicit ServoMG90S(uint8_t pin) :
            pin{pin} {}

        void setEnabled(bool enabled) {
            if (enabled) { backend.attach(pin); }
            else { backend.detach(); }
        }

        /// Установить угол
        void setAngle(Angle angle) { backend.write(angle); }
    };
}
