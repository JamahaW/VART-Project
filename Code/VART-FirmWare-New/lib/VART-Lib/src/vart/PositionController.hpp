#include "vart/Pulley.hpp"
#include "Macro.hpp"


namespace vart {

    /// Контроллер положения плоттера на рабочей области
    class PositionController {

    public:

        struct Settings {
            const uint16_t max_area_width;
            const uint16_t max_area_height;
        };

    private:

        const Settings &settings;

        Pulley &left_pulley;
        Pulley &right_pulley;

        uint16_t area_width_mm{settings.max_area_width};
        uint16_t area_height_mm{settings.max_area_height};

    public:

        explicit PositionController(const Settings &settings, Pulley &left_pulley, Pulley &right_pulley) :
            settings(settings), left_pulley(left_pulley), right_pulley(right_pulley) {}

        /// Обновление регуляторов шкивов
        void update() {
            left_pulley.update();
            right_pulley.update();
        }

        void setEnabled(bool enabled) {
            left_pulley.setEnabled(enabled);
            right_pulley.setEnabled(enabled);
        }

        void setSpeedLimit(float mm_per_second) {
            left_pulley.setSpeedLimit(mm_per_second);
            right_pulley.setSpeedLimit(mm_per_second);
        }

        /// Оба привода шкива достигли целевых позиций
        bool isReady() const {
            return left_pulley.isReady() and right_pulley.isReady();
        }

        /// Установить целевую позицию
        void setTargetPosition(int16_t target_x_mm, int16_t target_y_mm) {
            double left, right;
            calcDistance(left, right, target_x_mm, target_y_mm);
            left_pulley.setTargetRopeLength(left);
            right_pulley.setTargetRopeLength(right);
        }

        /// Установить текущее положение как home
        void setCurrentPositionAsHome() {
            double left, right;
            calcDistance(left, right, 0, 0);
            left_pulley.setCurrentRopeLength(left);
            right_pulley.setCurrentRopeLength(right);
        }

    private:

        void calcDistance(double &return_left_rope_length_mm, double &return_right_rope_length_mm, int16_t x, int16_t y) const {
            int32_t i = area_height_mm / 2 - y;
//            logFloat(i);

            int32_t j = area_width_mm / 2;
//            logFloat(j);

            return_left_rope_length_mm = std::hypot(x + j, i);
//            logFloat(return_left_rope_length_mm);

            return_right_rope_length_mm = std::hypot(x - j, i);
//            logFloat(return_right_rope_length_mm);
        }
    };
}