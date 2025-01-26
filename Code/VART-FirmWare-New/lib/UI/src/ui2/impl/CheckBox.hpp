#pragma once

#include <utility>

#include "../abc/Widget.hpp"
#include "functional"


namespace ui2 {
    namespace impl {
        struct CheckBox : abc::Widget {
            static constexpr const char *ON = "[ ON ]";
            static constexpr const char *OFF = "[ OFF ]";

            const char *name;
            std::function<void(bool)> on_change;
            bool state;

            explicit CheckBox(const char *name, std::function<void(bool)> on_change = nullptr, bool state = false) :
                name(name), on_change(std::move(on_change)), state(state) {}

            void render(abc::Display &display, bool is_selected) const override {
                display.setTextInverted(is_selected);
                display.print(name);
                display.write(':');
                display.write(' ');
                display.print(state ? ON : OFF);
                display.setTextInverted(false);
            }

            void onEvent(Event) override {
                state ^= 1;
                if (on_change != nullptr) { on_change(state); }
            }
        };
    }
}