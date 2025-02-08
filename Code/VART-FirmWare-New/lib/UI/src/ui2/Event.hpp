#pragma once

namespace ui2 {
    enum class Event : char {
        /// Принудительно Обновить страницу
        ForceUpdate,

        /// (Для виджета) Клик
        Click,

        /// (Для виджета) Изменение вверх
        StepUp,

        /// (Для виджета) Изменение вниз
        StepDown,

        /// Выбрать следующий виджет
        NextWidget,

        /// Выбрать предыдущий виджет
        PreviousWidget,
    };
}