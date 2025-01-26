#include "vart/Devices.hpp"


struct OLEDDisplay : ui2::abc::Display {
    gfx::OLED oled;

    void init() override { oled.init(); }

    size_t write(uint8_t uint_8) override { return oled.write(uint_8); }

    void setCursor(PixelPosition x, PixelPosition y) override { oled.setCursor(x, y); }

    void clear() override { oled.clear(); }

    void setTextInverted(bool is_inverted) override { oled.setInvertText(is_inverted); }

    uint8_t getWidth() const override { return 128 / 6; }

    uint8_t getRows() const override { return 8; }
};

static OLEDDisplay display;

ui2::Window vart::window(display);
