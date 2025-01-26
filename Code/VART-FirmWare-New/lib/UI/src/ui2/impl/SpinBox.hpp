#pragma once

#include "ui2/abc/Widget.hpp"
#include "functional"
#include <Arduino.h>


namespace ui2 {
    namespace impl {
        template<typename T> struct SpinBox : abc::Widget {

            struct Settings {
                T min;
                T max;
                T step;

                T clamp(T v) const { return constrain(v, min, max); }
            };

            const char *name;
            const Settings &settings;
            std::function<void(T)> on_change;
            T value;

            explicit SpinBox(
                const char *name,
                const Settings &settings,
                T default_value,
                std::function<void(T)> on_change = nullptr
            ) :
                name(name), settings(settings), value(default_value), on_change(on_change) {}

            void render(abc::Display &display, bool is_selected) const override {
                display.setTextInverted(is_selected);
                display.print(name);
                display.write(':');
                display.write(' ');
                display.write('<');
                display.print(value);
                display.write('>');
                display.setTextInverted(false);
            }

            void onEvent(Event event) override {
                if (event == Event::StepUp) { value += settings.step; }
                else if (event == Event::StepDown) { value -= settings.step; }
                value = settings.clamp(value);
                if (on_change != nullptr) { on_change(value); }
            }
        };
    }
}