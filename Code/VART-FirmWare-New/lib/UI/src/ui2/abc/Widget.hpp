#pragma once

#include "Display.hpp"
#include "../Event.hpp"


namespace ui2 {
    namespace abc {
        /// Виджет
        struct Widget {

            /// Отобразить виджет на странице
            virtual void render(Display &display, bool is_selected) const = 0;

            /// Отреагировать на событие
            virtual void onEvent(Event event) {};
        };
    }
}