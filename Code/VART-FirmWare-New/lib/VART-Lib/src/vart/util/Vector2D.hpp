#pragma once

#include <Arduino.h>


namespace vart {

    /// Двухмерный вектор
    struct Vector2D {
        double x;
        double y;

        Vector2D clamp(Vector2D _min, Vector2D _max) const {
            return {
                .x = constrain(x, _min.x, _max.x),
                .y = constrain(y, _min.y, _max.y),
            };
        }

        double length() const {
            return std::hypot(x, y);
        }

        double distance(const Vector2D &other) const {
            return std::hypot(x - other.x, y - other.y);
        }

        Vector2D floor() const {
            return {::floor(x), ::floor(y)};
        }

        Vector2D normalize() const {
            const auto len = length();
            if (abs(len) < 0.000001) { return {0, 0}; }
            return {x / len, y / len};
        }

        Vector2D add(const Vector2D &other) const {
            return {x + other.x, y + other.y};
        }

        Vector2D sub(const Vector2D &other) const {
            return {x - other.x, y - other.y};
        }

        Vector2D scale(double scale) const {
            return {x * scale, y * scale};
        }

        static Vector2D interpolate(const Vector2D &begin, const Vector2D &end, double a) {
            return begin.scale(1 - a).add(end.scale(a));
        }
    };
}