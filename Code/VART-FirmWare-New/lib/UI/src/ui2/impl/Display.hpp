#pragma once

#include "ui2/abc/Widget.hpp"


namespace ui2 {
    namespace impl {
        template<typename T> struct Display : abc::Widget {
            const Text text;
            T &value;

            explicit Display(const char *name, T &value) :
                text(name), value(value) {}

            void render(abc::Display &display) const override {
                text.render(display);
                display.write(':');
                display.write(' ');
                display.print(value);
            }
        };
    }
}