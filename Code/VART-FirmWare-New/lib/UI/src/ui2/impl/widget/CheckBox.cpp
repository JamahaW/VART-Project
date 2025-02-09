#include "Builtin.hpp"


using ui2::impl::widget::CheckBox;

CheckBox::CheckBox(Text text, std::function<void(bool)> on_change, bool state) :
    text(std::move(text)), on_change(std::move(on_change)), state(state) { onClick(); }

void CheckBox::onEvent(Event) {
    state ^= 1;
    onClick();
}

void CheckBox::render(abc::Screen &screen) const {
    text.render(screen);
    screen.write(':');
    screen.write(' ');
    screen.print(state ? "[ ON ]" : "[ OFF ]");
}

void CheckBox::onClick() const {
    if (on_change == nullptr) { return; }
    on_change(state);
}
