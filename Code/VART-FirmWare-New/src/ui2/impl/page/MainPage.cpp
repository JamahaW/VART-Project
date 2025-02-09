#include "VartPages.hpp"
#include "misc/Macro.hpp"


using ui2::impl::page::MainPage;

MainPage::MainPage() :
    Page("Main") {
    add(allocStatic(MediaPage()));
    add(allocStatic(WorkAreaPage()));
    add(allocStatic(MovementServicePage()));
    add(allocStatic(ToolServicePage()));
}

MainPage &MainPage::getInstance() {
    static MainPage instance;
    return instance;
}
