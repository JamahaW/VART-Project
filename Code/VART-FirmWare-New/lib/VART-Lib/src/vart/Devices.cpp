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


using hardware::MotorDriverL293;
using hardware::ServoMotor;
using vart::Pins;
using vart::Pulley;

static auto left_encoder = hardware::Encoder(Pins::left_encoder_a, Pins::left_encoder_b);

static auto right_encoder = hardware::Encoder(Pins::right_encoder_a, Pins::right_encoder_b);

static auto left_driver = MotorDriverL293(Pins::left_driver_a, Pins::left_driver_b);

static auto right_driver = MotorDriverL293(Pins::right_driver_a, Pins::right_driver_b);

static ServoMotor::Settings servo_settings = {
    .update_period_seconds = 1 * 1e-3,
    .ready_max_abs_error = 10,
    .position = {
        .pid = {
            .kp = 10,
            .ki = 3,
            .kd = 0.2,
            .abs_max_i = 204
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 10,
        },
        .abs_max_out = 255
    },
};

static auto left_servo = ServoMotor(servo_settings, left_driver, left_encoder);

static auto right_servo = ServoMotor(servo_settings, right_driver, right_encoder);

static const Pulley::Settings pulley_settings = {
    .ticks_in_mm = 5000.0 / 280.0,
};

static auto left_pulley = Pulley(pulley_settings, left_servo);

static auto right_pulley = Pulley(pulley_settings, right_servo);

using vart::PositionController;
using vart::Area;

Area::Settings area_settings = {
    .max_area_size = {4000, 4000},
    .min_area_size = {500, 500},
    .default_area_size = {1200, 1200},
};

static auto area = Area(area_settings);

PositionController vart::position_controller = PositionController(area, left_pulley, right_pulley);