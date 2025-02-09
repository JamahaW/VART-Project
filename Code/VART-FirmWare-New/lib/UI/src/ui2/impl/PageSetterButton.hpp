#pragma once

#include "Button.hpp"
#include "ui2/Window.hpp"


namespace ui2 {
    namespace impl {
        struct PageSetterButton : Button {
            explicit PageSetterButton(Page &page, const std::function<void(Page &)> &on_entry) :
                Button(page.title.text, [&page, on_entry]() {
                    Window::getInstance().setPage(&page);
                    if (on_entry != nullptr) { on_entry(page); }
                }) {}

            void render(abc::Display &display) const override {
                display.write('-');
                display.write('>');
                display.write(' ');
                text.render(display);
            }
        };
    }
}