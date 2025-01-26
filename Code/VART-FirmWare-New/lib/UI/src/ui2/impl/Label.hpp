#pragma once

#include "../abc/Widget.hpp"


namespace ui2 {
    namespace impl {
        /// Тестовое поле
        struct Label : abc::Widget {
            /// Сам текст
            const char *text;

            explicit Label(const char *text) :
                text(text) {}

            void render(abc::Display &display, bool is_selected) const override {
                display.setTextInverted(is_selected);
                display.print(text);
                display.setTextInverted(false);
            }
        };
    }
}