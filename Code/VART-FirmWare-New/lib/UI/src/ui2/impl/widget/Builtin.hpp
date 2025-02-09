#pragma once

#include <functional>
#include <utility>

#include "ui2/abc/Widget.hpp"


namespace ui2 {
    namespace abc { struct Page; }

    namespace impl {
        namespace widget {

            /// Тестовое поле
            struct Text : abc::Widget {
                const char *text;

                explicit Text(const char *text);

                void render(abc::Screen &screen) const override;
            };

            /// Кнопка
            struct Button : abc::Widget {
                const Text text;
                std::function<void()> on_click;

                explicit Button(Text text, std::function<void()> on_click = nullptr);

                void onEvent(Event event) override;

                void render(abc::Screen &screen) const override;
            };

            /// Кнопка для перехода на страницу
            struct PageSetterButton : Button {
                explicit PageSetterButton(abc::Page &page, const std::function<void(abc::Page &)> &on_entry);

                void render(abc::Screen &screen) const override;
            };

            /// Чек-бокс
            struct CheckBox : abc::Widget {
                const Text text;
                std::function<void(bool)> on_change;
                bool state;

                explicit CheckBox(Text text, std::function<void(bool)> on_change = nullptr, bool state = false);

                void onEvent(Event) override;

                void render(abc::Screen &screen) const override;

            private:
                void onClick() const;
            };

            /// Дисплей для отображения
            template<typename T> struct Display : abc::Widget {
                T &value;

                explicit Display(T &value) :
                    value(value) {}

                void render(abc::Screen &screen) const override {
                    screen.print(value);
                }
            };

            /// Спин-бокс
            template<typename T> struct SpinBox : abc::Widget {
                /// Параметры
                struct Settings {
                    T min, max, step;

                    T clamp(T v) const { return constrain(v, min, max); }
                };

                const Settings &settings;
                std::function<void(T)> on_change;
                T value;

                explicit SpinBox(const Settings &settings, T default_value = 0, std::function<void(T)> on_change = nullptr) :
                    settings(settings), value(default_value), on_change(on_change) {}

                void onEvent(Event event) override {
                    if (event == Event::StepUp) { value += settings.step; }
                    else if (event == Event::StepDown) { value -= settings.step; }
                    value = settings.clamp(value);
                    if (on_change != nullptr) { on_change(value); }
                }

                void render(abc::Screen &screen) const override {
                    screen.write('<');
                    screen.print(value);
                    screen.write('>');
                }
            };

            /// Прогресс-бар
            struct ProgressBar : abc::Widget {
                int &value;

                explicit ProgressBar(int &value);

                void render(abc::Screen &screen) const override;
            };

            /// Надстройка с наименованием
            struct Named : abc::Widget {
                const Text text;
                Widget &inner;

                explicit Named(Text text, Widget &inner);

                void render(abc::Screen &screen) const override;

                void onEvent(Event event) override;
            };
        }
    }
}
