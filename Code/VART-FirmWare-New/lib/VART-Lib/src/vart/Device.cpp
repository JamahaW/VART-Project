#include "Device.hpp"
#include "vart/util/Pins.hpp"


vart::Device &vart::Device::getInstance() {
    static Settings settings = {
        .servomotor = {
            .update_period_seconds = 1 * 1e-3,
            .ready_max_abs_error = 30,
            .position = {
                .kp = 10,
                .ki = 3,
                .kd = 0.2,
                .abs_max_i = 255,
                .abs_max_out = 255
            },
        },
        .area = {
            .max_area_size = {4000, 4000},
            .min_area_size = {500, 500},
            .default_area_size = {1200, 1200},
        },
        .pulley = {.ticks_in_mm = 5000.0 / 280.0},
        .planner = {
            .default_mode = vart::Planner::Mode::Accel,
            .speed_range = {.min = 5, .max = 150},
            .default_speed = 150,
            .accel_range = {.min = 25, .max = 100},
            .default_accel = 25
        },
        .marker_tool = {
            .angle_range = {.min = 30, .max = 120},
            .positions = {78, 42, 118}
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
                Pins::PrintToolServo
            )
        )
    };
    return instance;
}
