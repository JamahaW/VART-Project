#include "ui2/Window.hpp"


using ui2::Window;
using ui2::impl::widget::PageSetterButton;

PageSetterButton::PageSetterButton(abc::Page &page, const std::function<void(abc::Page & )> &on_entry) :
    Button(page.title, [&page, on_entry]() {
        Window::getInstance().setPage(page);
        if (on_entry != nullptr) { on_entry(page); }
    }) {}

void PageSetterButton::render(abc::Screen &screen) const {
    screen.write('-');
    screen.write('>');
    screen.write(' ');
    text.render(screen);
}
