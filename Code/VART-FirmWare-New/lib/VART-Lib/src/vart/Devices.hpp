/// Устройства VART

#pragma once

#include "ui/Window.hpp"
#include "vart/Planner.hpp"
#include "vart/MarkerPrintTool.hpp"
#include "bytelang/StreamInterpreter.hpp"


namespace vart {
    /// Окно графического интерфейса
    extern ui::Window window;

    /// Контекст для исполнения инструкций
    struct VartContext {
        /// Используемый планировщик
        Planner &planner;

        /// Используемый инструмент
        MarkerPrintTool &tool;

        /// Прогресс печати
        uint8_t progress;
    };

    extern VartContext context;

    /// Конкретизация интерпретатора
    using VartIntepreter = bytelang::StreamInterpreter<VartContext>;

    /// Потоковый интерпретатор команд
    extern VartIntepreter interpreter;
}