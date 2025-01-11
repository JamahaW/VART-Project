#pragma once

#include <Arduino.h>

#include <utility>
#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/PID.hpp"


namespace hardware {

    /// Сервомотор (Мотор + Обратная связь по энкодеру)
    class ServoMotor {

    public:
        /// Параметры сервопривода
        struct Config {
            /// Период обновления регулятора в секундах
            const float update_period_seconds;

            /// Максимальная абсолютная ошибка позиционирования
            const uint8_t ready_max_abs_error;

            /// Параметры регулятора скорости
            const PID::Config speed_config;

            /// Параметры регулятора позиции
            const PID::Config position_config;
        };
    private:

        const MotorDriverL293 driver;
        Encoder encoder;

        /// Регулятор шим по скорости
        PID speed_pid;

        /// Конфигурация сервопривода
        const Config &config;

        /// Регулятор скорости по положению
        PID position_pid;

        /// Ограничение скорости
        float abs_speed_limit{0};

        /// Целевое положение
        int32_t target_position_ticks{0};

        /// Последняя измеренная скорость
        float calculated_speed{0};

        /// Сервопривод включен
        bool is_enabled{false};

    public:
        explicit ServoMotor(
            const Config &config,
            const MotorDriverL293 &driver,
            const Encoder &encoder
        )
            :
            driver(driver),
            encoder(encoder),
            speed_pid(config.speed_config),
            position_pid(config.position_config),
            config(config) {}

        void disable() {
            encoder.disable();
            driver.setPower(0);
            calculated_speed = 0;
            is_enabled = false;
        }

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

        /// Обновить регулятор
        void update() {
            if (not is_enabled) { return; }

            driver.setPower(int32_t(speed_pid.calc(calcSpeedError(calcSpeedU()), config.update_period_seconds)));
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
            abs_speed_limit = constrain(speed, 0, position_pid.config.max_out);
        }

        /// Получить текущую скорость
        float getCurrentSpeed() const {
            return calculated_speed;
        }

        /// Достиг ли позиции?
        bool isReady() const {
            return abs(calcPositionError()) < config.ready_max_abs_error;
        }

        /// Получить период обновления регулятора в микросекундах
        uint32_t getUpdatePeriodUs() const {
            return uint32_t(getUpdatePeriodSeconds() * 1e6);
        }

        /// Получить период обновления регулятора в секундах
        float getUpdatePeriodSeconds() const {
            return config.update_period_seconds;
        }

        /// Получить управляемый драйвер
        const MotorDriverL293 &getDriver() const { return driver; }

        /// Получить управляемый энкодер
        const Encoder &getEncoder() const { return encoder; }


    private:

        /// Расчитать и ограничить управляющую скорость
        float calcSpeedU() {
            const double speed_u = position_pid.calc(calcPositionError(), config.update_period_seconds);
            return constrain(speed_u, -abs_speed_limit, abs_speed_limit);
        }

        /// Получить ошибку позиционирования
        int32_t calcPositionError() const {
            return target_position_ticks - getCurrentPosition();
        }

        /// Обновить рассчитанную скорость
        float updateCalculatedSpeed() {
            calculated_speed = encoder.calcSpeed(config.update_period_seconds);
            return calculated_speed;
        }

        /// Расчитать ошибку позиционирования
        float calcSpeedError(float target_speed) {
            return target_speed - updateCalculatedSpeed();
        }
    };
}  // namespace hardware