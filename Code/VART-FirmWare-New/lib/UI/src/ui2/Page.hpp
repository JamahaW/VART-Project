#pragma once

#include "ui2/Display.hpp"


namespace ui2 {

    /// Отображаемая страница
    struct Page {
        virtual void render(Display &display) const = 0;
    };
}