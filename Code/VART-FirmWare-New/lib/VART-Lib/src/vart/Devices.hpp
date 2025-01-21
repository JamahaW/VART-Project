/// Устройства VART

#pragma once

#include "ui/Window.hpp"
#include "vart/Planner.hpp"
#include "vart/MarkerPrintTool.hpp"


namespace vart {
    /// Окно графического интерфейса
    extern ui::Window window;

    /// Планировщик движения
    extern Planner planner;

    /// Инструмент печати
    extern MarkerPrintTool marker_print_tool;
}