#include "ui2/impl/Page.hpp"
#include "ui2/impl/widget/CheckBox.hpp"
#include "ui2/impl/widget/NamedSpinBox.hpp"
#include "vart/MarkerPrintTool.hpp"
#include "vart/Device.hpp"


using vart::Device;
using Angle = vart::MarkerPrintTool::Angle;
using Marker = vart::MarkerPrintTool::Marker;
using ui2::impl::widget::CheckBox;
using SB = ui2::impl::widget::NamedSpinBox<Angle>;

static constexpr const SB::In::Settings s = {0, 180, 1};

#define MarkerTool(m) (__extension__( {static SB __s(#m, SB::In(s, t.getToolAngle(m), [&t](Angle a) {t.updateToolAngle(m, a);t.setActiveTool(m);})); &__s;} ))


ui2::impl::page::ToolServicePage::ToolServicePage() :
    Page("Tool Service") {
    auto &t = vart::Device::getInstance().tool;
    add(new CheckBox("Enable", [&t](bool e) { t.servo.setEnabled(e); }));
    add(MarkerTool(Marker::None));
    add(MarkerTool(Marker::Left));
    add(MarkerTool(Marker::Right));
}
