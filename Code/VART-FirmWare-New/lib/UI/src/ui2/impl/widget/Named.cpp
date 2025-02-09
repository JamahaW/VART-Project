#include "Builtin.hpp"


using ui2::impl::widget::Named;

Named::Named(Text text, abc::Widget &inner) :
    text(std::move(text)), inner(inner) {}

void Named::render(abc::Screen &screen) const {
    text.render(screen);
    screen.write(':');
    screen.write(' ');
    inner.render(screen);
}

void Named::onEvent(Event event) { inner.onEvent(event); }
