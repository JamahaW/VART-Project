#include <Arduino.h>

#include <gfx/OLED.hpp>

#include <ui/Page.hpp>
#include <ui/Window.hpp>
#include <ui/Widget.hpp>


ui::Page::Page(ui::Window &window, const char *title) :
    window(window), title(title), to_this_page(ValueType::Chars, (void *) this->title, [&window, this](Widget *w) {
    window.current_page = this;
    window.display.clear();
}) {
    to_this_page.setStyle(ui::Style::arrow_prefix);
}

void ui::Page::render(gfx::OLED &display) const {
    constexpr auto gui_last_item_index = 5;

    display.setCursor(0, 0);

    display.println(title);

    uint8_t begin = max(cursor - gui_last_item_index, 0);
    uint8_t end = _min(items.size(), gui_last_item_index + 1) + begin;

    for (uint8_t i = begin; i < end; i++) {
        items[i]->render(display, i == cursor);
        display.println();
    }

    display.clearAfterCursor();
}

bool ui::Page::handleInput(ui::Event e) {
    switch (e) {
        case Event::click:items[cursor]->onClick();
            return true;
        case Event::next_item:moveCursor(1);
            return true;
        case Event::past_item:moveCursor(-1);
            return true;
        case Event::change_up:items[cursor]->onChange(1);
            return true;
        case Event::change_down:items[cursor]->onChange(-1);
            return true;
        case Event::idle:
        default:return false;
    }
}

void ui::Page::moveCursor(int delta) {
    cursor = constrain(cursor + delta, 0, items.size() - 1);
}

ui::Page *ui::Page::addPage(const char *child_title) {
    auto p = new Page(window, child_title);
    items.push_back(&p->to_this_page);
    p->items.push_back(&to_this_page);
    return p;
}

void ui::Page::addItem(Item *w) {
    items.push_back(w);
}


