#pragma once

#include <Print.h>


namespace ui2 {
    namespace abc {
        /// Дисплей для отображения виджетов
        struct Display : Print {

            using PixelPosition = uint8_t;

            /// Установить курсор
            virtual void setCursor(PixelPosition x, PixelPosition y) = 0;

            /// Очистить дисплей
            virtual void clear() = 0;

            /// Установить инверсию текста
            virtual void setTextInverted(bool is_inverted) = 0;
        };
    }
}