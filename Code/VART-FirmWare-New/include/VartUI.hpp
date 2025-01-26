#pragma once
#define  FS_NO_GLOBALS

#include <SD.h>
#include <SPIFFS.h>
#include "vart/util/Pins.hpp"
#include "bytelang/test/MockStream.hpp"
#include "bytelang/core/MemIO.hpp"
#include "vart/util/Macro.hpp"

#include "ui2/impl/SpinBox.hpp"
#include "ui2/impl/Button.hpp"
#include "ui2/impl/CheckBox.hpp"
#include "ui2/impl/Display.hpp"
#include "ui2/impl/ProgressBar.hpp"


void workAreaPage(ui2::Page *p) {
    using SpinBoxShort = ui2::impl::SpinBox<short>;
    static const SpinBoxShort::Settings settings = {.min = 100, .max = 4000, .step = 25,};

    vart::PositionController &controller = vart::context.planner.getController();
    auto size = controller.getAreaSize();

    p->add(new SpinBoxShort("Width", settings, size.x, [&controller](short w) {
        controller.setAreaSize({double(w), controller.getAreaSize().y});
    }));
    p->add(new SpinBoxShort("Height", settings, size.y, [&controller](short h) {
        controller.setAreaSize({controller.getAreaSize().x, double(h)});
    }));
}

void printingPage(ui2::Page *p) {
    using ui2::impl::Button;
    using ui2::impl::CheckBox;
    using ui2::impl::ProgressBar;

    p->add(new Button("Stop!", []() { vart::interpreter.abort(); }));
    p->add(new CheckBox("Pause", [](bool p) { vart::interpreter.setPaused(p); }));
    p->add(new ProgressBar<int>(vart::context.progress));
}

void afterPrint(ui2::Page *p) {
    using ui2::impl::Button;
    using ui2::impl::Display;

    p->add(vart::window.root.to_this_page);
    p->add(new Display<int>("QUIT CODE", vart::context.quit_code));
}

void markerToolPage(ui2::Page *p) {
    using Angle = vart::MarkerPrintTool::Angle;
    using Marker = vart::MarkerPrintTool::Marker;

    using ui2::impl::CheckBox;
    using SpinBoxAngle = ui2::impl::SpinBox<Angle>;
    using SpinBoxU16 = ui2::impl::SpinBox<uint16_t>;

    auto &t = vart::context.tool;
    p->add(new CheckBox("Tool", [&t](bool e) { t.setEnabled(e); }));

    static const SpinBoxAngle::Settings s = {0, 180, 1};
#define MarkerTool(m) (__extension__( {static SpinBoxAngle __s(#m, s, t.getToolAngle(m), [&t](Angle a) {t.updateToolAngle(m, a);t.setActiveTool(m);}); &__s;} ))
    p->add(MarkerTool(Marker::None));
    p->add(MarkerTool(Marker::Left));
    p->add(MarkerTool(Marker::Right));

    static const SpinBoxU16::Settings duration_settings = {0, 1000, 50};
    static SpinBoxU16 duration_spin_box("Duration", duration_settings, t.getChangeDuration(), [&t](uint16_t d) { t.setChangeDuration(d); });
    p->add(&duration_spin_box);
}

static void servicePage(ui2::Page *p) {
    using ui2::impl::Button;
    using ui2::impl::CheckBox;

    auto &c = vart::context.planner.getController();
    p->add(new CheckBox("Regulator", [&c](bool e) { c.setEnabled(e); }));
    p->add(new Button("setHome", [&c]() { c.setCurrentPositionAsHome(); }));
    p->add(new Button("pullOut", [&c]() { c.pullRopesOut(); }));
    p->add(new Button("pullIn", [&c]() { c.pullRopesIn(); }));
}


static void movementPage(ui2::Page *p) {
    using M = vart::Planner::Mode;
    using SpinBoxI16 = ui2::impl::SpinBox<int16_t>;
    using SpinBoxU8 = ui2::impl::SpinBox<uint8_t>;
    using SpinBoxI8 = ui2::impl::SpinBox<int8_t>;
    using ui2::impl::Button;

    // pos

    static const SpinBoxI16::Settings position_settings = {.min = -600, .max = 600, .step = 50};

    static SpinBoxI16 target_x_spin_box("Target-X", position_settings, 0);
    static int16_t &target_x = target_x_spin_box.value;
    p->add(&target_x_spin_box);

    static SpinBoxI16 target_y_spin_box("Target-Y", position_settings, 0);
    static int16_t &target_y = target_y_spin_box.value;
    p->add(&target_y_spin_box);

    vart::Planner &planner = vart::context.planner;
    p->add(new Button("Move", [&planner]() {
        planner.moveTo({double(target_x), double(target_y)});
    }));

    // mode
#define ModeButton(mode) (__extension__( {static Button __b(#mode, [&planner](){planner.setMode(mode);}); &__b;} ))

    p->add(ModeButton(M::Position));
    p->add(ModeButton(M::Speed));
    p->add(ModeButton(M::Accel));

    // set

    static const SpinBoxU8::Settings speed_settings = {.min = 10, .max = 250, .step = 10};
    static SpinBoxU8 speed_spin_box("Speed", speed_settings, planner.getSpeed(), [&planner](uint8_t s) { planner.setSpeed(s); });
    p->add(&speed_spin_box);

    static const SpinBoxU8::Settings accel_settings = {.min = 5, .max = 250, .step = 5};
    static SpinBoxU8 accel_spin_box("Accel", accel_settings, planner.getAccel(), [&planner](uint8_t s) { planner.setAccel(s); });
    p->add(&accel_spin_box);

    // offsets

    auto &c = planner.getController();
    static const SpinBoxI8::Settings offsets_settings = {-100, 100, 5};
    static SpinBoxI8 left_offset_spin_box("Left-Offset MM", offsets_settings, 0, [&c](int8_t o) {
        c.setLeftOffset(o);
        c.setTargetPosition({double(target_x), double(target_y)});
    });
    p->add(&left_offset_spin_box);

    static SpinBoxI8 right_offset_spin_box("Right-Offset MM", offsets_settings, 0, [&c](int8_t o) {
        c.setRightOffset(o);
        c.setTargetPosition({double(target_x), double(target_y)});
    });
    p->add(&right_offset_spin_box);
}
