#include <utility>
#include <Arduino.h>

#include <ui/Factory.hpp>


ui::Widget *ui::button(const char *title, std::function<void(Widget *)> callback) {
    return (new Widget(
        ValueType::Chars,
        (void *) title,
        std::move(callback))
    )->setStyle(ui::Style::square_framed);
}

ui::Widget *ui::display(void *value, ValueType type) {
    return new Widget(type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ValueType::Chars);
}

ui::Widget *ui::spinBox(int *value, int step, std::function<void(Widget *)> on_spin, int max, int min) {
    return (new Widget(
        ValueType::Integer,
        value,
        std::move(on_spin),
        [step, max, min](Widget *w, int c) {
            *(int *) w->value = constrain(*(int *) w->value + c * step, min, max);
            w->onClick();
        }
    )
    )->setStyle(ui::Style::triangle_framed);
}

ui::Widget *ui::spinBoxF(float *value, float step, float max, float min, std::function<void(Widget *)> on_spin) {
    return (new Widget(
        ValueType::Float,
        value,
        std::move(on_spin),
        [step, max, min](Widget *w, int c) {
            *(float *) w->value = constrain(*(float *) w->value + c * step, min, max);
            w->onClick();
        }
    ))->setStyle(ui::Style::triangle_framed);
}

ui::Widget *ui::checkBoxRaw(const char *on_true, const char *on_false, const std::function<void(bool)> &on_change, bool default_value) {
    return new Widget(
        ui::ValueType::Chars,
        (void *) (default_value ? on_true : on_false),
        [on_false, on_true, &on_change](ui::Widget *w) mutable {
            bool is_true = w->value == on_true;

            if (on_change != nullptr) {
                on_change(is_true);
            }

            w->value = const_cast<char *>(is_true ? on_false : on_true);
        }
    );
}


