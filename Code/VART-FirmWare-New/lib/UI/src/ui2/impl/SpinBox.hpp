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
                T default_value;

                T clamp(T v) const { return constrain(v, min, max); }
            };

            const char *name;
            const Settings &settings;
            std::function<void(T)> on_change;
            T value = settings.default_value;

            explicit SpinBox(const char *name, const Settings &settings, std::function<void(T)> on_change) :
                name(name), settings(settings), on_change(on_change) {}

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
                if (on_change == nullptr) { return; }

                if (event == Event::Click) { on_change(value); }

                if (event == Event::StepUp) { value += settings.step; }
                else if (event == Event::StepDown) { value -= settings.step; }

                value = settings.clamp(value);
                on_change(value);
            }
        };
    }
}