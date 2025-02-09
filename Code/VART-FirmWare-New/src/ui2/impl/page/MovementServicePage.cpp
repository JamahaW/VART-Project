#include "VartPages.hpp"


#include "vart/Device.hpp"
#include "bytelang/primitive.hpp"
#include "misc/Macro.hpp"


using M = vart::Planner::Mode;
using vart::Device;

using bytelang::primitive::u8;
using bytelang::primitive::i8;
using bytelang::primitive::i16;

using ui2::impl::widget::Button;
using ui2::impl::widget::CheckBox;
using ui2::impl::widget::Named;
using ui2::impl::widget::Text;
using SbI16 = ui2::impl::widget::SpinBox<i16>;
using SbU8 = ui2::impl::widget::SpinBox<u8>;
using SbI8 = ui2::impl::widget::SpinBox<i8>;

static constexpr const SbI16::Settings position_settings = {.min = -600, .max = 600, .step = 50};

static constexpr const SbU8::Settings speed_settings = {.min = 10, .max = 250, .step = 10};

static constexpr const SbU8::Settings accel_settings = {.min = 5, .max = 250, .step = 5};

static constexpr const SbI8::Settings offsets_settings = {-100, 100, 5};

static SbI16 spin_x(position_settings, 0);

static SbI16 spin_y(position_settings, 0);

#define ModeButton(mode) (__extension__( {static Button __b(Text(#mode), [&p](){p.setMode(mode);}); &__b;} ))

static void moveToTarget() {
    Device::getInstance().planner.getController().setTargetPosition({double(spin_x.value), double(spin_y.value)});
}

ui2::impl::page::MovementServicePage::MovementServicePage() :
    Page("Move Service") {
    auto &p = Device::getInstance().planner;
    auto &c = p.getController();

    add(allocStatic(CheckBox(Text("Enable"), [&c](bool e) { c.setEnabled(e); })));
    add(allocStatic(Button(Text("set Home"), [&c]() { c.setCurrentPositionAsHome(); })));
    add(allocStatic(Button(Text("pull Out"), [&c]() { c.pullRopesOut(); })));
    add(allocStatic(Button(Text("pull In"), [&c]() { c.pullRopesIn(); })));

    // pos

    add(allocStatic(Named(Text("Target-X"), spin_x)));
    add(allocStatic(Named(Text("Target-Y"), spin_y)));
    add(allocStatic(Button(Text("Move"), moveToTarget)));

    // mode

    add(ModeButton(M::Position));
    add(ModeButton(M::Speed));
    add(ModeButton(M::Accel));

    // set

    static SbU8 speed_in(speed_settings, u8(p.getSpeed()), [&p](u8 s) { p.setSpeed(s); });
    add(allocStatic(Named(Text("Speed"), speed_in)));
    static SbU8 accel_in(accel_settings, u8(p.getAccel()), [&p](u8 s) { p.setAccel(s); });
    add(allocStatic(Named(Text("Accel"), accel_in)));

    // offsets

    static SbI8 spin_left(offsets_settings, 0, [&c](i8 o) {
        c.setLeftOffset(o);
        moveToTarget();
    });
    add(allocStatic(Named(Text("Left-Offset MM"), spin_left)));

    static SbI8 spin_right(offsets_settings, 0, [&c](i8 o) {
        c.setRightOffset(o);
        moveToTarget();
    });
    add(allocStatic(Named(Text("Right-Offset MM"), spin_right)));

}
