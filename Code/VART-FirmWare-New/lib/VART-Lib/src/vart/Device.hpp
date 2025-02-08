#pragma once

#include "Planner.hpp"
#include "MarkerPrintTool.hpp"


namespace vart {
    struct Device {

        struct Settings {
            hardware::ServoMotor::Settings servomotor;
            hardware::ServoMG90S::Settings servo_mg90s;
            Area::Settings area;
            Pulley::Settings pulley;
            Planner::Settings planner;
            MarkerPrintTool::Settings marker_tool;
        };

        struct Context {
            int progress;
            int quit_code;
        };

        Context context;

        Planner planner;

        MarkerPrintTool tool;

        static Device &getInstance();

        Device(const Device &) = delete;

        Device &operator=(const Device &) = delete;
    };
}