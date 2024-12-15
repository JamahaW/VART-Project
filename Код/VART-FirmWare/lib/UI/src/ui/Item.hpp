#pragma once

#include <vector>

#include <gfx_legacy/OLED.hpp>


namespace ui {
    /// Элемент, который отображается на странице
    class Item {
    public:
        virtual void render(gfx::OLED &display, bool selected) const = 0;

        virtual void onClick() = 0;

        virtual void onChange(int change) = 0;
    };
}