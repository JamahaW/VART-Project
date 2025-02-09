#include "ui2/impl/Page.hpp"


ui2::impl::page::MainPage::MainPage() :
    Page("Main") {
    add(new MediaPage());
    add(new WorkAreaPage());
    add(new MovementServicePage());
    add(new ToolServicePage());
}

ui2::impl::page::MainPage &ui2::impl::page::MainPage::getInstance() {
    static MainPage instance;
    return instance;
}
