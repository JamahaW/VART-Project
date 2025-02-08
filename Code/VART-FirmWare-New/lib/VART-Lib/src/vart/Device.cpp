#include "Device.hpp"
#include "vart/util/Pins.hpp"


vart::Device &vart::Device::getInstance() {
    static Settings settings = {
        .servomotor = {
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
        },
        .servo_mg90s = {
            .speed = 25,
            .accel = 5
        },
        .area = {
            .max_area_size = {
                4000,
                4000
            },
            .min_area_size = {
                500,
                500
            },
            .default_area_size = {
                1200,
                1200
            },
        },
        .pulley = {
            .ticks_in_mm = 5000.0 / 280.0,
        },

        .planner = {
            .default_mode = vart::Planner::Mode::Position,
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
        },
        .marker_tool = {
            .angle_range = {
                .min = 30,
                .max = 120
            },
            .positions = {
                78,
                42,
                118
            }
        }
    };

    static Device instance = {
        .planner = Planner(
            settings.planner,
            {
                .area = Area(settings.area),
                .left_pulley = Pulley(
                    settings.pulley,
                    hardware::ServoMotor(
                        settings.servomotor,
                        hardware::MotorDriverL293(
                            Pins::LeftDriverA,
                            Pins::LeftDriverB
                        ),
                        hardware::Encoder(
                            Pins::LeftEncoderA,
                            Pins::LeftEncoderB
                        )
                    )
                ),
                .right_pulley = Pulley(
                    settings.pulley,
                    hardware::ServoMotor(
                        settings.servomotor,
                        hardware::MotorDriverL293(
                            RightDriverA,
                            RightDriverB
                        ),
                        hardware::Encoder(
                            Pins::RightEncoderA,
                            Pins::RightEncoderB
                        )
                    )
                )
            }
        ),
        .tool = MarkerPrintTool(
            settings.marker_tool,
            hardware::ServoMG90S(
                settings.servo_mg90s,
                Pins::PrintToolServo
            )
        )
    };
    return instance;
}
