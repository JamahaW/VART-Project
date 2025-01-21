#include "vart/Devices.hpp"
#include "vart/util/Pins.hpp"


using hardware::MotorDriverL293;
using hardware::ServoMotor;
using vart::Pins;
using vart::Pulley;
using vart::PositionController;
using vart::Area;
using vart::Planner;

static auto left_encoder = hardware::Encoder(Pins::LeftEncoderA, Pins::LeftEncoderB);

static auto right_encoder = hardware::Encoder(Pins::RightEncoderA, Pins::RightEncoderB);

static auto left_driver = MotorDriverL293(Pins::LeftDriverA, Pins::LeftDriverB);

static auto right_driver = MotorDriverL293(Pins::RightDriverA, Pins::RightDriverB);

static ServoMotor::Settings servo_settings = {
    .update_period_seconds = 1 * 1e-3,
    .ready_max_abs_error = 30,
    .position = {
        .pid = {
            .kp = 10,
            .ki = 3,
            .kd = 0.2,
            .abs_max_i = 255
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

Area::Settings area_settings = {
    .max_area_size = {4000, 4000},
    .min_area_size = {500, 500},
    .default_area_size = {1200, 1200},
};

static auto area = Area(area_settings);

static auto position_controller = PositionController(area, left_pulley, right_pulley);

static Planner::Settings settings = {
    .default_mode = Planner::Mode::Position,
    .speed_range = {
        .min = 5,
        .max = 150
    },
    .default_speed = 50,
    .accel_range = {
        .min = 25,
        .max = 100
    },
    .default_accel = 50
};

Planner vart::planner = Planner(settings, position_controller);
