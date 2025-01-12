#include <Arduino.h>
#include <cstdint>
#include "vart/Devices.hpp"

#include "ui/Factory.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"


void goTo(vart::PositionController &controller, int16_t x, int16_t y) {
    controller.setTargetPosition(x, y);
    const uint32_t us = vart::position_controller.getUpdatePeriodUs();

    while (not controller.isReady()) {
        vart::position_controller.update();
        delayMicroseconds(us);
    }
}

#define makeTask(fn, stack_size) do {                                                       \
    static StackType_t stack[stack_size];                                                   \
    static StaticTask_t task_data;                                                          \
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, 1, stack, &task_data, 1);  \
} while(false)


static void buildUI(ui::Page &page) {

    static int value = 0;

    page.addItem(ui::button(
        "button", [](ui::Widget *) {
            value++;
        }
    ));

    page.addItem(ui::display(&value, ui::ValueType::Integer));
    page.addItem(ui::spinBox(&value, 5));
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

void setup() {
    makeTask(uiTask, 4096);

    analogWriteFrequency(30000);
    Serial.begin(115200);
}

void loop() {}

#pragma clang diagnostic pop