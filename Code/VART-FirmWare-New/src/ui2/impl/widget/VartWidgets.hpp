#pragma once

#include <FS.h>
#include <functional>

#include "ui2/abc/Widget.hpp"


namespace ui2 {
    namespace impl {
        namespace widget {
            /// Виджет вызывающий открытие файла
            struct FileWidget : abc::Widget {
                FS &file_sys;
                const String path;
                std::function<void(const File &)> on_open;

                explicit FileWidget(
                    FS &file_sys,
                    const File &file,
                    std::function<void(const File &)> on_open = nullptr
                );

                void onEvent(Event event) override;

                void render(abc::Screen &display) const override;
            };
        }
    }
}