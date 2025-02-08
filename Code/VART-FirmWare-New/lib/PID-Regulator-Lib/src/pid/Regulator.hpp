#pragma once

#include "Arduino.h"
#include "pid/Tuner.hpp"

/// Модуль пид-регулятора
namespace pid {
    /// Настройки ПИД-регулятора
    struct PidSettings {
        /// KP
        float kp;

        // KI
        float ki;

        /// KD
        float kd;

        /// Модуль максимального значения интеграла
        float abs_max_i;

        /// Расчитать результирующее значение регулятора
        double calc(double p, double i, double d) const {
            return (kp * p) + (ki * i) + (kd * d);
        }
    };

    /// Настройки тюнера
    struct TunerSettings {
        /// Режим настройки
        const TunerMode mode;

        /// Кол-во циклов
        const uint16_t cycles;
    };

    /// Настройки регулятора
    struct RegulatorSettings {
        /// Настройки ПИД-регулятора (Значение может обновляться)
        PidSettings pid;

        /// Настройки тюнера
        const TunerSettings tuner;

        /// Модуль максимального выходного значения
        const float abs_max_out;
    };

    /// Регулятор
    class Regulator {

    public:

        /// Настройки
        const RegulatorSettings &settings;

    private:

        /// Интеграл
        mutable double integral{0};

        /// Прошлая ошибка
        mutable double last_error{0};

    public:
        explicit Regulator(const RegulatorSettings &settings) :
            settings(settings) {}

        double calc(double error, float delta_seconds) const {
            integral += error * delta_seconds;
            integral = constrain(integral, -settings.pid.abs_max_i, settings.pid.abs_max_i);

            double d_term = (error - last_error) / delta_seconds;
            last_error = error;

            double ret = settings.pid.calc(error, integral, d_term);
            return constrain(ret, -settings.abs_max_out, settings.abs_max_out);
        }
    };
}  // namespace pid
