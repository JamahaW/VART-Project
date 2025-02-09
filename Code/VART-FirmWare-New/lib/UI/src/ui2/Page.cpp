#include "Page.hpp"
#include "ui2/impl/PageSetterButton.hpp"


ui2::Page *ui2::Page::add(Page *child) {
    Page *parent = this;

    parent->add(child->to_this_page);
    child->add(parent->to_this_page);

    return child;
}

ui2::Page::Page(const char *title, const std::function<void(Page &)> &on_entry) :
    title(title), to_this_page{new impl::PageSetterButton(*this, on_entry)} {}
