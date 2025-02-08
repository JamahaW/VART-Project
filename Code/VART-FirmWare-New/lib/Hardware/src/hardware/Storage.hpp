#pragma once

#include "EEPROM.h"


namespace hardware {

    /// Постоянное хранилище данных
    template<typename T> struct Storage {

        /// Инициализировать хранилище
        void init() { EEPROM.begin(sizeof(T)); }

        /// Записать
        void write(T &source) const { EEPROM.put(0, source); }

        /// Считать
        void read(T &destination) { EEPROM.get(0, destination); }
    };
}