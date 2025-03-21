#pragma once

#include <hardware/ServoMotor.hpp>


namespace vart {

    /// Привод шкива
    class Pulley {
    public:

        /// Конфигурация привода
        struct Settings {

            /// Сколько тиков в миллиметре [tick / mm] (Обязательно > 0)
            const float ticks_in_mm;

            /// Перевести миллиметры в тики позиции сервопривода
            int32_t mmToTicks(double mm) const { return int32_t(mm * ticks_in_mm); }

            /// Перевести тики в миллиметры
            double ticksTomm(int32_t ticks) const { return double(ticks) / ticks_in_mm; }
        };

        /// Сервопривод
        hardware::ServoMotor servo;

    private:

        /// Настройки привода
        const Settings &settings;

    public:

        /// Смещение троса
        int32_t rope_offset_mm{0};

        explicit Pulley(const Settings &settings, hardware::ServoMotor &&servo) :
            settings(settings), servo(servo) {}


        /// Задать целевую длину троса
        void setTargetRopeLength(double target_length_mm) {
            servo.setTargetPosition(calcTicks(target_length_mm));
        }

        /// Установить новое значение для текущей длину троса
        void setCurrentRopeLength(double new_length_mm) {
            servo.setCurrentPosition(calcTicks(new_length_mm));
        }

        /// Получить текущую длину троса
        double getCurrentRopeLength() const {
            return calcMM(servo.getCurrentPosition());
        }

        /// Управление включением
        void setEnabled(bool enabled) { servo.setEnabled(enabled); }

        /// Обновить регулятор
        void update() { servo.update(); }

        /// Получить период обновления в ms
        uint32_t getUpdatePeriodMs() const { return servo.getUpdatePeriodMs(); }

        /// Достигнут трос заданной длины
        bool isReady() const { return servo.isReady(); }

    private:

        /// Рассчитать позицию в тиках из расстояния с учетом смещения
        int32_t calcTicks(double length_mm) const {
            return settings.mmToTicks(length_mm + rope_offset_mm);
        }

        /// Расчитать длину троса в мм из позиции в тиках
        double calcMM(int32_t ticks) const {
            return settings.ticksTomm(ticks) - rope_offset_mm;
        }
    };
}