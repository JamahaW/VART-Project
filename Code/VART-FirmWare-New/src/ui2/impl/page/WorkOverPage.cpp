#include "ui2/impl/Page.hpp"
#include "ui2/impl/widget/Display.hpp"
#include "ui2/impl/widget/Button.hpp"

#include "vart/Device.hpp"


using ui2::impl::widget::Button;
using ui2::impl::widget::Display;
using vart::Device;

ui2::impl::page::WorkOverPage::WorkOverPage() :
    Page("Work Over") {

    add(MainPage::getInstance().to_this_page);
    add(new Display<int>("QUIT CODE", Device::getInstance().context.quit_code));
}
