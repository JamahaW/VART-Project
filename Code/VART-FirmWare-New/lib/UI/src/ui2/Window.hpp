#pragma once

#include <queue>
#include "Page.hpp"
#include "Event.hpp"


namespace ui2 {

    /// Окно графического интерфейса
    struct Window {

        static constexpr int MAX_EVENTS = 8;

        /// Дисплей для отображения графики
        abc::Display *display{nullptr};

        /// Главная страница
        Page root{Page(*this, "Main")};

        /// Отображаемая страница
        Page *current_page{&root};

        /// Очередь событий
        std::queue<Event> events;

        static Window &getInstance(abc::Display &display) {
            Window &w = getInstance();
            w.display = &display;
            return w;
        }

        /// Получить экземпляр окна
        static Window &getInstance() {
            static Window instance;
            return instance;
        }

        /// Сменить страницу
        void setPage(Page *new_page) {
            current_page = new_page;
            display->clear();
            renderCurrentPage();
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

        // Запрет копирования и присваивания
        Window(const Window &) = delete;

        Window &operator=(const Window &) = delete;

    private:

        Window() = default;

        /// Отобразить текущую страницу
        void renderCurrentPage() const {
            if (display == nullptr) { return; }
            current_page->render(*display);
        }

        /// Вызвать событие
        void onEvent(Event event) const {
            current_page->onEvent(event);
            renderCurrentPage();
        }
    };
}