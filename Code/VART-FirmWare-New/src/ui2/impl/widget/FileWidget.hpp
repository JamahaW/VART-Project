#pragma once

#include <FS.h>
#include <functional>

#include "ui2/abc/Widget.hpp"


namespace ui2 {
    namespace impl {
        namespace widget {
            struct FileWidget : abc::Widget {

                FS &file_sys;
                const String path;
                std::function<void(const File &)> on_open;

                explicit FileWidget(
                    FS &file_sys,
                    const File &file,
                    std::function<void(const File &)> on_open = nullptr
                ) :
                    file_sys(file_sys),
                    path(file.path()),
                    on_open(std::move(on_open)) {}

                void onEvent(Event event) override {
                    if (on_open == nullptr) { return; }
                    if (event == Event::Click) { on_open(file_sys.open(path)); }
                }

            protected:
                void render(abc::Display &display) const override {
                    display.print(path);
                }
            };
        }
    }
}