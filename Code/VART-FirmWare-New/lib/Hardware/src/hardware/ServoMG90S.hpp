#pragma once

#include <cstdint>

#include "ServoSmooth.h"


namespace hardware {
    class ServoMG90S {

    public:

        struct Settings {
            /// Скорость [deg / s]
            uint8_t speed;

            /// Ускорение [deg / s^2]
            uint8_t accel;
        };

        ServoSmooth smooth;
    private:

        volatile bool is_ready{true};

    public:

        explicit ServoMG90S(const Settings &settings, uint8_t pin) {
            smooth.attach(pin);
            smooth.setAccel(settings.accel);
            smooth.setSpeed(settings.speed);
        }

        /// Включить сервопривод
        void enable() { smooth.start(); }

        /// Обновить регулятор сервопривода
        void update() { is_ready = smooth.tickManual(); }

        /// Сервопривод достиг целевой позиции
        bool isReady() const { return is_ready; }

        void setEnabled(bool enabled) {
            if (enabled) { smooth.start(); }
            else { smooth.stop(); }
        }

        /// Установить угол
        void setAngle(uint8_t angle) {
            smooth.setTargetDeg(angle);
        }
    };
}
