#pragma once

#include <functional>

#include <gfx/OLED.hpp>
#include <ui/Item.hpp>


namespace ui {

    enum Style : unsigned char {
        clean = 0,
        square_framed = 1,
        triangle_framed = 2,
        arrow_prefix = 3,
    };

    enum class ValueType : unsigned char {
        Chars = 0,
        Integer = 1,
        Float = 2,
    };

    class Widget : public Item {
    private:
        Style style{Style::clean};

        const ValueType type;
        const std::function<void(Widget *)> on_click;
        const std::function<void(Widget *, int)> on_change;

    public:
        void *value;

        explicit Widget(
            ValueType type,
            void *value,
            std::function<void(Widget *)> &&on_click = nullptr,
            std::function<void(Widget *, int)> &&on_change = nullptr
        );

        Widget *setStyle(Style new_style) {
            style = new_style;
            return this;
        }

        void render(gfx::OLED &display, bool selected) const override;

        void onClick() override;

        void onChange(int change) override;

    private:
        void draw(gfx::OLED &display) const;
    };
}


