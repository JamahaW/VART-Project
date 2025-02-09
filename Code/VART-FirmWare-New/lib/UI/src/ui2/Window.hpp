#pragma once

#include <queue>
#include "ui2/abc/Page.hpp"
#include "Event.hpp"


namespace ui2 {

    /// Окно графического интерфейса
    struct Window {
    private:

        static constexpr int MAX_EVENTS = 8;

        /// Дисплей для отображения графики
        abc::Display *display{nullptr};
        /// Отображаемая страница
        abc::Page *current_page{nullptr};
        /// Очередь событий
        std::queue<Event> events;

    public:

        /// Получить экземпляр окна
        static Window &getInstance() {
            static Window instance;
            return instance;
        }

        /// Установить дисплей для вывода
        void setDisplay(abc::Display &display) {
            this->display = &display;
        }

        /// Сменить страницу
        void setPage(abc::Page &new_page) {
            current_page = &new_page;
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
            if (current_page == nullptr) { return; }
            current_page->render(*display);
        }

        /// Вызвать событие
        void onEvent(Event event) const {
            if (current_page == nullptr) { return; }
            current_page->onEvent(event);
            renderCurrentPage();
        }
    };
}