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

//
//void printingPage(ui::Page *p) {
//    p->addItem(ui::button("ABORT", [](ui::Widget *) { vart::interpreter.abort(); }));
//    p->addItem(ui::button("PAUSE", [](ui::Widget *) { vart::interpreter.setPaused(true); }));
//    p->addItem(ui::button("RESUME", [](ui::Widget *) { vart::interpreter.setPaused(false); }));
//
//    p->addItem(ui::label("Progress"));
//    p->addItem(ui::display(&vart::context.progress, ui::ValueType::Integer));
//}
//
//void afterPrint(ui::Page *p) {
//    p->addItem(ui::button("TO MAIN", [](ui::Widget *) { vart::window.setPage(&vart::window.main_page); }));
//    p->addItem(ui::label("QUIT_CODE"));
//    p->addItem(ui::display(&vart::context.quit_code, ui::ValueType::Integer));
//}
//
//void markerToolPage(ui::Page *p) {
//    using Marker = vart::MarkerPrintTool::Marker;
//#define makeAngleWidget(m)  {\
//    static int v = vart::context.tool.getToolAngle(m);\
//    p->addItem(ui::label(#m));\
//    p->addItem(ui::spinBox(&v, 2, [&tool](ui::Widget *) {tool.updateToolAngle(m, v);tool.setActiveTool(m);}, 180));\
//}
//
//    auto &tool = vart::context.tool;
//    static int d = int(tool.getChangeDuration());
//
//    p->addItem(ui::button("enable", [&tool](ui::Widget *) { tool.setEnabled(true); }));
//    p->addItem(ui::button("disable", [&tool](ui::Widget *) { tool.setEnabled(false); }));
//    makeAngleWidget(Marker::Left)
//    makeAngleWidget(Marker::Right)
//    makeAngleWidget(Marker::None)
//
//    p->addItem(ui::spinBox(&d, 50, [&tool](ui::Widget *) { tool.setChangeDuration(d); }, 10000));
//}
//
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
