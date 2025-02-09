#include <Arduino.h>
#include <EncButton.h>

#include <SPI.h>

#include "misc/Macro.hpp"
#include "vart/Device.hpp"

#include "vart/util/Pins.hpp"
#include "ui2/Window.hpp"
#include "ui2/impl/page/VartPages.hpp"
#include "ui2/impl/screen/OledScreen.hpp"


using ui2::Event;
using ui2::Window;
using ui2::impl::screen::OledScreen;
using ui2::impl::page::MainPage;

using vart::Pins;

auto eb = EncButton(Pins::UserEncoderA, Pins::UserEncoderB, Pins::UserEncoderButton);

[[noreturn]] static void uiTaskI(void *) {
    auto &w = Window::getInstance();

    w.addEvent(Event::ForceUpdate);

    while (true) {
        eb.tick();
        if (eb.left()) { w.addEvent(Event::NextWidget); }
        if (eb.right()) { w.addEvent(Event::PreviousWidget); }
        if (eb.rightH()) { w.addEvent(Event::StepUp); }
        if (eb.leftH()) { w.addEvent(Event::StepDown); }
        if (eb.click()) { w.addEvent(Event::Click); }
        taskYIELD();
    }
}

[[noreturn]] static void uiTaskD(void *) {
    auto &display = OledScreen::getInstance();
    auto &w = Window::getInstance();
    display.oled.init();

    w.setScreen(display);
    w.setPage(MainPage::getInstance());

    while (true) {
        w.pull();
        taskYIELD();
    }
}

[[noreturn]] static void servoTask(void *) {
    auto &d = vart::Device::getInstance();
    auto &c = d.planner.getController();
    const auto update_period_ms = c.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        c.update();
        vTaskDelay(update_period_ms);
        taskYIELD();
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void setup() {
    createStaticTask(uiTaskD, 4096, 1);
    createStaticTask(uiTaskI, 4096, 1);
    createStaticTask(servoTask, 4096, 1);

    SPI.begin(Pins::SdClk, Pins::SdMiso, Pins::SdMosi, Pins::SdCs);
    Serial.begin(115200);
}

void loop() {}

#pragma clang diagnostic pop