#pragma once

#include <vector>
#include "Arduino.h"
#include "abc/Display.hpp"
#include "abc/Widget.hpp"
#include "Event.hpp"


namespace ui2 {

    struct Window;

    /// Отображаемая страница
    struct Page {
        Window &window;

        /// Заголовок страницы
        const char *title;

        /// Виджет для перехода на эту страницу
        abc::Widget *to_this_page;

        /// Виджеты
        std::vector<abc::Widget *> widgets;

        /// Текущий индекс выбранного виджета
        int cursor = 0;

        explicit Page(Window &window, const char *title);

        /// Добавить виджет
        void add(abc::Widget *widget) { widgets.push_back(widget); }

        /// Добавить вложенную страницу
        Page *add(const char *child);

        /// Отрисовать страницу
        void render(abc::Display &display) const {
            display.setCursor(0, 0);
            display.println(title);

            const auto gui_last_item_index = display.getRows() - 3;

            uint8_t begin = max(cursor - gui_last_item_index, 0);
            uint8_t end = _min(widgets.size(), gui_last_item_index + 1) + begin;

            for (int index = begin; index < end; index++) {
                widgets.at(index)->render(display, index == cursor);
                display.println();
            }
        }

        /// Отреагировать на событие
        void onEvent(Event event) {
            switch (event) {
                case Event::ForceUpdate:
                    return;

                case Event::NextWidget:
                    changeCursor(1);
                    return;

                case Event::PreviousWidget:
                    changeCursor(-1);
                    return;

                case Event::Click:
                case Event::StepUp:
                case Event::StepDown:
                    widgets[cursor]->onEvent(event);
                    return;
            }
        }

    private:

        void changeCursor(int offset) {
            const int new_cursor_position = cursor + offset;
            const unsigned int last_index = widgets.size() - 1;
            cursor = constrain(new_cursor_position, 0, last_index);
        }
    };
}