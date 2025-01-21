#pragma once

#include "Stream.h"

#include "bytelang/core/MemIO.hpp"


namespace bytelang {
    namespace test {

        /// Реализация-Затычка Stream для тестирования
        class MockStream : public Stream {

        public:

            /// Поток ввода
            core::MemIO input;

            explicit MockStream(core::MemIO &&input) :
                input(input) {}

            /// Получить количество доступных для чтения байт
            int available() override { return int(input.getAvailableSize()); }

            /// Считать байт (-1 если не удалось)
            int read() override { return input.isAvailable() ? input.read() : -1; }

            /// Считать верхний байт (-1 если не удалось)
            int peek() override { return input.peek(); }

            /// Отправить байт
            size_t write(uint8_t value) override { return 0; }

            /// Сколько ещё можно записать байт
            int availableForWrite() override { return 0; }

            void reset() { input.reset(); }
        };
    }
}