#pragma once

#include <ui2/abc/Display.hpp>
#include <gfx/OLED.hpp>


namespace ui2 {
    namespace impl {
        struct OledDisplay : ui2::abc::Display {
            gfx::OLED oled;

            static OledDisplay &getInstance() {
                static OledDisplay instance;
                return instance;
            }

            size_t write(uint8_t uint_8) override { return oled.write(uint_8); }

            void setCursor(PixelPosition x, PixelPosition y) override { oled.setCursor(x, y); }

            void clear() override { oled.clear(); }

            void setTextInverted(bool is_inverted) override { oled.setInvertText(is_inverted); }

            uint8_t getWidth() const override { return 128 / 6; }

            uint8_t getRows() const override { return 8; }

            OledDisplay(const OledDisplay &) = delete;

            OledDisplay &operator=(const OledDisplay &) = delete;

        private:
            OledDisplay() = default;
        };
    }
}