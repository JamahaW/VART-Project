#include "Builtin.hpp"


static constexpr char BAR = '|';

static constexpr char EMPTY = '.';


ui2::impl::widget::ProgressBar::ProgressBar(int &value) :
    value(value) {}

void ui2::impl::widget::ProgressBar::render(ui2::abc::Screen &screen) const {
    const uint8_t bars_total = screen.getWidth() - 3;
    const uint8_t bars = value * bars_total / 100;

    screen.write('[');

    for (uint8_t i = 0; i < bars_total; i++) {
        screen.write(i < bars ? BAR : EMPTY);
    }

    screen.write(']');
    screen.write(' ');
    screen.print(value);
}
