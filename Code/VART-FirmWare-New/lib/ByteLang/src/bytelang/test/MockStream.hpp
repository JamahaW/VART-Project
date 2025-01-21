#pragma once

#include "Stream.h"

#include "bytelang/core/MemIO.hpp"


namespace serialcmd {
    namespace test {

        /// Реализация-Затычка Stream для тестирования
        class MockStream : public Stream {

        public:

            /// Поток ввода
            core::MemIO input;

            /// Поток вывода
            core::MemIO output;

            explicit MockStream(core::MemIO &&input, core::MemIO &&output) :
                input(input), output(output) {}

            /// Получить количество доступных для чтения байт
            int available() override { return this->input.getAvailableSize(); }

            /// Считать байт (-1 если не удалось)
            int read() override { return input.isAvailable() ? input.read() : -1; }

            /// Считать верхний байт (-1 если не удалось)
            int peek() override { return input.peek(); }

            /// Отправить байт
            size_t write(uint8_t value) override {
                output.write(value);
                return 1;
            }

            /// Сколько ещё можно записать байт
            int availableForWrite() override { return output.getAvailableSize(); }
        };
    }
}