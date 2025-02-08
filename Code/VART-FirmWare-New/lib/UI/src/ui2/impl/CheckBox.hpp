#pragma once

#include <utility>

#include "ui2/abc/Widget.hpp"
#include "functional"


namespace ui2 {
    namespace impl {
        struct CheckBox : abc::Widget {
            static constexpr const char *ON = "[ ON ]";
            static constexpr const char *OFF = "[ OFF ]";

            const Text text;
            std::function<void(bool)> on_change;
            bool state;

            explicit CheckBox(const char *name, std::function<void(bool)> on_change = nullptr, bool state = false) :
                text(name), on_change(std::move(on_change)), state(state) {}

            void onEvent(Event) override {
                state ^= 1;
                if (on_change != nullptr) { on_change(state); }
            }

            void render(abc::Display &display) const override {
                text.render(display);
                display.write(':');
                display.write(' ');
                display.print(state ? ON : OFF);
            }
        };
    }
}