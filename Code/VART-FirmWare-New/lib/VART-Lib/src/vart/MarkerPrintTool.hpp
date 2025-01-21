#pragma once

#include <hardware/ServoMG90S.hpp>
#include <utility>
#include "vart/util/Range.hpp"


namespace vart {
    class MarkerPrintTool {
    public:

        /// Выбор инструмента
        enum Marker : unsigned char {
            /// Никакой (Маркер не выбран)
            None = 0x00,

            /// Левый
            Left = 0x01,

            /// Правый
            Right = 0x02,

            /// Константа для определения общего кол-ва инструментов
            TotalCount
        };

        /// Примитив для передачи угла
        using Angle = uint8_t;

        /// Параметры инструмента печати
        struct Settings {

            /// Допустимый диапазон углов
            Range<Angle> angle_range;

            /// Углы для каждого маркера
            std::array<Angle, Marker::TotalCount> positions;

            /// Длительность смены инструмента в миллисекундах
            uint32_t tool_change_duration_ms;
        };

    private:

        /// Сервопривод для выбора маркеров
        const hardware::ServoMG90S servo;

        /// Настройки
        Settings &settings;

        /// Инструмент готов
        bool is_ready{false};

    public:

        explicit MarkerPrintTool(Settings &settings, hardware::ServoMG90S servo) :
            servo(std::move(servo)), settings(settings) {}

        /// Обновить угол для маркера
        void updateToolAngle(Marker marker, Angle angle) {
            settings.positions.at(marker) = settings.angle_range.clamp(angle);
        }

        /// Получить текущий угол сервопривода
        Angle getToolAngle(Marker marker) { return settings.positions.at(marker); }

        /// Выбрать маркер
        void setActiveTool(Marker marker) {
            is_ready = false;
            servo.setAngle(settings.positions.at(marker));
            delay(settings.tool_change_duration_ms);
            is_ready = true;
        }

        /// Инструмент готов к смене
        bool isReady() const { return is_ready; }

        void setChangeDuration(uint32_t ms) { settings.tool_change_duration_ms = ms; }

        uint32_t getChangeDuration() const { return settings.tool_change_duration_ms; }

        void setEnabled(bool enabled) {
            if (enabled) { servo.enable(); }
            else { servo.disable(); }
        }
    };
}