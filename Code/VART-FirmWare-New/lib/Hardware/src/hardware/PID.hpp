#pragma once

#include "Arduino.h"


namespace hardware {

/// ПИД регулятор
    class PID {
    public:
        /// Параметры регулятора
        struct Config {
            /// KP
            const float kp;

            // KI
            const float ki;

            /// KD
            const float kd;

            /// Модуль максимального выходного значения
            const float max_out;

            /// Модуль максимального значения интеграла
            const float max_i;
        };

        /// Параметры
        const Config &config;

    private:
        /// Интеграл
        mutable double integral{0};

        /// Прошлая ошибка
        mutable double last_error{0};

    public:
        explicit PID(const Config &config) :
            config(config) {}

        double calc(double error, float delta_seconds) const {
            integral += error * delta_seconds;
            integral = constrain(integral, -config.max_i, config.max_i);

            double d_term = (error - last_error) / delta_seconds;
            last_error = error;

            auto ret = (config.kp * error) + (config.ki * integral) + (config.kd * d_term);
            return constrain(ret, -config.max_out, config.max_out);
        }
    };
}  // namespace hardware
