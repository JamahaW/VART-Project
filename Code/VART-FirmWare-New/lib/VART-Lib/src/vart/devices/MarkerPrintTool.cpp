#include "vart/Devices.hpp"
#include "vart/util/Pins.hpp"


using vart::MarkerPrintTool;

static MarkerPrintTool::Settings marker_print_tool_settings = {
    .angle_range = {
        .min = 30,
        .max = 120
    },
    .positions = {
        78,
        42,
        118
    },
    .tool_change_duration_ms = 500
};

static const hardware::ServoMG90S servo_mg_90_s = {
    .servo = Servo(),
    .pin = vart::Pins::PrintToolServo
};

MarkerPrintTool vart::marker_print_tool = MarkerPrintTool(marker_print_tool_settings, servo_mg_90_s);

