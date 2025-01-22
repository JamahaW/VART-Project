#pragma once

#include "ui/Widget.hpp"

#include <FS.h>


namespace ui {
    static char *c_str_copy(const char *s) {
        char *ret = new char[strlen(s) + 1];
        strcpy(ret, s);
        return ret;
    }

    class FileWidget : public Widget {

    public:
        explicit FileWidget(fs::File &file, std::function<void(Widget *)> onClick) :
            Widget(
                ValueType::Chars,
                (void *) c_str_copy(file.path()),
                std::move(onClick),
                nullptr
            ) {}

        ~FileWidget() {
            delete[] (char *) value;
        }
    };
}

