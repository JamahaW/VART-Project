#pragma once

#include <Print.h>


/// Graphics API
template<class Impl> class Graphics : public Print {

private:
    constexpr Impl *impl() const { return static_cast<Impl *>(this); }

public:

    const uint8_t width;
    const uint8_t height;

    mutable uint8_t cursor_x{0};
    mutable uint8_t cursor_y{0};

    explicit Graphics(uint8_t width, uint8_t height) :
        width{width}, height{height} {}

    size_t write(uint8_t character) override { impl()->write_impl(character); }

    void init() { impl()->init_impl(); }

    void clean() { impl()->clean_impl(); }

    void setCursor(uint8_t x, uint8_t y) {
        this->cursor_x = x;
        this->cursor_y = y;
    }

};
