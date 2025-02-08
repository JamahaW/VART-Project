#pragma once

#include "ui2/abc/Widget.hpp"
#include "Arduino.h"


namespace ui2 {
    namespace impl {
        template<typename T> struct ProgressBar : abc::Widget {

            static constexpr char BAR = '|';
            static constexpr char EMPTY = '.';

            T &value;

            explicit ProgressBar(T &value) :
                value(value) {}

        protected:
            void render(abc::Display &display) const override {
                const uint8_t bars_total = display.getWidth() - 3;
                const uint8_t bars = value * bars_total / 100;

                display.write('[');

                for (uint8_t i = 0; i < bars_total; i++) {
                    display.write(i < bars ? BAR : EMPTY);
                }

                display.write(']');
                display.write(' ');
                display.print(value);
            }


        };
    }
}