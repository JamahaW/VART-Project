#pragma once

#include <utility>

#include "ui2/abc/Widget.hpp"
#include "FS.h"
#include "functional"


namespace ui2 {
    namespace impl {
        struct FileWidget : abc::Widget {
            fs::File file;

            std::function<void(fs::File &)> on_open;

            explicit FileWidget(fs::File file, std::function<void(fs::File &)> on_open = nullptr) :
                file(std::move(file)), on_open(std::move(on_open)) {}

            void onEvent(Event event) override {
                if (on_open == nullptr) { return; }
                file.seek(0);
                if (event == Event::Click) { on_open(file); }
            }

        protected:
            void render(abc::Display &display) const override {
                display.print(file.name());
            }
        };
    }
}