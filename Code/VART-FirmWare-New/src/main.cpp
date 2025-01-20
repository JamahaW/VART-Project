#include <Arduino.h>
#include "vart/Devices.hpp"

#include "vart/Planner.hpp"

#include "ui/Factory.hpp"

#include "vart/Macro.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

using vart::Vector2D;

#define logV(v) \
    logFloat(v.x);\
    logFloat(v.y)


static void goConstSpeed(Vector2D begin, Vector2D end, double mm_per_second) {
    auto &controller = vart::position_controller;

    auto path_len = end.distance(begin);

    if (path_len < 1) { return; }

    auto step_len = mm_per_second * 1e-3;  // mm_per_ms
    auto steps_total = path_len / step_len;

    for (int i = 0; i < steps_total; i++) {
        controller.setTargetPosition(Vector2D::interpolate(begin, end, i / steps_total));
        delay(1);
    }
}

void goConstAccel(vart::PositionController &controller, Vector2D start, Vector2D goal, double max_velocity, double acceleration, double delta_time = 0.001) {
    Vector2D position = start;
    Vector2D velocity = {0, 0};
    controller.setTargetPosition(position);

    while (position.distance(goal) > 0.001) {
        Vector2D to_goal = goal.sub(position);
        double distance_to_goal = to_goal.length();

        double braking_distance = (velocity.length() * velocity.length()) / (2.0 * acceleration);

        Vector2D desired_velocity{};
        if (distance_to_goal > braking_distance) {
            desired_velocity = to_goal.normalize().scale(max_velocity);
        } else {
            desired_velocity = to_goal.normalize().scale(std::max(max_velocity * (distance_to_goal / braking_distance), 0.5)); // Минимальная скорость
        }

        Vector2D needed_acceleration = (desired_velocity.sub(velocity)).scale(1.0 / delta_time);

        if (needed_acceleration.length() > acceleration) {
            needed_acceleration = needed_acceleration.normalize().scale(acceleration);
        }

        Vector2D new_velocity = velocity.add(needed_acceleration.scale(delta_time));
        Vector2D new_position = position.add(new_velocity.scale(delta_time));

        if (new_position.distance(goal) > position.distance(goal)) {
            new_velocity = {0, 0};
        }

        velocity = new_velocity;
        position = new_position;
        controller.setTargetPosition(position);

        delay(1);
    }
    controller.setTargetPosition(goal);
}


static void goTo(const Vector2D end, double speed, double accel) {
    goConstAccel(vart::position_controller, vart::position_controller.getCurrentPosition(), end, speed, accel);
}

static void buildUI(ui::Page &p) {
    static int tx = 0, ty = 0, speed = 100, accel = 25;

    p.addItem(ui::label("GoTo Target XY"));
    p.addItem(ui::spinBox(&tx, 50, nullptr, 600, -600));
    p.addItem(ui::spinBox(&ty, 50, nullptr, 600, -600));
    p.addItem(ui::spinBox(&speed, 25, nullptr, 1000, 0));
    p.addItem(ui::spinBox(&accel, 5, nullptr, 1000, 0));

    p.addItem(ui::button("GoTo", [](ui::Widget *) {
        goTo({double(tx), double(ty)}, speed, accel);
    }));

    static int size = 200;
    p.addItem(ui::spinBox(&size, 25, nullptr, 500, 0));

    p.addItem(ui::button("DemoRect", [](ui::Widget *) {
        const auto s = double(size);
        goTo({s, s}, speed, accel);
        goTo({-s, s}, speed, accel);
        goTo({-s, -s}, speed, accel);
        goTo({s, -s}, speed, accel);
        goTo({s, s}, speed, accel);
        goTo({0, 0}, speed, accel);
    }));

    p.addItem(ui::button("DemoCircle", [](ui::Widget *) {
        const auto r = double(size);
        double x, y;

        for (int deg = 0; deg < 360; deg += 5) {
            x = r * std::cos(radians(deg));
            y = r * std::sin(radians(deg));
            goTo({x, y}, speed, accel);
        }

        goTo({0, 0}, speed, accel);

    }));
    p.addItem(ui::button("enable", [](ui::Widget *) {
        vart::position_controller.setEnabled(true);
    }));
    p.addItem(ui::button("pullOut", [](ui::Widget *) {
        vart::position_controller.pullRopesOut();
    }));
    p.addItem(ui::button("pullIn", [](ui::Widget *) {
        vart::position_controller.pullRopesIn();
    }));
    p.addItem(ui::button("setHome", [](ui::Widget *) {
        vart::position_controller.setCurrentPositionAsHome();
    }));
    p.addItem(ui::button("disable", [](ui::Widget *) {
        vart::position_controller.setEnabled(false);
    }));
}

[[noreturn]] static void uiTask(void *) {
    vart::window.display.init();

    ui::Page &page = vart::window.main_page;
    buildUI(page);

    while (true) {
        vart::window.update();
        vTaskDelay(1);
        taskYIELD();
    }
}

[[noreturn]] static void posTask(void *) {
    const auto update_period_ms = vart::position_controller.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        vart::position_controller.update();
        vTaskDelay(update_period_ms);
        taskYIELD();
    }
}

#define createStaticTask(fn, stack_size, priority) { \
    static StackType_t stack[stack_size];\
    static StaticTask_t task_data;\
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, priority, stack, &task_data, 1);\
}

void setup() {
    Serial.begin(115200);
    createStaticTask(uiTask, 4096, 1)
    createStaticTask(posTask, 4096, 1)
}

void loop() {}

#pragma clang diagnostic pop