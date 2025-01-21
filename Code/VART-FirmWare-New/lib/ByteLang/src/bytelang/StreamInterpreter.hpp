#pragma once

#include <Stream.h>
#include "Reader.hpp"
#include "primitives.hpp"


namespace bytelang {

    template<class Context> struct StreamInterpreter {

        enum Result : primitive::u8 {
            /// Прекращение работы виртуальной машины
            ExitOk = 0x00,

            /// Продолжение работы виртуальной машины
            Ok = 0x01,

            /// Выключение пользователем
            Abort = 0x02,

            /// Байт сдвига начала программы был неверен
            InvalidHeader = 0x10,

            /// Неверный код инструкции
            InvalidInstructionCode = 0x20,

            /// Не удалось считать код инструкции
            InstructionCodeReadError = 0x30,

            /// Не удалось считать аргумент инструкции
            InstructionArgumentReadError = 0x40
        };

        /// Тип указателя на функцию-обработчик инструкции интерпретатора
        typedef Result (*Instruction)(Reader &, Context &);

        /// Исполнение программы было прервано
        volatile bool is_aborted;

        /// Исполнение программы было приостановлено
        volatile bool is_paused;

        /// Количество инструкций
        const primitive::u8 instruction_count;

        /// таблица инструкций
        const Instruction instruction_table[];

        /// Запустить исполнение инструкций из потока
        Result run(Stream &stream, Context &context) {
            primitive::u8 instruction_code;
            Result instruction_result;
            Reader::Result reader_result;

            Reader reader(stream);

            if (stream.read() != 0x01) { return Result::InvalidHeader; }

            is_aborted = false;
            is_paused = false;

            while (true) {
                delay(1);

                if (is_aborted) { return Result::Abort; }
                if (is_paused) { continue; }

                reader_result = reader.read(instruction_code);

                if (reader_result == Reader::Result::Fail) { return Result::InstructionCodeReadError; }

                if (instruction_code > instruction_count) { return Result::InvalidInstructionCode; }

                instruction_result = instruction_table[instruction_code](reader, context);

                if (instruction_result != Result::Ok) { return instruction_result; }
            }
        }

        /// Прервать исполнение
        void abort() { is_aborted = true; }

        /// Приостановить исполнение
        void setPaused(bool paused) { is_paused = paused; }
    };


}
