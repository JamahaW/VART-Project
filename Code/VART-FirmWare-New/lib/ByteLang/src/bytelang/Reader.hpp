#pragma once

#include <Stream.h>
#include "primitives.hpp"


namespace bytelang {

    class Reader {

    private:

        Stream &stream;

    public:

        /// Результат чтения
        enum class Result : primitive::u8 {
            /// Успешное чтение данных
            Ok = 0x00,

            /// Не удалось считать данные
            Fail = 0x01
        };

        explicit Reader(Stream &stream) :
            stream(stream) {}

        template<typename T> Result read(T &buffer) {
            auto *buffer_pointer = (uint8_t *) (&buffer);

            if (sizeof(T) == 1) {
                int ret = stream.read();

                // Stream.h Magic Num
                if (ret == -1) { return Result::Fail; }

                *buffer_pointer = (ret & 0xFF);

                return Result::Ok;
            } else {
                stream.readBytes(buffer_pointer, sizeof(T));

                return Result::Ok;
            }
        }


    };
}