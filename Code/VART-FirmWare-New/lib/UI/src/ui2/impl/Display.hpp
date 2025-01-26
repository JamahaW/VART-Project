#pragma once

#include "ui2/abc/Widget.hpp"


namespace ui2 {
    namespace impl {
        template<typename T> struct Display : abc::Widget {
            const char *name;
            T &value;

            explicit Display(const char *name, T &value) :
                name(name), value(value) {}

            void render(abc::Display &display, bool is_selected) const override {
                display.setTextInverted(is_selected);
                display.print(name);
                display.write(':');
                display.write(' ');
                display.print(value);
                display.setTextInverted(false);
            }
        };
    }
}