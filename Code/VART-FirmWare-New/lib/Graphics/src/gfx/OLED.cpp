#include <Arduino.h>
#include <Wire.h>

#include <gfx/OLED.hpp>


static constexpr uint32_t gfx_font_32[] = {
    0x00000000,  //   (32)
    0x00000df7,  // ! (33)
    0x40007007,  // " (34)
    0xd2fd2fd2,  // # (35)
    0xc60dec18,  // $ (36)
    0xf1c8c4e3,  // % (37)
    0xe06e59b8,  // & (38)
    0x00000007,  // ' (39)
    0x4002185e,  // ( (40)
    0x4001e861,  // ) (41)
    0x40014214,  // * (42)
    0x40008708,  // + (43)
    0x00000638,  // , (44)
    0x40008208,  // - (45)
    0x00000618,  // . (46)
    0x40003330,  // / (47)
    0xde86d85e,  // 0 (48)
    0x8083f880,  // 1 (49)
    0xe6a69a72,  // 2 (50)
    0xda965852,  // 3 (51)
    0xd0fd2518,  // 4 (52)
    0xd9965963,  // 5 (53)
    0xd0a69a9c,  // 6 (54)
    0xc3159841,  // 7 (55)
    0xda96595a,  // 8 (56)
    0xdea69a46,  // 9 (57)
    0x000006db,  // : (58)
    0x000006fb,  // ; (59)
    0x40011284,  // < (60)
    0x8028a28a,  // = (61)
    0x40004291,  // > (62)
    0xc7165b41,  // ? (63)
    0xefa6d87f,  // @ (64)
    0xfe24927e,  // A (65)
    0xfa96597f,  // B (66)
    0xd286185e,  // C (67)
    0xde86187f,  // BEGIN (68)
    0xe186597f,  // E (69)
    0xc114517f,  // F (70)
    0xd8a6985e,  // G (71)
    0xff20823f,  // H (72)
    0x40021fe1,  // I (73)
    0x4003d870,  // J (74)
    0xf128413f,  // K (75)
    0xe082083f,  // L (76)
    0xff0840bf,  // M (77)
    0xff2040bf,  // N (78)
    0xde86185e,  // O (79)
    0xc624927f,  // P (80)
    0xee46985e,  // Q (81)
    0xf624927f,  // R (82)
    0xd2a69952,  // S (83)
    0xc107f041,  // T (84)
    0xdf82081f,  // U (85)
    0xc7620607,  // V (86)
    0xdf81c81f,  // W (87)
    0xf12842b1,  // X (88)
    0xc3138103,  // Y (89)
    0xe3965a71,  // Z (90)
    0x4002187f,  // [ (91)
    0x40030303,  // \ (92)
    0x4003f861,  // ] (93)
    0xc4081084,  // ^ (94)
    0xc4210204,  // _ (95)
    0x40004081,  // ` (96)
    0xdcaaaa90,  // a (97)
    0xd892493f,  // b (98)
    0xd48a289c,  // c (99)
    0xff924918,  // d (100)
    0xccaaaa9c,  // e (101)
    0x8027e200,  // f (102)
    0xdcaaaa84,  // g (103)
    0xf810413f,  // h (104)
    0x00000f40,  // i (105)
    0x00000760,  // j (106)
    0xe250823f,  // k (107)
    0x0000081f,  // l (108)
    0xfc0bc0be,  // m (109)
    0xfc0820be,  // n (110)
    0xdc8a289c,  // o (111)
    0xc428a2bc,  // p (112)
    0xfc28a284,  // q (113)
    0xc210423e,  // r (114)
    0xd2aaaaa4,  // s (115)
    0x400247c4,  // t (116)
    0xde82081e,  // u (117)
    0xce42040e,  // v (118)
    0xde81881e,  // w (119)
    0xe2508522,  // x (120)
    0xdea28a06,  // y (121)
    0xe6aaaab2,  // z (122)
    0x40021ccc,  // { (123)
    0x00000fc0,  // | (124)
    0x4000cce1,  // } (125)
    0x80108108,  // ~ (126)
};

enum OledCmd : unsigned char {
    oled_height_64 = 0x12,
    oled_64 = 0x3F,
    oled_display_off = 0xAE,
    oled_display_on = 0xAF,
    oled_command_mode = 0x00,
    oled_one_command_mode = 0x80,
    oled_data_mode = 0x40,
    oled_addressing_mode = 0x20,
    oled_vertical = 0x01,
    oled_normal_v = 0xC8,
    oled_flip_v = 0xC0,
    oled_normal_h = 0xA1,
    oled_flip_h = 0xA0,
    oled_contrast = 0x81,
    oled_set_com_pins = 0xDA,
    oled_set_v_com_detect = 0xDB,
    oled_clock_div = 0xD5,
    oled_set_multiplex = 0xA8,
    oled_column_address = 0x21,
    oled_page_address = 0x22,
    oled_charge_pump = 0x8D,
    oled_normal_display = 0xA6,
    oled_invert_display = 0xA7,
    oled_max_row = 7,
    oled_max_x = 127,
    oled_font_width = 6,
};

static uint32_t getFont(uint8_t code) {
    const uint32_t unknown = gfx_font_32['?'];
    return (code < 127) ? gfx_font_32[code - 32] : unknown;
}

static const uint8_t oled_init_commands[] = {
    oled_display_off, oled_clock_div, 0x80,  // value
    oled_charge_pump, 0x14,  // value
    oled_addressing_mode, oled_vertical, oled_normal_h, oled_normal_v, oled_contrast, 0x7F,  // value
    oled_set_v_com_detect, 0x40,  // value
    oled_normal_display, oled_display_on,
};

gfx::OLED::OLED(uint8_t address) :
    address(address) {
}

#define OLED_FONT_GET_COL(f, col) (((f) >> (col)) & 0b111111)
#define OLED_FONT_GET_WIDTH(f) (((f) >> 30) & 0b11)

size_t gfx::OLED::write(uint8_t data) {
    if (data == 0 || data > 191 || isEndY() || data == '\r' || isEndX()) {
        return 0;
    }

    if (data == '\n') {
        clearAfterCursor();
        setCursor(0, cursor_row + 1);
        return 1;
    }

    uint32_t bits = getFont(data);
    uint8_t width_6 = (2 * oled_font_width) + OLED_FONT_GET_WIDTH(bits) * oled_font_width;
    uint8_t col;

    beginData();

    for (uint8_t offset = 0; offset < width_6; offset += oled_font_width, cursor_x++) {
        col = OLED_FONT_GET_COL(bits, offset) ^ text_mask;

        for (uint8_t t = 0; t < 1; t++) {
            sendByte(col);
        }
    }


    sendByte(text_mask);

    endTransmission();

    cursor_x++;
    return 1;
}

void gfx::OLED::init(uint32_t clock) {
    Wire.begin();
    Wire.setClock(clock);

    beginCommand();
    for (uint8_t command: oled_init_commands) {
        sendByte(command);
    }
    endTransmission();

    beginCommand();
    sendByte(oled_set_com_pins);
    sendByte(oled_height_64);
    sendByte(oled_set_multiplex);
    sendByte(oled_64);
    endTransmission();

    clear();
}

void gfx::OLED::clear() {
    clear(0, 0, oled_max_x, oled_max_row);
    setCursor(0, 0);
}

void gfx::OLED::clear(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    setWindow(x0, y0, x1, y1);

    beginData();

    uint16_t end = (x1 - x0 + 1) * (y1 - y0 + 1);

    for (uint16_t i = 0; i < end; i++) {
        sendByte(0);
    }

    endTransmission();
}

void gfx::OLED::clearAfterCursor() {
    clear(cursor_x, cursor_row, oled_max_x, cursor_row);
}

void gfx::OLED::setCursor(uint8_t new_x, uint8_t new_row) {
    cursor_x = new_x;
    cursor_row = new_row;
    updateTextWindow();
}

void gfx::OLED::setBright(uint8_t value) {
    sendCommand((value > 0) ? oled_display_on : oled_display_off);
    sendTwoCommands(value);
}

void gfx::OLED::setInvertColor(bool mode) {
    sendCommand(mode ? oled_invert_display : oled_normal_display);
}

void gfx::OLED::setInvertText(bool mode) {
    text_mask = 0xFF * mode;
}

void gfx::OLED::setFlipV(bool mode) {
    sendCommand(mode ? oled_flip_v : oled_normal_v);
}

void gfx::OLED::setFlipH(bool mode) {
    sendCommand(mode ? oled_flip_h : oled_normal_h);
}

void gfx::OLED::sendByte(uint8_t data) {
    Wire.write(data);

    if (++writes >= 16) {
        endTransmission();
        beginData();
    }
}

void gfx::OLED::sendCommand(uint8_t cmd1) {
    beginOneCommand();
    Wire.write(cmd1);
    endTransmission();
}

void gfx::OLED::sendTwoCommands(uint8_t cmd2) {
    beginCommand();
    Wire.write(129);
    Wire.write(cmd2);
    endTransmission();
}

void gfx::OLED::setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    beginCommand();
    Wire.write(oled_column_address);
    Wire.write(constrain(x0, 0, oled_max_x));
    Wire.write(constrain(x1, 0, oled_max_x));
    Wire.write(oled_page_address);
    Wire.write(constrain(y0, 0, oled_max_row));
    Wire.write(constrain(y1, 0, oled_max_row));
    endTransmission();
}

void gfx::OLED::updateTextWindow() {
    setWindow(cursor_x, cursor_row, oled_max_x, cursor_row);
}

void gfx::OLED::beginData() {
    beginTransmission(oled_data_mode);
}

void gfx::OLED::beginCommand() {
    beginTransmission(oled_command_mode);
}

void gfx::OLED::beginOneCommand() {
    beginTransmission(oled_one_command_mode);
}

void gfx::OLED::endTransmission() {
    Wire.endTransmission();
    writes = 0;
}

void gfx::OLED::beginTransmission(uint8_t mode) const {
    Wire.beginTransmission(address);
    Wire.write(mode);
}

bool gfx::OLED::isEndY() const {
    return cursor_row > oled_max_row;
}

bool gfx::OLED::isEndX() const {
    return cursor_x > oled_max_x - oled_font_width;
}
