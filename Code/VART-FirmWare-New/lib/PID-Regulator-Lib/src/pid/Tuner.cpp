// PidSettings automated tuning (Ziegler-Nichols/relay method) for Arduino and compatible boards
// Copyright (c) 2016-2020 jackw01
// This code is distrubuted under the MIT License, see LICENSE for details

#include "Tuner.hpp"


pid::Tuner::Tuner() = default;

/// Set target input for tuning
void pid::Tuner::setTargetInputValue(float target) {
    targetInputValue = target;
}

/// Set loop interval
void pid::Tuner::setLoopInterval(long interval) {
    loopInterval = float(interval);
}

/// Set output range
void pid::Tuner::setOutputRange(float min_output, float max_output) {
    minOutput = min_output;
    maxOutput = max_output;
}

/// Set Ziegler-Nichols tuning mode
void pid::Tuner::setZNMode(TunerMode mode) {
    znMode = mode;
}

/// Set tuning new_cycles
void pid::Tuner::setTuningCycles(int new_cycles) {
    this->cycles = new_cycles;
}

/// Initialize all variables before loop
void pid::Tuner::startTuningLoop(unsigned long us) {
    i = 0; // Cycle counter
    output = true; // Current output state
    outputValue = maxOutput;
    t1 = t2 = us; // Times used for calculating period
    microseconds = tHigh = tLow = 0; // More time variables
    max = -100000000000.0F; // Max input
    min = 100000000000.0F; // Min input
    p_average = i_average = d_average = 0;
}

/// Run one cycle of the loop
float pid::Tuner::tunePID(float input, unsigned long us) {
    // Useful information on the algorithm used (Ziegler-Nichols method/Relay method)
    // http://www.processcontrolstuff.net/wp-content/uploads/2015/02/relay_autot-2.pdf
    // https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method
    // https://www.cds.caltech.edu/~murray/courses/cds101/fa04/caltech/am04_ch8-3nov04.pdf

    // Basic explanation of how this works:
    //  * Turn on the output of the PID controller to full power
    //  * Wait for the output of the system being tuned to reach the target input value
    //      and then turn the controller output off
    //  * Wait for the output of the system being tuned to decrease below the target input
    //      value and turn the controller output back on
    //  * Do this a lot
    //  * Calculate the ultimate gain using the amplitude of the controller output and
    //      system output
    //  * Use this and the period of oscillation to calculate PID gains using the
    //      Ziegler-Nichols method

    // Calculate time delta
    //long prevMicroseconds = microseconds;
    microseconds = us;
    //float deltaT = microseconds - prevMicroseconds;

    // Calculate max and min
    max = (max > input) ? max : input;
    min = (min < input) ? min : input;

    // Output is on and input signal has risen to target
    if (output && input > targetInputValue) {
        // Turn output off, record current time as t1, calculate tHigh, and reset maximum
        output = false;
        outputValue = minOutput;
        t1 = us;
        tHigh = t1 - t2;
        max = targetInputValue;
    }

    // Output is off and input signal has dropped to target
    if (!output && input < targetInputValue) {
        // Turn output on, record current time as t2, calculate tLow
        output = true;
        outputValue = maxOutput;
        t2 = us;
        tLow = t2 - t1;

        // Calculate Ku (ultimate gain)
        // Formula given is Ku = 4d / πa
        // d is the amplitude of the output signal
        // A is the amplitude of the input signal
        float ku = (4.0 * ((maxOutput - minOutput) / 2.0)) / (m_pi * (max - min) / 2.0);

        // Calculate Tu (period of output oscillations)
        auto tu = float(tLow + tHigh);

        // How gains are calculated
        // PidSettings control algorithm needs Kp, Ki, and Kd
        // Ziegler-Nichols tuning method gives Kp, Ti, and Td
        //
        // Kp = 0.6Ku = Kc
        // Ti = 0.5Tu = Kc/Ki
        // Td = 0.125Tu = Kd/Kc
        //
        // Solving these equations for Kp, Ki, and Kd gives this:
        //
        // Kp = 0.6Ku
        // Ki = Kp / (0.5Tu)
        // Kd = 0.125 * Kp * Tu

        // Constants
        // https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method

        float kpConstant, tiConstant, tdConstant;
        if (znMode == basic) {
            kpConstant = 0.6;
            tiConstant = 0.5;
            tdConstant = 0.125;
        } else if (znMode == less_overshoot) {
            kpConstant = 0.33;
            tiConstant = 0.5;
            tdConstant = 0.33;
        } else { // Default to No Overshoot mode as it is the safest
            kpConstant = 0.2;
            tiConstant = 0.5;
            tdConstant = 0.33;
        }

        // Calculate gains
        kp = kpConstant * ku;
        ki = (kp / (tiConstant * tu)) * loopInterval;
        kd = (tdConstant * kp * tu) / loopInterval;

        // Average all gains after the first two cycles
        if (i > 1) {
            p_average += kp;
            i_average += ki;
            d_average += kd;
        }

        // Reset minimum
        min = targetInputValue;

        // Increment cycle count
        i++;
    }

    // If loop is done, disable output and calculate averages
    if (i >= cycles) {
        output = false;
        outputValue = minOutput;
        auto i_1 = float(i - 2);
        kp = p_average / i_1;
        ki = i_average / i_1;
        kd = d_average / i_1;
    }

    return outputValue;
}

/// Get PidSettings constants after tuning
float pid::Tuner::getKp() const { return kp; }

float pid::Tuner::getKi() const { return ki; }

float pid::Tuner::getKd() const { return kd; }

/// Is the tuning loop finished?
bool pid::Tuner::isFinished() const {
    return (i >= cycles);
}

/// return number of tuning cycle
int pid::Tuner::getCycle() const {
    return i;
}