#pragma once

#include "ui2/abc/Page.hpp"


namespace ui2 {
    namespace impl {
        namespace page {
            using abc::Page;

            /// Страница меню рабочей области
            struct WorkAreaPage : Page { WorkAreaPage(); };

            /// Страница меню печати
            struct PrintingPage : Page { PrintingPage(); };

            /// Страница завершенной печати
            struct WorkOverPage : Page { WorkOverPage(); };

            /// Страница настройки инструмента печати
            struct ToolServicePage : Page { ToolServicePage(); };

            /// Страница настройки системы перемещений
            struct MovementServicePage : Page { MovementServicePage(); };
        }
    }
}