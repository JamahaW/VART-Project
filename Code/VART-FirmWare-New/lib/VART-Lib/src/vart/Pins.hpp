#pragma once

/// Vertical ArtBot Technology
namespace vart {
    enum Pins : unsigned char {

        ///  Адаптер SD-карты
        sd_cs = 5,
        sd_mosi = 23,
        sd_clk = 18,
        sd_miso = 19,

        ///  Энкодер-кнопка пользователя
        user_encoder_a = 34,
        user_encoder_b = 35,
        user_encoder_button = 32,

        /// Левый регулятор мотора
        left_encoder_a = 14, // Желтый провод
        left_encoder_b = 13, // Зеленый провод
        left_driver_a = 33,
        left_driver_b = 25,
        // Красный провод -  OUT 4
        // Белый провод -    OUT 3

        /// Правый регулятор мотора
        right_encoder_a = 17, // Зеленый провод
        right_encoder_b = 16, // Желтый провод
        right_driver_a = 26,
        right_driver_b = 27,
        // Красный провод -  OUT 1
        // Белый провод -    OUT 2

        ///  Сервопривод смены инструмента
        tool_servo = 4,
    };
}