#include "vart/Area.hpp"
#include "vart/Pulley.hpp"


namespace vart {

    /// Контроллер положения плоттера на рабочей области
    class PositionController {

    private:

        Area &area;
        Pulley &left_pulley;
        Pulley &right_pulley;

    public:

        explicit PositionController(Area &area, Pulley &left_pulley, Pulley &right_pulley) :
            area(area), left_pulley(left_pulley), right_pulley(right_pulley) {}

        /// Обновление регуляторов шкивов
        void update() {
            left_pulley.update();
            right_pulley.update();
        }

        void setAreaSize(Vector2D size) { area.setSize(size); }

        Vector2D getAreaSize() const { return area.getSize(); }

        void setEnabled(bool enabled) {
            left_pulley.setEnabled(enabled);
            right_pulley.setEnabled(enabled);
        }

        /// Оба привода шкива достигли целевых позиций
        bool isReady() const {
            return left_pulley.isReady() and right_pulley.isReady();
        }

        /// Установить целевую позицию
        void setTargetPosition(Vector2D target) {
            double l, r;
            area.calcBackward(target, l, r);
            left_pulley.setTargetRopeLength(l);
            right_pulley.setTargetRopeLength(r);
        }

        /// Установить текущее положение как home
        void setCurrentPositionAsHome() {
            double l, r;
            area.calcBackward({0, 0}, l, r);
            left_pulley.setCurrentRopeLength(l);
            right_pulley.setCurrentRopeLength(r);
        }

        uint32_t getUpdatePeriodMs() const {
            return max(left_pulley.getUpdatePeriodMs(), right_pulley.getUpdatePeriodMs());
        }

    };
}