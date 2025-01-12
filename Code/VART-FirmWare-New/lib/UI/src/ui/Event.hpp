#pragma once

namespace ui {
    enum class Event : unsigned char {
        /// Ничего не произошло
        idle,

        /// Клик на виджет
        click,

        /// Выбрать следующий виджет
        next_item,

        /// Выбрать предыдущий виджет

        past_item,

        /// Изменение вверх
        change_up,

        /// Изменение вниз
        change_down,
    };
}
