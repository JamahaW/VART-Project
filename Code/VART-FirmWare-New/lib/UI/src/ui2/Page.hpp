#pragma once

#include "ui2/abc/Display.hpp"
#include "ui2/abc/Widget.hpp"
#include "ui2/Event.hpp"


namespace ui2 {

    /// Отображаемая страница
    struct Page {
        using WidgetPointer = abc::Widget *;
        using WidgetIndex = int;

        /// Заголовок страницы
        const char *title;

        /// Текущий индекс выбранного виджета
        WidgetIndex cursor;

        /// Последовательность виджетов, обязательно заканчивающаяся на nullptr
        WidgetPointer widgets[];

        /// Отрисовать страницу
        void render(abc::Display &display) const {
            display.setCursor(0, 0);
            display.println(title);

            for (WidgetIndex index = 0; widgets[index] != nullptr; index++) {
                widgets[index]->render(display, index == cursor);
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

        void changeCursor(WidgetIndex offset) {
            WidgetIndex new_cursor_position = cursor + offset;

            if (new_cursor_position < 0) { new_cursor_position = 0; }
            else if (widgets[new_cursor_position] == nullptr) { new_cursor_position = cursor; }

            cursor = new_cursor_position;
        }
    };
}