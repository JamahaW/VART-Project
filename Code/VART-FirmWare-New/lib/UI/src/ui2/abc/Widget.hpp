#pragma once

#include "ui2/abc/Display.hpp"
#include "ui2/Event.hpp"


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