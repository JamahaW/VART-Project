#include "Page.hpp"


using ui2::impl::widget::PageSetterButton;
using ui2::abc::Page;

void Page::add(Page *child) {
    Page *parent = this;

    parent->add(child->to_this_page);
    child->add(parent->to_this_page);
}

Page::Page(const char *title, const std::function<void(Page &)> &on_entry) :
    title(title), to_this_page{new PageSetterButton(*this, on_entry)} {}
