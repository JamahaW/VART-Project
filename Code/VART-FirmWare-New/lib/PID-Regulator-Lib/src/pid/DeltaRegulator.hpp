#pragma once

#include <cstdint>
#include "pid/Regulator.hpp"
#include "pid/DeltaRegulator.hpp"


namespace pid {

    /// Настройки регулятора смещения
    struct DeltaRegulatorSettings {

        /// Настройки этого регулятора
        RegulatorSettings regulator;

        /// Период обновления целевой позиции в секундах
        const float update_period_seconds;
    };

    /// Регулятор смещения
    class DeltaRegulator {

    private:
        const DeltaRegulatorSettings &settings;

        /// Сам регулятор
        Regulator regulator;

        /// Период обновления целевой позиции в миллисекундах
        const uint32_t update_period_ms;

        /// Момент обновления целевой позиции в миллисекундах
        uint32_t next_update_time_ms{0};

    public:

        /// Следующая целевая позиция смещения
        double target{0.0};

        explicit DeltaRegulator(DeltaRegulatorSettings &settings) :
            settings(settings), regulator(settings.regulator),
            update_period_ms(uint32_t(settings.update_period_seconds * 1e3)) {}

        const PidSettings &tune(
            int32_t target_position,
            const std::function<float()> &getInput,
            const std::function<void(float)> &setOutput,
            uint32_t loop_period_us
        ) {
            regulator.tune(float(target_position), loop_period_us, getInput, setOutput);
            return settings.regulator.pid;
        }

        int32_t calc(double input, float speed, float period_seconds) {
            if (millis() > next_update_time_ms) {
                next_update_time_ms = millis() + update_period_ms;
                target += speed * settings.update_period_seconds;
            }

            return int32_t(regulator.calc(target - input, period_seconds));
        }
    };
}