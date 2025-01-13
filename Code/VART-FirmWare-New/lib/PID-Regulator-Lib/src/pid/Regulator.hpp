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
        RegulatorSettings &settings;

    private:

        /// Интеграл
        mutable double integral{0};

        /// Прошлая ошибка
        mutable double last_error{0};

    public:
        explicit Regulator(RegulatorSettings &settings) :
            settings(settings) {}

        double calc(double error, float delta_seconds) const {
            integral += error * delta_seconds;
            integral = constrain(integral, -settings.pid.abs_max_i, settings.pid.abs_max_i);

            double d_term = (error - last_error) / delta_seconds;
            last_error = error;

            double ret = settings.pid.calc(error, integral, d_term);
            return constrain(ret, -settings.abs_max_out, settings.abs_max_out);
        }

        /// Автоматическая настройка регулятора
        void tune(
            float target,
            uint32_t loop_period_us,
            const std::function<float()> &getInput,
            const std::function<void(float)> &setOutput
        ) {
            auto tuner = pid::Tuner();
            tunerSetup(tuner, target, loop_period_us);

            logFloat(target);
            logFloat(loop_period_us);

            uint32_t next_update_us;

            const uint32_t end_time_ms = millis() + 20000;

            tuner.startTuningLoop(micros());

            while (not tuner.isFinished()) {
                if (millis() > end_time_ms) {
                    Serial.println("TIMEOUT");
                    break;
                }

                next_update_us = micros() + loop_period_us;

                const float input = getInput();
                logFloat(input);

                const float output = tuner.tunePID(input, micros());
                logFloat(output);

                setOutput(output);

                while (micros() < next_update_us) { delayMicroseconds(1); }
            }

            settings.pid = {
                .kp = tuner.getKp(),
                .ki = tuner.getKi(),
                .kd = tuner.getKd(),
                .abs_max_i = settings.abs_max_out * 0.8F,
            };
        }

        void tunerSetup(Tuner &tuner, float target, uint32_t loop_period_us) const {
            tuner.setLoopInterval(int32_t(loop_period_us));
            tuner.setOutputRange(-settings.abs_max_out, settings.abs_max_out);
            tuner.setZNMode(settings.tuner.mode);
            tuner.setTuningCycles(settings.tuner.cycles);
            tuner.setTargetInputValue(target);
        }
    };
}  // namespace pid
