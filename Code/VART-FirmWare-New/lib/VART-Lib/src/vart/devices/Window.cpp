#include "vart/Devices.hpp"
#include "vart/util/Pins.hpp"
#include "gfx/OLED.hpp"


using ui::Window;
using ui::Event;

static gfx::OLED display;

static EncButton encoder(vart::Pins::UserEncoderA, vart::Pins::UserEncoderB, vart::Pins::UserEncoderButton);

Window vart::window = Window(display, []() -> Event {
    encoder.tick();

    if (encoder.left()) {
        return Event::NextItem;
    }
    if (encoder.right()) {
        return Event::PastItem;
    }
    if (encoder.click()) {
        return Event::Click;
    }
    if (encoder.leftH()) {
        return Event::ChangeUp;
    }
    if (encoder.rightH()) {
        return Event::ChangeDown;
    }

    return Event::Idle;
});
