#pragma once

#include <Arduino.h>
#include <cstdint>


#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "pid/Regulator.hpp"


namespace pid {

    /// Сервомотор (Мотор + Обратная связь по энкодеру)
    class ServoMotor {

    public:

        /// Настройки сервопривода
        struct Settings {
            /// Период обновления регулятора в секундах
            const float update_period_seconds;

            /// Максимальная абсолютная ошибка позиционирования
            const uint8_t ready_max_abs_error;

            /// Параметры регулятора скорости
            RegulatorSettings speed;

            /// Параметры регулятора позиции
            RegulatorSettings position;
        };

        /// Настройки
        const Settings &settings;

    private:

        /// Драйвер
        const MotorDriverL293 driver;

        /// Энкодер
        Encoder encoder;

        /// Регулятор шим по скорости
        Regulator speed_regulator;

        /// Регулятор скорости по положению
        Regulator position_regulator;

        /// Ограничение скорости
        float abs_speed_limit{0};

        /// Целевое положение
        int32_t target_position_ticks{0};

        /// Последняя измеренная скорость
        float calculated_speed{0};

        /// Сервопривод включен
        bool is_enabled{false};

    public:

        explicit ServoMotor(Settings &settings, const MotorDriverL293 &driver, const Encoder &encoder) :
            settings(settings),
            driver(driver),
            encoder(encoder),
            speed_regulator(settings.speed),
            position_regulator(settings.position) {}

        /// Отключить сервопривод
        void disable() {
            encoder.disable();
            driver.setPower(0);
            calculated_speed = 0;
            is_enabled = false;
        }

        /// Включить сервопривод
        void enable() {
            encoder.enable();
            is_enabled = true;
        }

        /// Управление включением сервопривода
        void setEnabled(bool enabled) {
            if (enabled) {
                enable();
            } else {
                disable();
            }
        }

        /// Установить новую целевую позицию
        void setTargetPosition(int32_t new_target_position_ticks) {
            target_position_ticks = new_target_position_ticks;
        }

        /// Установить новое текущее положение
        void setCurrentPosition(int new_position_ticks) {
            encoder.setPosition(new_position_ticks);
        }

        /// Получить текущее положение
        int32_t getCurrentPosition() const {
            return encoder.getPosition();
        }

        /// Установить ограничение скорости
        void setSpeedLimit(float speed) {
            abs_speed_limit = constrain(speed, 0, position_regulator.settings.abs_max_out);
        }

        /// Получить текущую скорость
        float getCurrentSpeed() const {
            return calculated_speed;
        }

        /// Достиг ли позиции?
        bool isReady() const {
            return abs(calcPositionError()) < settings.ready_max_abs_error;
        }

        /// Получить период обновления регулятора в микросекундах
        uint32_t getUpdatePeriodUs() const {
            return uint32_t(getUpdatePeriodSeconds() * 1e6);
        }

        /// Получить период обновления регулятора в секундах
        float getUpdatePeriodSeconds() const {
            return settings.update_period_seconds;
        }

        /// Автоматически настроить регулятор скорости
        void tuneSpeedRegulator(float target_speed) {
            const auto dt = getUpdatePeriodSeconds();

            const auto getInput = [this, dt]() -> float {
                return this->encoder.calcSpeed(dt);
            };

            const auto setOutput = [this](float power) -> void {
                this->driver.setPower(int32_t(power));
            };

            speed_regulator.tune(target_speed, getUpdatePeriodUs(), getInput, setOutput);
        }

        void tunePositionRegulator(int32_t target_position) {
            const auto getInput = [this]() -> float {
                return float(this->getCurrentPosition());
            };

            const auto setOutput = [this](float speed) -> void {
                this->setDriverPowerBySpeed(speed);
            };

            speed_regulator.tune(float(target_position), getUpdatePeriodUs(), getInput, setOutput);
        }

        /// Обновить регулятор
        void update() {
            if (not is_enabled) { return; }
            setDriverPowerBySpeed(calcSpeedU());
        }


        /// Установить мощность на драйвер по заданной скорости
        void setDriverPowerBySpeed(float speed) {
            const float speed_error = calcSpeedError(speed);
            const int32_t &power = int32_t(speed_regulator.calc(speed_error, getUpdatePeriodSeconds()));
            driver.setPower(power);
        }

    private:

        /// Расчитать и ограничить управляющую скорость
        float calcSpeedU() {
            const double speed_u = position_regulator.calc(calcPositionError(), getUpdatePeriodSeconds());
            return constrain(speed_u, -abs_speed_limit, abs_speed_limit);
        }

        /// Получить ошибку позиционирования
        int32_t calcPositionError() const {
            return target_position_ticks - getCurrentPosition();
        }

        /// Расчитать ошибку позиционирования
        float calcSpeedError(float target_speed) {
            calculated_speed = encoder.calcSpeed(getUpdatePeriodSeconds());
            return target_speed - calculated_speed;
        }
    };
}  // namespace pid