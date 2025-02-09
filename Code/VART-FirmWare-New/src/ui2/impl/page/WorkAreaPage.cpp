#include "ui2/impl/Page.hpp"
#include "ui2/impl/widget/NamedSpinBox.hpp"
#include "vart/Device.hpp"


using vart::Device;

using SB = ui2::impl::widget::NamedSpinBox<short>;

static constexpr const char *name = "Work Area";

static constexpr const char *width = "Width";

static constexpr const char *height = "Height";

static constexpr const SB::In::Settings spin_box_settings = {.min = 100, .max = 4000, .step = 25,};

ui2::impl::page::WorkAreaPage::WorkAreaPage() :
    Page(name) {

    auto &c = Device::getInstance().planner.getController();
    auto init_size = c.getAreaSize();

    add(new SB(width, SB::In(spin_box_settings, short(init_size.x), [&c](short w) {
        c.setAreaSize({double(w), c.getAreaSize().y});
    })));

    add(new SB(height, SB::In(spin_box_settings, short(init_size.y), [&c](short h) {
        c.setAreaSize({c.getAreaSize().x, double(h)});
    })));
}
