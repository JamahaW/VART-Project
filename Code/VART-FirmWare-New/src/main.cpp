#include <Arduino.h>
#include "vart/Devices.hpp"


#include "ui/Factory.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


using vart::Vector2D;

static void buildUI(ui::Page &p) {
    static int x = 0;
    static int y = 0;

    auto spin = [](ui::Widget *) {
        vart::position_controller.setTargetPosition({int16_t(x), int16_t(y)});
    };

    p.addItem(ui::spinBox(&x, 5, spin, 10000, -10000));
    p.addItem(ui::spinBox(&y, 5, spin, 10000, -10000));

    p.addItem(ui::button("set Home", [](ui::Widget *) {
        vart::position_controller.setCurrentPositionAsHome();
    }));

    p.addItem(ui::button("enable", [](ui::Widget *) {
        vart::position_controller.setEnabled(true);

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