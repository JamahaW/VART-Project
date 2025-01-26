#include "Page.hpp"

#include "impl/PageSetterButton.hpp"


ui2::Page *ui2::Page::add(const char *child) {
    auto child_page = new Page(window, child);
    add(new impl::PageSetterButton(*child_page, window));
    child_page->add(to_this_page);
    return child_page;
}

ui2::Page::Page(ui2::Window &window, const char *title) :
    title(title), window(window), to_this_page(new impl::PageSetterButton(*this, window)) {}
