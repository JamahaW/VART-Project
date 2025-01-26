#pragma once

namespace ui {
    enum class Event : unsigned char {
        /// Ничего не произошло
        Idle,

        /// Клик на виджет
        Click,

        /// Выбрать следующий виджет
        NextItem,

        /// Выбрать предыдущий виджет

        PastItem,

        /// Изменение вверх
        ChangeUp,

        /// Изменение вниз
        ChangeDown,
    };
}
