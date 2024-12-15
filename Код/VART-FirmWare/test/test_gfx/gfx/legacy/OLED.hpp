#pragma once

#include <Print.h>
#include <cstdint>


class OLED : public Print {

private:
    const uint8_t address;

    uint8_t cursor_x = 0;
    uint8_t cursor_row = 0;
    uint8_t writes = 0;

    void sendByte(uint8_t data);

    void sendCommand(uint8_t cmd1);

    void sendTwoCommands(uint8_t cmd2);

    void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

    void beginData();

    void beginCommand();

    void beginOneCommand();

    void endTransmission();

    void updateTextWindow();

    void beginTransmission(uint8_t mode) const;

public:

    /// @brief
    /// @param address адрес. по умолчанию 0x3C
    explicit OLED(uint8_t address = 0x3C);

    size_t write(uint8_t data) override;

    /// @brief инициализация дисплея
    void init(uint32_t clock = 200000UL);

    /// @brief установить яркость
    /// @param value яркость (0-255), дисплей выключается при яркости 0
    void setBright(uint8_t value);


    /// @brief Установить инверсию цвета дисплея глобально
    void setInvertColor(bool mode);

    /// @brief Установить инверсию шрифта
    void setInvertText(bool mode);

    /// @brief Отразить дисплей по вертикали
    void setFlipV(bool mode);

    /// @brief Отразить дисплей по горизонтали
    void setFlipH(bool mode);
};
