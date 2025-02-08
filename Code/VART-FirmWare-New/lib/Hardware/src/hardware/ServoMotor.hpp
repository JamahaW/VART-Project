#pragma once

#include <Arduino.h>
#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "pid/Regulator.hpp"


namespace hardware {

    /// Сервомотор (Удержание положения)
    class ServoMotor {

    public:

        /// Настройки сервопривода
        struct Settings {
            /// Период обновления регулятора в секундах
            float update_period_seconds;
            /// Максимальная абсолютная ошибка позиционирования
            uint8_t ready_max_abs_error;
            /// Параметры регулятора позиции
            pid::Regulator::Settings position;
        };

        /// Настройки
        const Settings &settings;

    private:

        /// Энкодер
        Encoder encoder;
        /// Драйвер
        const MotorDriverL293 driver;
        /// Регулятор ШИМ по положению
        const pid::Regulator position_regulator;
        /// Целевое положение
        int32_t target_position_ticks{0};
        /// Сервопривод включен
        bool is_enabled{false};

    public:

        explicit ServoMotor(const Settings &settings, MotorDriverL293 &&driver, Encoder &&encoder) :
            settings(settings),
            encoder(encoder), driver(driver),
            position_regulator(settings.position) {}

        /// Управление включением сервопривода
        void setEnabled(bool enabled) {
            is_enabled = enabled;
            if (enabled) {
                encoder.enable();
            } else {
                encoder.disable();
                driver.setPower(0);
            }
        }

        /// Установить новую целевую позицию
        void setTargetPosition(int32_t new_target_position_ticks) {
            target_position_ticks = new_target_position_ticks;
        }

        /// Установить новое текущее положение
        void setCurrentPosition(int32_t new_position_ticks) {
            encoder.setPosition(new_position_ticks);
        }

        /// Получить текущее положение
        int32_t getCurrentPosition() const {
            return encoder.getPosition();
        }

        /// Достиг ли позиции?
        bool isReady() const {
            return abs(calcPositionError()) < settings.ready_max_abs_error;
        }

        /// Получить период обновления в миллисекундах
        uint32_t getUpdatePeriodMs() const {
            return uint32_t(getUpdatePeriodSeconds() * 1e3);
        }

        /// Получить период обновления регулятора в секундах
        float getUpdatePeriodSeconds() const {
            return settings.update_period_seconds;
        }

        /// Обновить регулятор
        void update() {
            if (is_enabled) {
                driver.setPower(int32_t(position_regulator.calc(calcPositionError(), getUpdatePeriodSeconds())));
            }
        }

    private:

        /// Получить ошибку позиционирования
        int32_t calcPositionError() const {
            return target_position_ticks - getCurrentPosition();
        }

    };
}  // namespace pid