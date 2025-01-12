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
            RegulatorSettings delta_position;

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

        /// Регулятор шим по удержанию позиции смещения
        Regulator delta_position_regulator;

        /// Регулятор скорости по положению
        Regulator position_regulator;

        /// Ограничение скорости
        float abs_speed_limit{0};

        /// Целевое положение
        int32_t target_position_ticks{0};

        /// Следующая целевая позиция смещения
        double target_delta_position_ticks{0.0};

        /// Сервопривод включен
        bool is_enabled{false};

    public:

        explicit ServoMotor(Settings &settings, const MotorDriverL293 &driver, const Encoder &encoder) :
            settings(settings),
            driver(driver),
            encoder(encoder),
            delta_position_regulator(settings.delta_position),
            position_regulator(settings.position) {}

        /// Отключить сервопривод
        void disable() {
            encoder.disable();
            driver.setPower(0);
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

        /// Автоматически настроить регулятор ШИМ по смещению положения
        const PidSettings &tuneDeltaPositionRegulator(int32_t target_position) {
            const auto getInput = [this]() -> float {
                return float(this->getCurrentPosition());
            };

            const auto setOutput = [this](float power) -> void {
                this->driver.setPower(int32_t(power));
            };

            delta_position_regulator.tune(float(target_position), getUpdatePeriodUs(), getInput, setOutput);

            return settings.delta_position.pid;
        }

        const PidSettings &tunePositionRegulator(int32_t target_position) {
            const auto getInput = [this]() -> float {
                return float(this->getCurrentPosition());
            };

            const auto setOutput = [this](float speed) -> void {
                this->setDriverPowerBySpeed(speed);
            };

            position_regulator.tune(float(target_position), getUpdatePeriodUs(), getInput, setOutput);

            return settings.position.pid;
        }

        /// Обновить регулятор
        void update() {
            if (not is_enabled) { return; }
            setDriverPowerBySpeed(calcSpeedU());
        }


        /// Установить мощность на драйвер по заданной скорости
        void setDriverPowerBySpeed(float speed) {
            const double delta_position_error = calcDeltaPositionError(speed);
            const int32_t &power = int32_t(delta_position_regulator.calc(delta_position_error, getUpdatePeriodSeconds()));
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
        double calcDeltaPositionError(float target_speed) {
            target_delta_position_ticks += target_speed * getUpdatePeriodSeconds();
            return target_delta_position_ticks - getCurrentPosition();
        }
    };
}  // namespace pid