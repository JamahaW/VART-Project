#include "vart/Devices.hpp"


using hardware::Encoder;
using hardware::MotorDriverL293;
using hardware::ServoMotor;
using vart::Pins;
using vart::Pulley;

static ServoMotor::Settings servo_settings = {
    .update_period_seconds = 1000 * 1e-6,
    .ready_max_abs_error = 5,
    .min_speed_limit = 80,
    .delta_position = {
        .regulator = {
            .pid = {
                .kp = 16.1296730042,
                .ki = 0.0492058247,
                .kd = 1000,//2244.5698242188,
                .abs_max_i = 204
            },
            .tuner = {
                .mode = pid::TunerMode::no_overshoot,
                .cycles = 20,
            },
            .abs_max_out = 255
        },
        .update_period_seconds = 0.004,
    },
    .position = {
        .pid = {
            .kp = 6.2555165291,
            .ki = 0.0105996681,
            .kd = 2437.8041992188,
            .abs_max_i = 640
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 10,
        },
        .abs_max_out = 800
    },
};

static auto left_encoder = Encoder(Pins::left_encoder_a, Pins::left_encoder_b);

static auto right_encoder = Encoder(Pins::right_encoder_a, Pins::right_encoder_b);

static auto left_driver = MotorDriverL293(Pins::left_driver_a, Pins::left_driver_b);

static auto right_driver = MotorDriverL293(Pins::right_driver_a, Pins::right_driver_b);

static auto left_servo = ServoMotor(servo_settings, left_driver, left_encoder);

static auto right_servo = ServoMotor(servo_settings, right_driver, right_encoder);

static const Pulley::Settings pulley_settings = {
    .ticks_in_mm = 5000.0 / 280.0,
};

Pulley vart::left_pulley = Pulley(pulley_settings, left_servo);

Pulley vart::right_pulley = Pulley(pulley_settings, right_servo);
