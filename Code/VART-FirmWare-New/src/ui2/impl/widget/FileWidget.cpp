#include "VartWidgets.hpp"


using ui2::impl::widget::FileWidget;

FileWidget::FileWidget(FS &file_sys, const File &file, std::function<void(const File &)> on_open) :
    file_sys(file_sys),
    path(file.path()),
    on_open(std::move(on_open)) {}

void FileWidget::render(abc::Screen &display) const {
    display.print(path);
}

void FileWidget::onEvent(Event event) {
    if (on_open == nullptr) { return; }
    if (event == Event::Click) { on_open(file_sys.open(path)); }
}
