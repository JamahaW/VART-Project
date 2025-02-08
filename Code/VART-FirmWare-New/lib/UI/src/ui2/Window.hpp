#pragma once

#include <queue>
#include "Page.hpp"
#include "Event.hpp"


namespace ui2 {

    /// Окно графического интерфейса
    struct Window {

        static constexpr int MAX_EVENTS = 16;

        /// Дисплей для отображения графики
        abc::Display &display;

        /// Главная страница
        Page root = Page(*this, "Main");

        /// Отображаемая страница
        Page *current_page = &root;

        /// Очередь событий
        std::queue<Event> events;

        explicit Window(abc::Display &display) :
            display(display) {}

        /// Сменить страницу
        void setPage(Page *new_page) {
            display.clear();
            current_page = new_page;
            current_page->render(display);
        }

        /// Добавить событие
        void addEvent(Event event) {
            if (events.size() > MAX_EVENTS) { return; }
            events.push(event);
        }

        /// Проверка событий
        void pull() {
            if (events.empty()) { return; }
            onEvent(events.front());
            events.pop();
        }

    private:

        /// Вызвать событие
        void onEvent(Event event) {
            current_page->onEvent(event);
            current_page->render(display);
        }
    };
}