/// Устройства VART

#pragma once

#include "vart/PositionController.hpp"
#include "vart/ToolController.hpp"
#include "ui/Window.hpp"


namespace vart {

    /// Контроллер положения
    extern PositionController position_controller;

    extern ToolController tool_controller;

    extern ui::Window window;
}