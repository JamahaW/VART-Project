#pragma once

#include <cstdint>

#include "ESP32Servo.h"


namespace hardware {
    struct ServoMG90S {
        mutable Servo servo;
        const uint8_t pin;

        /// Включить сервопривод
        void enable() const;

        /// Выключить сервопривод
        void disable() const;
        
        /// Установить угол
        void setAngle(uint8_t angle) const;
    };
} // namespace pid
