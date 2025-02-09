#include "bytelang/primitive.hpp"
#include "ui2/impl/Page.hpp"
#include "ui2/impl/widget/Button.hpp"
#include "ui2/impl/widget/CheckBox.hpp"
#include "ui2/impl/widget/NamedSpinBox.hpp"
#include "vart/Device.hpp"


using bytelang::primitive::u8;
using bytelang::primitive::i8;
using bytelang::primitive::i16;

using ui2::impl::widget::Button;
using ui2::impl::widget::CheckBox;
using ui2::impl::widget::Button;

using SbI16 = ui2::impl::widget::NamedSpinBox<i16>;
using SbU8 = ui2::impl::widget::NamedSpinBox<u8>;
using SbI8 = ui2::impl::widget::NamedSpinBox<i8>;

using M = vart::Planner::Mode;
using vart::Device;

static constexpr const SbI16::In::Settings position_settings = {.min = -600, .max = 600, .step = 50};

static constexpr const SbU8::In::Settings speed_settings = {.min = 10, .max = 250, .step = 10};

static constexpr const SbU8::In::Settings accel_settings = {.min = 5, .max = 250, .step = 5};

static constexpr const SbI8::In::Settings offsets_settings = {-100, 100, 5};

#define ModeButton(mode) (__extension__( {static Button __b(#mode, [&p](){p.setMode(mode);}); &__b;} ))


ui2::impl::page::MovementServicePage::MovementServicePage() :
    Page("Move Service") {
    auto &p = Device::getInstance().planner;
    auto &c = p.getController();

    add(new CheckBox("Enable", [&c](bool e) { c.setEnabled(e); }));
    add(new Button("set Home", [&c]() { c.setCurrentPositionAsHome(); }));
    add(new Button("pull Out", [&c]() { c.pullRopesOut(); }));
    add(new Button("pull In", [&c]() { c.pullRopesIn(); }));

    // pos
    static SbI16 target_x_spin_box("Target-X", SbI16::In(position_settings, 0));
    static i16 &target_x = target_x_spin_box.spin_box.value;
    add(&target_x_spin_box);

    static SbI16 target_y_spin_box("Target-Y", SbI16::In(position_settings, 0));
    static i16 &target_y = target_y_spin_box.spin_box.value;
    add(&target_y_spin_box);

    add(new Button("Move", [&p]() {
        p.moveTo({double(target_x), double(target_y)});
    }));

    // mode
    add(ModeButton(M::Position));
    add(ModeButton(M::Speed));
    add(ModeButton(M::Accel));

    // set
    add(new SbU8("Speed", SbU8::In(speed_settings, p.getSpeed(), [&p](u8 s) { p.setSpeed(s); })));
    add(new SbU8("Accel", SbU8::In(accel_settings, p.getAccel(), [&p](u8 s) { p.setAccel(s); })));

    // offsets
    static SbI8 left_offset_spin_box("Left-Offset MM", SbI8::In(offsets_settings, 0, [&c](i8 o) {
        c.setLeftOffset(o);
        c.setTargetPosition({double(target_x), double(target_y)});
    }));
    add(&left_offset_spin_box);

    static SbI8 right_offset_spin_box("Right-Offset MM", SbI8::In(offsets_settings, 0, [&c](i8 o) {
        c.setRightOffset(o);
        c.setTargetPosition({double(target_x), double(target_y)});
    }));
    add(&right_offset_spin_box);
}
