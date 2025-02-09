#pragma once

#include "ui2/abc/Widget.hpp"


namespace ui2 {
    namespace impl {
        namespace widget {
            /// Тестовое поле
            struct Text : abc::Widget {
                static constexpr const char *null_text = "#null";

                const char *text;

                explicit Text(const char *text) :
                    text(text) {}

                void render(abc::Display &display) const override {
                    display.print(text == nullptr ? null_text : text);
                }
            };
        }
    }
}