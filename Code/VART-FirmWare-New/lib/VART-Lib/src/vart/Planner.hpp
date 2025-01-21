#pragma once

#include "vart/PositionController.hpp"
#include "vart/util/Range.hpp"


namespace vart {

    /// Планировщик перемещение
    class Planner {
    public:

        /// Режим работы
        enum Mode : unsigned char {
            /// По текущей позиции
            Position = 0x00,

            /// Со скоростью
            Speed = 0x01,

            /// С заданным ускорением
            Accel = 0x02,
        };

        /// Настройки планировщика
        struct Settings {

            /// Режим планировщика по умолчанию
            Mode default_mode;

            /// Диапазон скоростей
            Range<double> speed_range;

            /// Скорость по умолчанию
            double default_speed;

            /// Ускорение по умолчанию
            Range<double> accel_range;

            /// Диапазон ускорений
            double default_accel;
        };

    private:

        const Settings &settings;

        /// Готов к следующему шагу
        bool is_ready{false};

        /// Текущий режим работы
        Mode mode{settings.default_mode};

        /// Заданная скорость
        double speed_set{settings.default_speed};

        /// Заданное ускорение
        double accel_set{settings.default_accel};

        /// Контроллер позиции
        PositionController &controller;

    public:

        explicit Planner(const Settings &settings, PositionController &controller) :
            controller(controller), settings(settings) {}

        /// Получить контроллер позиции
        PositionController &getController() const { return controller; }

        /// Установить режим работы
        void setMode(Mode new_mode) { mode = new_mode; }

        /// Задать ограничение скорости
        void setSpeed(double speed) { speed_set = settings.speed_range.clamp(speed); }

        /// Получить текущую установку скорости
        double getSpeed() const { return speed_set; }

        /// Задать ограничение ускорения
        void setAccel(double accel) { accel_set = settings.accel_range.clamp(accel); }

        /// Получить текущую установку ускорения
        double getAccel() const { return accel_set; }

        /// Планировщик завершил перемещение и готов к следующему
        bool isReady() const { return is_ready; }

        /// Перейти в позицию
        void moveTo(Vector2D position) {
            is_ready = false;

            switch (mode) {
                case Position:
                    goPosition(position);
                    break;
                case Speed:
                    goConstSpeed(position);
                    break;
                case Accel:
                    goConstAccel(position);
                    break;
            }

            is_ready = true;
        }

    private:

        void goPosition(Vector2D target) {
            controller.setTargetPosition(target);
            while (not controller.isReady()) { delay(1); }
        }

        void goConstSpeed(Vector2D target) {
            Vector2D begin = controller.getCurrentPosition();
            auto path_len = target.distance(begin);

            if (path_len < 1) { return; }

            auto steps_total = 1e3 * path_len / speed_set;

            for (int i = 0; i < steps_total; i++) {
                controller.setTargetPosition(Vector2D::interpolate(begin, target, i / steps_total));
                delay(1);
            }

            controller.setTargetPosition(target);
        }

        void goConstAccel(Vector2D target) {
            const double delta_time = 0.001;

            Vector2D position = controller.getCurrentPosition();
            Vector2D velocity = {0, 0};
            controller.setTargetPosition(position);

            while (position.distance(target) > 0.001) {
                Vector2D to_goal = target.sub(position);
                double distance_to_goal = to_goal.length();

                double braking_distance = (velocity.length() * velocity.length()) / (2.0 * accel_set);

                Vector2D desired_velocity{};

                if (distance_to_goal > braking_distance) {
                    desired_velocity = to_goal.normalize().scale(speed_set);
                } else {
                    desired_velocity = to_goal.normalize().scale(std::max(speed_set * (distance_to_goal / braking_distance), 0.5)); // Минимальная скорость
                }

                Vector2D needed_acceleration = (desired_velocity.sub(velocity)).scale(1.0 / delta_time);

                if (needed_acceleration.length() > accel_set) {
                    needed_acceleration = needed_acceleration.normalize().scale(accel_set);
                }

                Vector2D new_velocity = velocity.add(needed_acceleration.scale(delta_time));
                Vector2D new_position = position.add(new_velocity.scale(delta_time));

                if (new_position.distance(target) > position.distance(target)) {
                    new_velocity = {0, 0};
                }

                velocity = new_velocity;
                position = new_position;
                controller.setTargetPosition(position);

                delay(1);
            }

            controller.setTargetPosition(target);
        }
    };
}


