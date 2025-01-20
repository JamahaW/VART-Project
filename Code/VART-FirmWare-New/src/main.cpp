#include <Arduino.h>
#include "vart/Devices.hpp"
#include "vart/PositionController.hpp"
#include "vart/Pins.hpp"
#include "ui/Factory.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


using hardware::MotorDriverL293;
using hardware::ServoMotor;
using vart::Pins;
using vart::Pulley;

static auto left_encoder = hardware::Encoder(Pins::left_encoder_a, Pins::left_encoder_b);

//static auto right_encoder = hardware::Encoder(Pins::right_encoder_a, Pins::right_encoder_b);

static auto left_driver = MotorDriverL293(Pins::left_driver_a, Pins::left_driver_b);

//static auto right_driver = MotorDriverL293(Pins::right_driver_a, Pins::right_driver_b);

static ServoMotor::Settings servo_settings = {
    .update_period_seconds = 1 * 1e-3,
    .ready_max_abs_error = 10,
    .position = {
        .pid = {
            .kp = 10,
            .ki = 3,
            .kd = 0.2,
            .abs_max_i = 204
        },
        .tuner = {
            .mode = pid::TunerMode::no_overshoot,
            .cycles = 10,
        },
        .abs_max_out = 255
    },
};

static auto left_servo = ServoMotor(servo_settings, left_driver, left_encoder);

//static auto right_servo = ServoMotor(servo_settings, right_driver, right_encoder);

//static const Pulley::Settings pulley_settings = {
//    .ticks_in_mm = 5000.0 / 280.0,
//};
//static auto left_pulley = Pulley(pulley_settings, left_servo);
//
//static auto right_pulley = Pulley(pulley_settings, right_servo);
//using vart::PositionController;
//
//PositionController::Settings position_controller_settings = {
//    .max_area_width = 1200,
//    .max_area_height = 1200,
//};
//auto position_controller = PositionController(position_controller_settings, left_pulley, right_pulley);


static void buildUI(ui::Page &p) {
    static int target_position = 0;

    auto spin = [](ui::Widget *) {
        left_servo.setTargetPosition(target_position);
    };

    p.addItem(ui::spinBox(&target_position, 10, spin, 10000, -10000));
    p.addItem(ui::spinBox(&target_position, 50, spin, 10000, -10000));
    p.addItem(ui::spinBox(&target_position, 200, spin, 10000, -10000));

    p.addItem(ui::button("enable", [](ui::Widget *) {
        left_servo.setEnabled(true);
    }));

    p.addItem(ui::button("disable", [](ui::Widget *) {
        left_servo.setEnabled(true);
    }));

    p.addItem(ui::spinBoxF(&left_servo.settings.position.pid.kp, 0.5, 100, 0));
    p.addItem(ui::spinBoxF(&left_servo.settings.position.pid.ki, 0.1, 100, 0));
    p.addItem(ui::spinBoxF(&left_servo.settings.position.pid.kd, 0.02, 100, 0));
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
    const auto update_period_ms = left_servo.getUpdatePeriodMs();

    analogWriteFrequency(30000);

    while (true) {
        left_servo.update();
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