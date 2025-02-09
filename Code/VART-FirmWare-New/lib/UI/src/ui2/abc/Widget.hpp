#pragma once

#include "Screen.hpp"
#include "ui2/Event.hpp"


namespace ui2 {
    namespace abc {
        /// Виджет
        struct Widget {
            /// Отреагировать на событие
            virtual void onEvent(Event event) {};

            /// Отрисовать сам виджет
            virtual void render(Screen &display) const = 0;
        };
    }
}