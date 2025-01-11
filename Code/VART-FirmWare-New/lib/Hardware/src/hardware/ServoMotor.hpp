#pragma once

#include <Arduino.h>

#include <utility>
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"
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

            /// Максимальная скорость
            const float max_speed;
        };

        const MotorDriverL293 driver;
        const Encoder encoder;

        /// Регулятор шим по скорости
        PID speed_pid;
    private:

        /// Конфигурация сервопривода
        const Config &config;

        /// Регулятор скорости по положению
        PID position_pid;

        /// Ограничение скорости
        float abs_speed_limit{0};

        /// Целевое положение
        int32_t target_position_ticks{0};

    public:

        explicit ServoMotor(
            const Config &config,
            const PID::Config &speed_config,
            const PID::Config &position_config,
            const MotorDriverL293 &driver,
            const Encoder &encoder
        ) :
            driver(driver),
            encoder(encoder),
            speed_pid(speed_config),
            position_pid(position_config),
            config(config) {}

        /// Установить ограничение скорости
        void setSpeedLimit(float speed) {
            abs_speed_limit = constrain(speed, 0, config.max_speed);
        }

        /// Установить новую целевую позицию
        void setTargetPosition(int32_t new_target_position_ticks) {
            target_position_ticks = new_target_position_ticks;
        }

        /// Обновить регулятор
        void update() {
            const float dt = config.update_period_seconds;

            double speed_u = position_pid.calc(calcPositionError(), dt);

            speed_u = constrain(speed_u, -config.max_speed, config.max_speed);
            double speed_error = speed_u - encoder.calcSpeed(dt);

            const auto power_u = int32_t(speed_pid.calc(speed_error, dt));
            driver.setPower(power_u);
        }

        /// Получить ошибку позиционирования
        int32_t calcPositionError() const { return target_position_ticks - encoder.getPosition(); }

        /// Достиг ли позиции?
        bool isReady() const {
            return abs(calcPositionError()) < config.ready_max_abs_error;
        }

    };
}