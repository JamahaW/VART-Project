#pragma once

#include "Button.hpp"
#include "../Page.hpp"
#include "../Window.hpp"


namespace ui2 {
    namespace impl {
        struct PageSetterButton : Button {

            PageSetterButton(Page &page, __attribute__((unused)) Window &window) :
                Button(page.title, [&page, &window]() { window.setPage(&page); }) {}

            void render(abc::Display &display, bool is_selected) const override {
                display.setTextInverted(is_selected);
                display.write('-');
                display.write('>');
                display.write(' ');
                display.print(name);
                display.setTextInverted(false);
            }
        };
    }
}