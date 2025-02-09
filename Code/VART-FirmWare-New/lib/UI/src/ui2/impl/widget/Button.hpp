#pragma once

#include <functional>

#include "ui2/abc/Widget.hpp"
#include "Text.hpp"


namespace ui2 {
    namespace impl {
        namespace widget {
            struct Button : abc::Widget {
                const Text text;

                std::function<void()> on_click;

                explicit Button(const char *name, std::function<void()> on_click = nullptr) :
                    text(name), on_click(std::move(on_click)) {}

                void onEvent(Event event) override {
                    if (on_click == nullptr) { return; }
                    if (event == Event::Click) { on_click(); }
                }

                void render(abc::Display &display) const override {
                    display.write('[');
                    text.render(display);
                    display.write(']');
                }
            };
        }
    }
}