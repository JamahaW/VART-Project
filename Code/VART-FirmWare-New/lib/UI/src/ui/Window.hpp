#pragma once

#include <functional>
#include <EncButton.h>


#include <ui/Page.hpp>
#include <ui/Event.hpp>
#include <gfx/OLED.hpp>


namespace ui {
    class Window {

    private:
        std::function<Event()> input_handler;

    public:
        gfx::OLED &display;
        Page main_page;
        Page *current_page{};

        explicit Window(gfx::OLED &display, std::function<Event()> &&input_handler);

        void update();

        void setPage(Page *new_page);
    };
}