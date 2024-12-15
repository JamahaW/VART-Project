#include <Arduino.h>

#include "hardware/MotorRegulator.hpp"

#include "ui/Window.hpp"
#include "ui/Factory.hpp"

#include "gfx_legacy/OLED.hpp"


const hardware::MotorRegulatorConfig config{
    .update_delta_ms = 10,
    .pos_kp = 0.5,
    .pos_ki = 0.5,
    .pos_max_abs_i = 1.0,
    .pwm_kp = 0.5,
    .delta_ticks_max = 10,
    .is_ready_ticks_deviation = 10,
    .ticks_in_mm = 1,
};

auto left_regulator = hardware::MotorRegulator::makeLeft(config);

auto right_regulator = hardware::MotorRegulator::makeRight(config);

gfx::OLED display;

auto &window = ui::Window::getInstance(display);

void motor_test_page(ui::Page *p, hardware::MotorRegulator& motor_regulator) {
    p->addItem(ui::spinbox(new int(0), 4, [&motor_regulator](ui::Widget * w){
        motor_regulator.motor.set(*(int*)(w->value));
    }, 255));

    p->addItem(ui::display(&motor_regulator.encoder.ticks, ui::ValueType::INT));
}

void buildUI(ui::Page &p) {
    p.addItem(ui::label("Motor test"));
    motor_test_page(p.addPage("Left"), left_regulator);
    motor_test_page(p.addPage("Right"), right_regulator);
}

void setup() {
    left_regulator.encoder.attach();
    right_regulator.encoder.attach();

    buildUI(window.main_page);
}

void loop() {
    window.update();
}