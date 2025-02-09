#pragma once

#define createStaticTask(fn, stack_size, priority) ({ \
    static StackType_t stack[stack_size];\
    static StaticTask_t task_data;\
    xTaskCreateStaticPinnedToCore(fn, #fn, stack_size, nullptr, priority, stack, &task_data, 1);\
})

#define allocStatic(__init__) ({  static auto _x = (__init__);  &_x ;})
