#pragma once

#include <hardware/ServoController.hpp>


namespace vart {
    class ToolController {
    public:

        enum Tool : unsigned char {
            none = 0,
            left,
            middle,
            right,
            tool_total_count
        };

        using PositionArray = std::array<int, tool_total_count>;

        hardware::ServoController servo_controller;

        PositionArray positions;

        explicit ToolController(hardware::ServoController &&servo, PositionArray &&positions) :
            servo_controller(servo),
            positions(positions) {}

        void setActiveTool(Tool tool) {
            servo_controller.setPosition(positions.at(tool));
        }
    };
}