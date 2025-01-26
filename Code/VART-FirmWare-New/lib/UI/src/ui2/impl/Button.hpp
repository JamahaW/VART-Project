#pragma once

#include <utility>

#include "../abc/Widget.hpp"
#include "functional"


namespace ui2 {
    namespace impl {
        struct Button : abc::Widget {
            const char *name;

            std::function<void()> on_click;

            explicit Button(const char *name, std::function<void()> on_click = nullptr) :
                name(name), on_click(std::move(on_click)) {}

            void render(abc::Display &display, bool is_selected) const override {
                display.setTextInverted(is_selected);
                display.write('[');
                display.print(name);
                display.write(']');
                display.setTextInverted(false);
            }

            void onEvent(Event event) override {
                if (on_click == nullptr) { return; }
                if (event == Event::Click) { on_click(); }
            }

        };
    }
}