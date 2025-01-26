#pragma once

#include "Page.hpp"
#include "Event.hpp"


namespace ui2 {

    /// Окно графического интерфейса
    struct Window {

        /// Дисплей для отображения графики
        abc::Display &display;

        /// Главная страница
        Page root = Page(*this, "Main");

        /// Отображаемая страница
        Page *current_page = &root;

        explicit Window(abc::Display &display) :
            display(display) {}

        /// Сменить страницу
        void setPage(Page *new_page) {
            current_page = new_page;
            display.clear();
            current_page->render(display);
        }

        /// Вызвать событие
        void onEvent(Event event) {
            current_page->onEvent(event);
            current_page->render(display);
        }
    };
}