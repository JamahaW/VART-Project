#include <utility>

#include "Builtin.hpp"


using ui2::impl::widget::Button;
using ui2::impl::widget::Text;
using ui2::abc::Screen;
using ui2::Event;

Button::Button(Text text, std::function<void()> on_click) :
    text(std::move(text)), on_click(std::move(on_click)) {}

void Button::onEvent(Event event) {
    if (on_click == nullptr) { return; }
    if (event == Event::Click) { on_click(); }
}

void Button::render(abc::Screen &screen) const {
    screen.write('[');
    text.render(screen);
    screen.write(']');
}
