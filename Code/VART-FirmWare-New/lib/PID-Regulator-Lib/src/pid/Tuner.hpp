// PidSettings automated tuning (Ziegler-Nichols/relay method) for Arduino and
// compatible boards Copyright (c) 2016-2020 jackw01 This code is distrubuted
// under the MIT License, see LICENSE for details
#pragma once

namespace pid {

    static constexpr auto m_pi = 3.14159265358979323846;

    /// Constants for Ziegler-Nichols tuning mode
    enum TunerMode : char {
        /// Стандартный режим
        basic,

        /// Меньшее Перерегулирование
        less_overshoot,

        /// Перерегулирования нет
        no_overshoot
    };

    /// Тюнер коэффициентов регулятора
    class Tuner {
    public:

        Tuner();

        // Configure parameters for PidSettings tuning
        // See README for more details -
        // https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
        // targetInputValue: the target value to tuneSpeed to
        // loopInterval: PidSettings loop interval in microseconds - must match whatever the
        // PidSettings loop being tuned runs at outputRange: min and max values of the output
        // that can be used to control the system (0, 255 for analogWrite) znMode:
        // Ziegler-Nichols tuning mode (znModeBasicPID, znModeLessOvershoot,
        // znModeNoOvershoot) tuningCycles: number of cycles that the tuning runs for
        // (optional, default is 10)
        void setTargetInputValue(float target);

        void setLoopInterval(long interval);

        void setOutputRange(float min_output, float max_output);

        void setZNMode(TunerMode mode);

        void setTuningCycles(int new_cycles);

        int getCycle() const;  // return tuning cycle

        // Must be called immediately before the tuning loop starts
        void startTuningLoop(unsigned long us);

        // Automatically tuneSpeed PidSettings
        // This function must be run in a loop at the same speed as the PidSettings loop being
        // tuned See README for more details -
        // https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
        float tunePID(float input, unsigned long us);

        // Get results of most recent tuning
        float getKp() const;

        float getKi() const;

        float getKd() const;

        bool isFinished() const;  // Is the tuning finished?

    private:
        float targetInputValue = 0;
        float loopInterval = 0;
        float minOutput, maxOutput;
        TunerMode znMode = no_overshoot;
        int cycles = 10;

        // See startTuningLoop()
        int i;
        bool output;
        float outputValue;

        long microseconds, t1, t2, tHigh, tLow;

        float max, min;

        float p_average;
        float i_average;
        float d_average;

        float kp, ki, kd;
    };
}


