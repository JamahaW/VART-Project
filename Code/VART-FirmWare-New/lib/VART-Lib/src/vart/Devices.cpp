#include "vart/Devices.hpp"
#include "vart/Pins.hpp"
#include "gfx/OLED.hpp"

static gfx::OLED display;

static EncButton encoder(vart::Pins::user_encoder_a, vart::Pins::user_encoder_b, vart::Pins::user_encoder_button);

ui::Window vart::window = ui::Window(display, []() -> ui::Event {
    using ui::Event;

    encoder.tick();

    if (encoder.left()) {
        return Event::next_item;
    }
    if (encoder.right()) {
        return Event::past_item;
    }
    if (encoder.click()) {
        return Event::click;
    }
    if (encoder.leftH()) {
        return Event::change_up;
    }
    if (encoder.rightH()) {
        return Event::change_down;
    }

    return Event::idle;
});


