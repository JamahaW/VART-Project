#pragma once

#include <hardware/ServoMG90S.hpp>
#include <utility>
#include "vart/util/Range.hpp"


namespace vart {
    class MarkerPrintTool {
    public:

        using Angle = hardware::ServoMG90S::Angle;

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

        /// Параметры инструмента печати
        struct Settings {
            /// Допустимый диапазон углов
            Range<Angle> angle_range;
            /// Углы для каждого маркера
            std::array<Angle, Marker::TotalCount> positions;
        };

    private:

        /// Настройки
        Settings &settings;

    public:

        /// Сервопривод для выбора маркеров
        hardware::ServoMG90S servo;

        explicit MarkerPrintTool(Settings &settings, hardware::ServoMG90S servo) :
            servo(std::move(servo)), settings(settings) {}

        /// Обновить угол для маркера
        void updateToolAngle(Marker marker, Angle angle) {
            settings.positions.at(marker) = settings.angle_range.clamp(angle);
        }

        /// Получить текущий угол сервопривода
        Angle getToolAngle(Marker marker) { return settings.positions.at(marker); }

        /// Выбрать маркер
        void setActiveTool(Marker marker) { servo.setAngle(settings.positions.at(marker)); }
    };
}