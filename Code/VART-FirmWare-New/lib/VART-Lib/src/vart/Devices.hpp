#pragma once

#include "ui2/Window.hpp"
#include "bytelang/StreamInterpreter.hpp"
#include "vart/Device.hpp"
#include <gfx/OLED.hpp>

namespace vart {
    /// Окно графического интерфейса
    extern ui2::Window window;

    /// Действующие настройки устройства
    extern const DeviceSettings settings;

    /// Устройство
    extern Device device;

    /// Контекст для исполнения инструкций
    struct Context {
        /// Прогресс печати
        int progress;

        /// Код завершения печати
        int quit_code;
    };

    extern Context context;

    /// Конкретизация интерпретатора
    using VartIntepreter = bytelang::StreamInterpreter<Context>;

    /// Потоковый интерпретатор команд
    extern VartIntepreter interpreter;
}