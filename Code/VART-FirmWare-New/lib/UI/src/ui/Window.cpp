#include <ui/Window.hpp>


ui::Window::Window(gfx::OLED &display, std::function<Event()> &&input_handler) :
    display(display), main_page(*this, "Main"), current_page(&main_page), input_handler(input_handler) {
}

void ui::Window::update() {
    if (current_page->handleInput(input_handler())) {
        current_page->render(display);
    }
}

void ui::Window::setPage(ui::Page *new_page) {
    current_page = new_page;
    display.clear();
    update();
}
