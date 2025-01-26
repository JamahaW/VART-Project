#pragma once

#include <SD.h>
#include <SPIFFS.h>
#include "ui/Factory.hpp"
#include "ui/FileWidget.hpp"
#include "vart/util/Pins.hpp"
#include "bytelang/test/MockStream.hpp"
#include "bytelang/core/MemIO.hpp"
#include "vart/util/Macro.hpp"


void workAreaPage(ui::Page *p) {
    static int w;
    static int h;

    auto size = vart::context.planner.getController().getAreaSize();
    w = int(size.x);
    h = int(size.y);

    auto update = [](ui::Widget *) {
        vart::context.planner.getController().setAreaSize({double(w), double(h)});
    };

    p->addItem(ui::label("Width"));
    p->addItem(ui::spinBox(&w, 25, update, 4000, 100));
    p->addItem(ui::label("Height"));
    p->addItem(ui::spinBox(&h, 25, update, 4000, 100));
}

void printingPage(ui::Page *p) {
    p->addItem(ui::button("ABORT", [](ui::Widget *) { vart::interpreter.abort(); }));
    p->addItem(ui::button("PAUSE", [](ui::Widget *) { vart::interpreter.setPaused(true); }));
    p->addItem(ui::button("RESUME", [](ui::Widget *) { vart::interpreter.setPaused(false); }));

    p->addItem(ui::label("Progress"));
    p->addItem(ui::display(&vart::context.progress, ui::ValueType::Integer));
}

void afterPrint(ui::Page *p) {
    p->addItem(ui::button("TO MAIN", [](ui::Widget *) { vart::window.setPage(&vart::window.main_page); }));
    p->addItem(ui::label("QUIT_CODE"));
    p->addItem(ui::display(&vart::context.quit_code, ui::ValueType::Integer));
}

void markerToolPage(ui::Page *p) {
    using Marker = vart::MarkerPrintTool::Marker;
#define makeAngleWidget(m)  {\
    static int v = vart::context.tool.getToolAngle(m);\
    p->addItem(ui::label(#m));\
    p->addItem(ui::spinBox(&v, 2, [&tool](ui::Widget *) {tool.updateToolAngle(m, v);tool.setActiveTool(m);}, 180));\
}

    auto &tool = vart::context.tool;
    static int d = int(tool.getChangeDuration());

    p->addItem(ui::button("enable", [&tool](ui::Widget *) { tool.setEnabled(true); }));
    p->addItem(ui::button("disable", [&tool](ui::Widget *) { tool.setEnabled(false); }));
    makeAngleWidget(Marker::Left)
    makeAngleWidget(Marker::Right)
    makeAngleWidget(Marker::None)

    p->addItem(ui::spinBox(&d, 50, [&tool](ui::Widget *) { tool.setChangeDuration(d); }, 10000));
}

static void servicePage(ui::Page *p) {
    auto &controller = vart::context.planner.getController();
    p->addItem(ui::button("enable", [&controller](ui::Widget *) { controller.setEnabled(true); }));
    p->addItem(ui::button("disable", [&controller](ui::Widget *) { controller.setEnabled(false); }));
    p->addItem(ui::button("setHome", [&controller](ui::Widget *) { controller.setCurrentPositionAsHome(); }));
    p->addItem(ui::button("pullOut", [&controller](ui::Widget *) { controller.pullRopesOut(); }));
    p->addItem(ui::button("pullIn", [&controller](ui::Widget *) { controller.pullRopesIn(); }));
}

static void movementPage(ui::Page *p) {

    using M = vart::Planner::Mode;

    static int tx = 0, ty = 0;
#define addModeButton(page, mode)\
    page->addItem(ui::button( #mode, [](ui::Widget *) { vart::context.planner.setMode(mode); }))

    p->addItem(ui::button("Move", [](ui::Widget *) {
        vart::context.planner.moveTo((const vart::Vector2D) {double(tx), double(ty)});
    }));

    p->addItem(ui::label("Target"));
    p->addItem(ui::spinBox(&tx, 50, nullptr, 600, -600));
    p->addItem(ui::spinBox(&ty, 50, nullptr, 600, -600));

    addModeButton(p, M::Position);
    addModeButton(p, M::Speed);
    addModeButton(p, M::Accel);

    static int accel = int(vart::context.planner.getAccel());
    static int speed = int(vart::context.planner.getSpeed());

    void (*spin)(ui::Widget *) = [](ui::Widget *) {
        vart::context.planner.setSpeed(speed);
        vart::context.planner.setAccel(accel);
    };

    p->addItem(ui::label("Speed"));
    p->addItem(ui::spinBox(&speed, 25, spin, 1000, 0));
    p->addItem(ui::label("Accel"));
    p->addItem(ui::spinBox(&accel, 5, spin, 1000, 0));

    static int l, r;

    auto &controller = vart::context.planner.getController();

    auto on_spin = [&controller](ui::Widget *) {
        controller.setOffsets(l, r);
        controller.setTargetPosition({double(tx), double(ty)});
    };

    p->addItem(ui::label("Offset"));
    p->addItem(ui::spinBox(&l, 5, on_spin, 100, -100));
    p->addItem(ui::spinBox(&r, 5, on_spin, 100, -100));
}
