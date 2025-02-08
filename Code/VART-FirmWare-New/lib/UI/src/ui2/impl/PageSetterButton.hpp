#pragma once

#include "Button.hpp"
#include "../Page.hpp"
#include "../Window.hpp"


namespace ui2 {
    namespace impl {
        struct PageSetterButton : Button {

            PageSetterButton(Page &page, Window &window) :
                Button(page.title.text, [&page, &window]() { window.setPage(&page); }) {}

        protected:

            void render(abc::Display &display) const override {
                display.write('-');
                display.write('>');
                display.write(' ');
                text.render(display);
            }
        };
    }
}