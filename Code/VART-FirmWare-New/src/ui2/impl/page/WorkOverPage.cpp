#include "VartPages.hpp"
#include "misc/Macro.hpp"

#include "vart/Device.hpp"


using namespace ui2::impl::widget;
using vart::Device;

ui2::impl::page::WorkOverPage::WorkOverPage() :
    Page("Work Over") {

    add(MainPage::getInstance().to_this_page);

    static Display<int> display(Device::getInstance().context.quit_code);
    add(allocStatic(Named(Text("Quit Code"), display)));
}
