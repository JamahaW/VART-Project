#pragma once

#include <vector>
#include "Arduino.h"

#include "ui2/Event.hpp"
#include "ui2/abc/Widget.hpp"
#include "ui2/abc/Display.hpp"
#include "ui2/impl/Text.hpp"


namespace ui2 {

    struct Window;

    /// Отображаемая страница
    struct Page {
        Window &window;

        /// Заголовок страницы
        const impl::Text title;

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
            const auto gui_last_item_index = display.getRows() - 3;

            display.setCursor(0, 0);

            title.render(display);
            display.println();

            uint8_t begin = max(cursor - gui_last_item_index, 0);
            uint8_t end = _min(widgets.size(), gui_last_item_index + 1) + begin;

            for (int index = begin; index < end; index++) {
                display.setTextInverted(index == cursor);
                widgets.at(index)->render(display);
                display.setTextInverted(false);
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

        void clearWidgets(int except) {
            auto to_parent = widgets.at(0);
            auto w = widgets.at(except);
            widgets.clear();
            add(to_parent);
            add(w);
        }

    private:

        void changeCursor(int offset) {
            const int new_cursor_position = cursor + offset;
            const unsigned int last_index = widgets.size() - 1;
            cursor = constrain(new_cursor_position, 0, last_index);
        }
    };
}