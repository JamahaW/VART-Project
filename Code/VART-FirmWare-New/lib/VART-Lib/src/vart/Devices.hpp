#pragma once

#include "ui2/Window.hpp"
#include "vart/Planner.hpp"
#include "vart/MarkerPrintTool.hpp"
#include "bytelang/StreamInterpreter.hpp"
#include "gfx/OLED.hpp"


namespace vart {
    /// Окно графического интерфейса
    extern ui2::Window window;

    /// Контекст для исполнения инструкций
    struct VartContext {
        /// Используемый планировщик
        Planner &planner;

        /// Используемый инструмент
        MarkerPrintTool &tool;

        /// Прогресс печати
        int progress;

        /// Код завершения печати
        int quit_code;
    };

    extern VartContext context;

    /// Конкретизация интерпретатора
    using VartIntepreter = bytelang::StreamInterpreter<VartContext>;

    /// Потоковый интерпретатор команд
    extern VartIntepreter interpreter;
}