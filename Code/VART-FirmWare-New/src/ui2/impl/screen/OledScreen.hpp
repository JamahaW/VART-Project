#pragma once

#include "ui2/abc/Screen.hpp"
#include "gfx/OLED.hpp"


namespace ui2 {
    namespace impl {
        namespace screen {
            struct OledScreen : ui2::abc::Screen {
                gfx::OLED oled;

                static OledScreen &getInstance() {
                    static OledScreen instance;
                    return instance;
                }

                size_t write(uint8_t uint_8) override { return oled.write(uint_8); }

                void setCursor(PixelPosition x, PixelPosition y) override { oled.setCursor(x, y); }

                void clear() override { oled.clear(); }

                void setTextInverted(bool is_inverted) override { oled.setInvertText(is_inverted); }

                uint8_t getWidth() const override { return 128 / 6; }

                uint8_t getRows() const override { return 8; }

                OledScreen(const OledScreen &) = delete;

                OledScreen &operator=(const OledScreen &) = delete;

            private:
                OledScreen() = default;
            };
        }
    }
}