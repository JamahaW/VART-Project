#include "EncButton.h"

#include "gfx/OLED.hpp"
#include "ui2/abc/Screen.hpp"
#include "ui2/Window.hpp"
#include "ui2/abc/Page.hpp"
#include "ui2/impl/widget/Text.cpp"
#include "ui2/impl/widget/Button.cpp"
#include "ui2/impl/widget/Named.cpp"
#include "vart/util/Pins.hpp"


using vart::Pins;
using ui2::impl::Label;
using ui2::impl::NamedSpinBox;
using uButton = ui2::impl::Button;
using ui2::Event;

static EncButton eb(Pins::UserEncoderA, Pins::UserEncoderB, Pins::UserEncoderButton);


struct OledDisplay : ui2::abc::Display {
    OledDisplay() {}

    gfx::OLED oled;

    size_t write(uint8_t uint_8) override { return oled.write(uint_8); }

    void setCursor(PixelPosition x, PixelPosition y) override { oled.setCursor(x, y); }

    void clear() override { oled.clear(); }

    void setTextInverted(bool is_inverted) override { oled.setInvertText(is_inverted); }
};

static OledDisplay display;


void cb() {
    Serial.println("Click!");
}

void cbs(int v) {
    Serial.println(v);
}

const NamedSpinBox<int>::Settings s = {
    100,
    200,
    5,
    150
};

static ui2::Page p = {<#initializer#>, "Test Page"};

ui2::Window window = {
    .display = display,
    .current_page = &p
};

void setup() {
    display.oled.init();
    window.onEvent(Event::ForceUpdate);
    Serial.begin(115200);
}

void loop() {
    eb.tick();
    if (eb.left()) { window.onEvent(Event::NextWidget); }
    if (eb.right()) { window.onEvent(Event::PreviousWidget); }
    if (eb.rightH()) { window.onEvent(Event::StepUp); }
    if (eb.leftH()) { window.onEvent(Event::StepDown); }
    if (eb.click()) { window.onEvent(Event::Click); }
}
