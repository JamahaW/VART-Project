#include "Builtin.hpp"


using ui2::impl::widget::Text;

Text::Text(const char *text) :
    text(text == nullptr ? "#null" : text) {}

void Text::render(abc::Screen &screen) const {
    screen.print(text);
}
