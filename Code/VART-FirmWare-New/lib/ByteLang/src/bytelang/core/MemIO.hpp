#pragma once

#include "Arduino.h"


namespace bytelang {
    namespace core {

        /// Обёртка для IO операций над памятью
        struct MemIO {
            /// Начало
            const uint8_t *begin;

            /// Конец
            const uint8_t *end;

            /// текущее положение
            uint8_t *cursor;


            MemIO(void *buffer, size_t length) :
                begin(static_cast<const uint8_t *>(buffer)),
                end(static_cast<const uint8_t *>(buffer) + length),
                cursor(static_cast<uint8_t *>(buffer)) {}

            /// Узнать размер области
            inline size_t getAllocSize() const { return end - begin; }

            /// Узнать доступный размер
            inline size_t getAvailableSize() const { return end - cursor; }

            /// Получить текущий байт
            inline uint8_t peek() const { return *cursor; }

            /// Поток доступен для операций
            inline bool isAvailable() const { return cursor < end; }

            /// Считать и получить следующий байт
            uint8_t read() {
                return isAvailable() ? *cursor++ : 0;
            }

            /// Записать байт
            void write(uint8_t value) {
                if (isAvailable()) {
                    *cursor++ = value;
                }
            }

            void reset() { cursor = (uint8_t *) (begin); }
        };
    }
}