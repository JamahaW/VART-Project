#include "vart/Devices.hpp"
#include "vart/Pins.hpp"
#include "gfx/OLED.hpp"


using hardware::MotorDriverL293;
using hardware::ServoMotor;
using vart::Pins;
using vart::Pulley;

static auto left_encoder = hardware::Encoder(Pins::left_encoder_a, Pins::left_encoder_b);

static auto right_encoder = hardware::Encoder(Pins::right_encoder_a, Pins::right_encoder_b);

static auto left_driver = MotorDriverL293(Pins::left_driver_a, Pins::left_driver_b);

static auto right_driver = MotorDriverL293(Pins::right_driver_a, Pins::right_driver_b);

static ServoMotor::Settings servo_settings = {
    .update_period_seconds = 1000 * 1e-6,
    .ready_max_abs_error = 5,
    .min_speed_limit = 80,
    .delta_position = {
        .regulator = {
            .pid = {
                .kp = 30, //3.4613900185,
                .ki = 0.0518503077,
                .kd = 0, //155.0851440430,
                .abs_max_i = 255
            },
            .tuner = {
                .mode = pid::TunerMode::no_overshoot,
                .cycles = 20,
            },
            .abs_max_out = 255
        },
        .update_period_seconds = 0.001,
    },
    .position = {
        .pid = {
            .kp = 20, //6.0334749222,
            .ki = 0.1068679616,
            .kd = 1, //453.7068481445,
            .abs_max_i = 480
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 10,
        },
        .abs_max_out = 600
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

PositionController::Settings position_controller_settings = {
    .max_area_width = 1200,
    .max_area_height = 1200,
};

PositionController vart::position_controller = PositionController(position_controller_settings, left_pulley, right_pulley);

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