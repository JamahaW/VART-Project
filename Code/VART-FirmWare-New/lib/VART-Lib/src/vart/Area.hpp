#pragma once

#include <cstdint>


namespace vart {
    struct Vector2D {
        int16_t x;
        int16_t y;

        Vector2D clamp(Vector2D _min, Vector2D _max) {
            return {
                .x = constrain(x, _min.x, _max.x),
                .y = constrain(y, _min.y, _max.y),
            };
        }
    };

    /// Рабочая область
    class Area {

    public:

        /// Настройки рабочей области
        struct Settings {
            /// Максимальный размер рабочей области
            const Vector2D max_area_size;

            /// Минимальный размер рабочей области
            const Vector2D min_area_size;

            /// Рабочая область по умолчанию
            const Vector2D default_area_size;
        };

    private:

        const Settings &settings;

        /// Текущий размер рабочей области
        Vector2D current_size_mm;

        /// Половины размера для расчётов
        int w{0}, h{0};

    public:

        explicit Area(const Settings &settings) :
            settings(settings),
            current_size_mm(settings.default_area_size) {
            updateWH(current_size_mm);
        }

        /// Получить размер рабочей области
        Vector2D getSize() const { return current_size_mm; }

        /// Установить новый размер рабочей области
        void setSize(Vector2D new_size) {
            current_size_mm = new_size.clamp(settings.min_area_size, settings.max_area_size);
            updateWH(new_size);
        }

        void updateWH(Vector2D size) {
            w = size.x / 2;
            h = size.y / 2;
        }

        /// Получить положения тросов по текущей позиции
        void calcBackward(Vector2D position, double &l, double &r) const {
            auto i = h - position.y;
            l = std::hypot(position.x + w, i);
            r = std::hypot(position.x - w, i);
        }

        Vector2D calcForward(double l, double r) const {
            auto x = (l - r) * (l + r) / (w * 4);
            return {
                .x = int16_t(x),
                .y = int16_t(h - std::sqrt((l - x - w) * (l + x + w)))
            };
        }
    };
}