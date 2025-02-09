#include "ui2/impl/Page.hpp"
#include "ui2/impl/widget/Button.hpp"
#include "ui2/impl/widget/CheckBox.hpp"
#include "ui2/impl/widget/ProgressBar.hpp"

#include "bytelang/impl/VartInterpreter.hpp"
#include "vart/Device.hpp"


using ui2::impl::widget::Button;
using ui2::impl::widget::CheckBox;
using ui2::impl::widget::ProgressBar;

using bytelang::impl::VartInterpreter;
using vart::Device;

static constexpr const char *name = "Printing";

ui2::impl::page::PrintingPage::PrintingPage() :
    abc::Page(name) {
    auto &interpreter = VartInterpreter::getInstance();

    add(new Button("Stop!", [&interpreter]() { interpreter.is_aborted = true; }));
    add(new CheckBox("Pause", [&interpreter](bool p) { interpreter.is_paused = p; }));
    add(new ProgressBar<int>(Device::getInstance().context.progress));
}
