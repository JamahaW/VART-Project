#pragma once

#include <vector>
#include <functional>
#include "Arduino.h"

#include "ui2/Event.hpp"
#include "ui2/abc/Widget.hpp"
#include "ui2/abc/Display.hpp"
#include "ui2/impl/widget/Text.hpp"


namespace ui2 {
    namespace abc {
        /// Отображаемая страница
        struct Page {
            /// Заголовок страницы
            const impl::widget::Text title;
            /// Виджет для перехода на эту страницу
            Widget *to_this_page;
            /// Виджеты
            std::vector<Widget *> widgets;
            /// Текущий индекс выбранного виджета
            int cursor{0};

            explicit Page(const char *title, const std::function<void(Page &)> &on_entry = nullptr);

            /// Добавить виджет
            void add(Widget *widget) { widgets.push_back(widget); }

            /// Добавить вложенную страницу
            void add(Page *child);

            /// Отрисовать страницу
            void render(Display &display) const {
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

            void clearWidgets() {
                auto to_parent = widgets.at(0);
                widgets.clear();
                add(to_parent);
            }

        private:

            void changeCursor(int offset) {
                const int new_cursor_position = cursor + offset;
                const unsigned int last_index = widgets.size() - 1;
                cursor = constrain(new_cursor_position, 0, last_index);
            }
        };
    }
}