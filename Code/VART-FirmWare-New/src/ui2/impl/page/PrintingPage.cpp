#include "VartPages.hpp"


#include "misc/Macro.hpp"

#include "ui2/impl/widget/Builtin.hpp"
#include "bytelang/impl/VartInterpreter.hpp"
#include "vart/Device.hpp"


using ui2::impl::widget::Button;
using ui2::impl::widget::CheckBox;
using ui2::impl::widget::ProgressBar;
using ui2::impl::widget::Text;

using bytelang::impl::VartInterpreter;
using vart::Device;

ui2::impl::page::PrintingPage::PrintingPage() :
    abc::Page("Printing") {
    auto &i = VartInterpreter::getInstance();

    add(allocStatic(Button(Text("Stop!"), [&i]() { i.is_aborted = true; })));
    add(allocStatic(CheckBox(Text("Pause"), [&i](bool p) { i.is_paused = p; })));
    add(allocStatic(ProgressBar(Device::getInstance().context.progress)));
}
