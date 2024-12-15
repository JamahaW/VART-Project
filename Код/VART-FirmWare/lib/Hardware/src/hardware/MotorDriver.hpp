#pragma once

#include <cstdint>


namespace hardware {

    class MotorDriverL293 {
    private:
        const uint8_t DIR_A;
        const uint8_t DIR_B;

    public:
        explicit MotorDriverL293(uint8_t dir_a, uint8_t dir_b);

        static MotorDriverL293 makeLeft();

        static MotorDriverL293 makeRight();

        /// установить ШИМ и направление
        void set(int32_t pwm_dir) const;
    };
}