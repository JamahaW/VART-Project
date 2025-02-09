#include "Page.hpp"
#include "ui2/impl/widget/PageSetterButton.hpp"


ui2::abc::Page *ui2::abc::Page::add(ui2::abc::Page *child) {
    ui2::abc::Page *parent = this;

    parent->add(child->to_this_page);
    child->add(parent->to_this_page);

    return child;
}

ui2::abc::Page::Page(const char *title, const std::function<void(ui2::abc::Page &)> &on_entry) :
    title(title), to_this_page{new impl::widget::PageSetterButton(*this, on_entry)} {}
