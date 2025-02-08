#pragma once

#include "Arduino.h"

/// Модуль пид-регулятора
namespace pid {
    /// Регулятор
    class Regulator {

    public:

        /// Настройки регулятора
        struct Settings {
            /// Коэффициенты
            float kp, ki, kd;
            /// Модуль максимального значения интеграла
            float abs_max_i;
            /// Модуль максимального выходного значения
            float abs_max_out;

            /// Расчитать результирующее значение регулятора
            double calc(double p, double i, double d) const {
                double ret = (kp * p) + (ki * i) + (kd * d);
                return constrain(ret, -abs_max_out, abs_max_out);
            }
        };

    private:
        /// Настройки
        const Settings &settings;
        /// Интеграл
        mutable double integral{0};
        /// Прошлая ошибка
        mutable double last_error{0};

    public:
        explicit Regulator(const Settings &settings) :
            settings(settings) {}

        double calc(double error, float delta_seconds) const {
            integral += error * delta_seconds;
            integral = constrain(integral, -settings.abs_max_i, settings.abs_max_i);

            double d_term = (error - last_error) / delta_seconds;
            last_error = error;

            return settings.calc(error, integral, d_term);
        }
    };
}
