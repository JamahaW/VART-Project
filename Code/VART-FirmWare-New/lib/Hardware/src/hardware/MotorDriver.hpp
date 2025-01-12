#pragma once

#include <cstdint>


namespace pid {

    class MotorDriverL293 {
    private:
        const uint8_t dir_a;
        const uint8_t dir_b;

    public:
        explicit MotorDriverL293(uint8_t dir_a, uint8_t dir_b);

        /// установить ШИМ и направление
        void setPower(int32_t power) const;
    };
}