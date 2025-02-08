#pragma once

#include "Planner.hpp"
#include "MarkerPrintTool.hpp"


namespace vart {

    /// Настройки устройства
    struct DeviceSettings {
        hardware::ServoMotor::Settings servomotor;
        Pulley::Settings pulley;
        Area::Settings area;
        Planner::Settings planner;
        MarkerPrintTool::Settings marker_tool;
        hardware::ServoMG90S::Settings servo_mg90s;
    };

    /// Устройство (VART)
    struct Device {
        /// Планировщик перемещений
        Planner planner;

        /// Инструмент печати
        MarkerPrintTool tool;
    };


}