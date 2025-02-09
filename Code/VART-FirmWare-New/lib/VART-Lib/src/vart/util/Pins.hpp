#pragma once

/// Vertical ArtBot Technology
namespace vart {
    enum Pins : unsigned char {

        ///  Адаптер SD-карты
        SdCs = 5,
        SdMosi = 23,
        SdClk = 18,
        SdMiso = 19,

        ///  Энкодер-кнопка пользователя
        UserEncoderA = 34,
        UserEncoderB = 35,
        UserEncoderButton = 32,

        /// Левый регулятор мотора
        LeftEncoderA = 14, // Желтый провод
        LeftEncoderB = 13, // Зеленый провод
        LeftDriverA = 33,
        LeftDriverB = 25,
        // Красный провод -  OUT 4
        // Белый провод -    OUT 3

        /// Правый регулятор мотора
        RightEncoderA = 17, // Зеленый провод
        RightEncoderB = 16, // Желтый провод
        RightDriverA = 26,
        RightDriverB = 27,
        // Красный провод -  OUT 1
        // Белый провод -    OUT 2

        ///  Сервопривод смены инструмента
        PrintToolServo = 15,
    };
}