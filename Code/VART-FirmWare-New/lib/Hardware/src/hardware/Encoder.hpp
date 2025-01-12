#pragma once

#include <cstdint>


namespace pid {

    /// Инкрементальный энкодер
    class Encoder {

    private:
        /// Пин фазы А (Основная фаза с прерыванием)
        const uint8_t pin_phase_a;

        /// Пин фазы Б (Второстепенная фаза для определения направления)
        const uint8_t pin_phase_b;

        /// Текущая позиция в тиках
        volatile int32_t current_position_ticks = 0;

        mutable int32_t last_position_ticks{0};

    public:

        explicit Encoder(uint8_t pin_phase_a, uint8_t pin_phase_b);

        /// Получить текущее положение
        int32_t getPosition() const;

        /// Установить текущее положение
        void setPosition(int32_t new_position_ticks);

        /// Расчитать текущую скорость
        float calcSpeed(float delta_seconds) const;

        /// Подключить обработчик прерывания
        void enable() const;

        /// Отключить обработчик прерывания
        void disable() const;

        void IRAM_ATTR onRisingPhaseA() {
            if (digitalRead(pin_phase_b)) {
                current_position_ticks++;
            } else {
                current_position_ticks--;
            }
        }
    };
}