#pragma once

#include <Arduino.h>
#include <cstdint>


#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "pid/DeltaRegulator.hpp"


namespace hardware {

    /// Сервомотор (Мотор + Обратная связь по энкодеру)
    class ServoMotor {

    public:

        /// Настройки сервопривода
        struct Settings {
            /// Период обновления регулятора в секундах
            const float update_period_seconds;

            /// Максимальная абсолютная ошибка позиционирования
            const uint8_t ready_max_abs_error;

            /// Минимальный лимит скорости
            const float min_speed_limit;

            /// Параметры регулятора скорости
            pid::DeltaRegulatorSettings delta_position;

            /// Параметры регулятора позиции
            pid::RegulatorSettings position;
        };

        /// Настройки
        Settings &settings;

    private:

        /// Регулятор шим по удержанию позиции смещения
        pid::DeltaRegulator delta_position_regulator{settings.delta_position};

        /// Энкодер
        Encoder &encoder;

        /// Драйвер
        const MotorDriverL293 &driver;

        /// Регулятор скорости по положению
        pid::Regulator position_regulator;

        /// Ограничение скорости
        float abs_speed_limit{0};

        /// Целевое положение
        int32_t target_position_ticks{0};

        /// Сервопривод включен
        bool is_enabled{false};


    public:

        explicit ServoMotor(Settings &settings, const MotorDriverL293 &driver, Encoder &encoder) :
            settings(settings),
            encoder(encoder), driver(driver),
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
        void setCurrentPosition(int32_t new_position_ticks) {
            encoder.setPosition(new_position_ticks);
        }

        /// Получить текущее положение
        int32_t getCurrentPosition() const {
            return encoder.getPosition();
        }

        /// Установить ограничение скорости
        void setSpeedLimit(float speed) {
            abs_speed_limit = constrain(speed, settings.min_speed_limit, position_regulator.settings.abs_max_out);
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
        const pid::PidSettings &tuneDeltaPositionRegulator(int32_t target_position) {
            const auto getInput = [this]() -> float {
                return float(this->getCurrentPosition());
            };

            const auto setOutput = [this](float power) -> void {
                this->driver.setPower(int32_t(power));
            };

            return delta_position_regulator.tune(target_position, getInput, setOutput, getUpdatePeriodUs());
        }

        const pid::PidSettings &tunePositionRegulator(int32_t target_position) {
            enable();

            const auto getInput = [this]() -> float {
                return float(this->getCurrentPosition());
            };

            const auto setOutput = [this](float speed) -> void {
                this->setDriverPowerBySpeed(speed);
            };

            beginMove();

            position_regulator.tune(float(target_position), getUpdatePeriodUs(), getInput, setOutput);

            disable();

            return position_regulator.settings.pid;
        }

        /// Обновить регулятор
        void update() {
            if (not is_enabled) { return; }
            setDriverPowerBySpeed(calcSpeedU());
        }

        /// Установить целевое положение регулятора смещения в текущее положение мотора (Вызывать перед началом движения)
        void beginMove() {
            delta_position_regulator.target = getCurrentPosition();
        }

        /// Установить мощность на драйвер по заданной скорости
        void setDriverPowerBySpeed(float speed) {
            driver.setPower(delta_position_regulator.calc(getCurrentPosition(), speed, getUpdatePeriodSeconds()));
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

    };
}  // namespace pid