#pragma once

#include <Stream.h>
#include <esp32-hal.h>
#include "bytelang/Reader.hpp"
#include "bytelang/primitives.hpp"


namespace bytelang {
    namespace abc {
        /// Исполнитель сценариев
        struct Interpreter {
            enum Result : primitive::u8 {
                /// Прекращение работы виртуальной машины
                ExitOk = 0x00,
                /// Продолжение работы виртуальной машины
                Ok = 0x01,
                /// Выключение пользователем
                Abort = 0x02,
                /// Байт сдвига начала программы был неверен
                InvalidHeader = 0x03,
                /// Неверный код инструкции
                InvalidInstructionCode = 0x04,
                /// Не удалось считать код инструкции
                InstructionCodeReadError = 0x05,
                /// Не удалось считать аргумент инструкции
                InstructionArgumentReadError = 0x06
            };

            /// Тип указателя на функцию-обработчик инструкции интерпретатора
            typedef Result (*Instruction)(Reader &);

            /// Исполнение сценария было прервано
            volatile bool is_aborted{false};
            /// Исполнение сценария было приостановлено
            volatile bool is_paused{false};
            /// Количество инструкций
            const primitive::u8 instruction_count;
            /// Таблица инструкций
            const Instruction *const instruction_table;

            explicit Interpreter(primitive::u8 instruction_count, Result (**instruction_table)(Reader &)) :
                instruction_count(instruction_count), instruction_table(instruction_table) {}

            /// Запустить исполнение сценария
            Result run(Stream &stream) {
                if (stream.read() != 0x01) { return Result::InvalidHeader; }

                primitive::u8 instruction_code;
                Result instruction_result;
                Reader reader(stream);

                is_aborted = false;
                is_paused = false;

                while (true) {
                    delay(1);
                    if (is_aborted) { return Result::Abort; }
                    if (is_paused) { continue; }
                    if (reader.read(instruction_code) == Reader::Result::Fail) { return Result::InstructionCodeReadError; }
                    if (instruction_code >= instruction_count) { return Result::InvalidInstructionCode; }

                    instruction_result = instruction_table[instruction_code](reader);
                    if (instruction_result != Result::Ok) { return instruction_result; }
                }
            }
        };
    }
}
