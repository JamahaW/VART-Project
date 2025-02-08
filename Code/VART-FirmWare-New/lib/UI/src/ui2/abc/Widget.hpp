#pragma once

#include "Display.hpp"
#include "../Event.hpp"


namespace ui2 {
    namespace abc {
        /// Виджет
        struct Widget {

            /// Отреагировать на событие
            virtual void onEvent(Event event) {};

            /// Отрисовать сам виджет
            virtual void render(abc::Display &display) const = 0;
        };
    }
}