#pragma once

#include <functional>


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

        explicit Window(gfx::OLED &display, std::function<Event()> &&input_handler) :
            display(display), main_page(*this, "Main"), current_page(&main_page), input_handler(input_handler) {
        }

        void update() {
            if (current_page->handleInput(input_handler())) {
                render();
            }
        }

        void render() { current_page->render(display); }

        void setPage(Page *new_page) {
            current_page = new_page;
            display.clear();
            render();
        }
    };


}