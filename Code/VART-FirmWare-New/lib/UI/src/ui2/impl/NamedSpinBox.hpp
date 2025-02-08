#pragma once

#include "functional"
#include <Arduino.h>

#include "ui2/abc/Widget.hpp"
#include "ui2/impl/Text.hpp"
#include "ui2/impl/SpinBox.hpp"


namespace ui2 {
    namespace impl {
        template<typename T> struct NamedSpinBox : abc::Widget {
            using Input = SpinBox<T>;

            const Text text;
            Input spin_box;

            explicit NamedSpinBox(const char *name, Input spin_box) :
                text(name), spin_box(std::move(spin_box)) {}

            void render(abc::Display &display) const override {
                text.render(display);
                display.write(':');
                display.write(' ');
                spin_box.render(display);
            }

            void onEvent(Event event) override { spin_box.onEvent(event); }
        };
    }
}