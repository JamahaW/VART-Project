#include "VartPages.hpp"

#include "vart/MarkerPrintTool.hpp"
#include "vart/Device.hpp"
#include "misc/Macro.hpp"


using vart::Device;
using Angle = vart::MarkerPrintTool::Angle;
using Marker = vart::MarkerPrintTool::Marker;
using ui2::impl::widget::CheckBox;
using ui2::impl::widget::Named;
using ui2::impl::widget::SpinBox;
using ui2::impl::widget::Text;

static constexpr const SpinBox<Angle>::Settings s = {0, 180, 1};

#define ToolWidget(m) ({                                                \
    static SpinBox <Angle> _s(s, t.getToolAngle(m), [&t](Angle a) {     \
        t.updateToolAngle(m, a);                                        \
        t.setActiveTool(m);                                             \
    });                                                                 \
    static Named _n(Text(#m), _s);                                      \
    &_n;                                                                \
})

ui2::impl::page::ToolServicePage::ToolServicePage() :
    Page("Tool Service") {
    auto &t = vart::Device::getInstance().tool;

    add(allocStatic(CheckBox(Text("Enable"), [&t](bool e) { t.servo.setEnabled(e); })));
    add(ToolWidget(Marker::None));
    add(ToolWidget(Marker::Left));
    add(ToolWidget(Marker::Right));
}
