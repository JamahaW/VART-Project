#pragma once

#include "ui2/abc/Page.hpp"


namespace ui2 {
    namespace impl {
        namespace page {
            using abc::Page;

            /// Главная страница
            struct MainPage : Page {
                /// Получить экземпляр главной страницы
                static MainPage &getInstance();

                MainPage(const MainPage &) = delete;

                MainPage &operator=(const MainPage &) = delete;

            private:
                MainPage();
            };

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

            /// Страница выбора сценария для печати
            struct MediaPage : Page { MediaPage(); };
        }
    }
}