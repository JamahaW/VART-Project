//#pragma once
//
//#include <hardware/Encoder.hpp>
//#include <hardware/MotorDriver.hpp>
//
//namespace hardware
//{
//
//    struct MotorRegulatorConfig
//    {
//        /// временная дельта обновления регулятора
//        float update_delta_ms;
//
//        /// P-коэффициент регулятора delta_ticks по положению
//        float pos_kp;
//        /// I-коэффициент регулятора delta_ticks по положению
//        float pos_ki;
//        /// Максимальное значение I-составляющей по модулю
//        float pos_max_abs_i;
//
//        /// P-коэффициент регулятора ШИМ по delta_ticks
//        float pwm_kp;
//
//        /// максимальное значение delta_ticks
//        int delta_ticks_max;
//        /// максимальное отклонение, до которого регулятор считает, что он пришел к цели
//        int is_ready_ticks_deviation;
//
//        /// кол-во тиков в 1 мм
//        float ticks_in_mm;
//    };
//
//    /// Регулятор мотора
//    class MotorRegulator
//    {
//
//    public:
//        const MotorRegulatorConfig &config;
//
//    private:
//        mutable float integral{0.0F};
//
//        /// следующая позиция смещения
//        mutable int32_t next_position_tick{0};
//        /// Дельта смещения в тиках энкодера за dt
//        mutable int8_t delta_tick{0};
//
//    public:
//        /// Целевая позиция регулятора в тиках энкодера
//        mutable int32_t target_position_tick{0};
//
//        Encoder encoder;
//        MotorDriverL293 motor;
//
//        explicit MotorRegulator(const MotorRegulatorConfig &state, const Encoder &encoder, const MotorDriverL293 &motor);
//
//        /// Обновить состояние регулятора. Вызывать циклично
//        void update();
//
//        void setTarget(int distance_mm) const;
//
//        void setCurrent(int distance_mm) const;
//
//        void setDelta(char new_delta);
//
//        void reset();
//
//        /// Регулятор достиг цели
//        bool isReady() const;
//
//    private:
//        int calcUDelta();
//
//        int calcUDirPWM() const;
//
//        long calcTicks(int distance_mm) const;
//    };
//}
