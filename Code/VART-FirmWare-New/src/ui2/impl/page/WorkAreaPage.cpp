#include "VartPages.hpp"

#include "vart/Device.hpp"
#include "misc/Macro.hpp"


using vart::Device;

using ui2::impl::widget::Text;
using ui2::impl::widget::SpinBox;
using ui2::impl::widget::Named;

static constexpr const SpinBox<short>::Settings spin_box_settings = {.min = 100, .max = 4000, .step = 25,};

ui2::impl::page::WorkAreaPage::WorkAreaPage() :
    Page("Work Area") {

    auto &c = Device::getInstance().planner.getController();
    auto init_size = c.getAreaSize();

    static SpinBox<short> width_spin_box(spin_box_settings, short(init_size.x), [&c](short w) {
        c.setAreaSize({double(w), c.getAreaSize().y});
    });
    add(allocStatic(Named(Text("Width"), width_spin_box)));

    static SpinBox<short> height_spin_box(spin_box_settings, short(init_size.x), [&c](short h) {
        c.setAreaSize({c.getAreaSize().x, double(h)});
    });
    add(allocStatic(Named(Text("Height"), height_spin_box)));
}
