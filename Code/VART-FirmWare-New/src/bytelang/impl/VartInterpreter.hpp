#pragma once

#include "bytelang/abc/Interpreter.hpp"


namespace bytelang {
    namespace impl {
        struct VartInterpreter : abc::Interpreter {
            using abc::Interpreter::Interpreter;

            static VartInterpreter &getInstance();
        };
    }
}