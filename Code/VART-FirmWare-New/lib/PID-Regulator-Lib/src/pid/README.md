# arduino-pos_to_pwm_pid-autotuner
Automated PidSettings tuning using Ziegler-Nichols/relay method.

Originally written for Arduino and compatible boards, but does not rely on the Arduino standard library.

## Disclaimer
**Issues have been disabled on this repository due to too many off-topic questions about PidSettings control in general or how to use this code. This project is a simple implementation of the algorithm described [here](https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method) and is not guaranteed to work in every use case. If you don't know what this code is intended to do, you probably don't need to use it.**

## How does it work?
`pidautotuner.h` and `pidautotuner.cpp` are fully commented to explain how the algorithm works.

## What PidSettings controller does this work with?
This algorithm should work with any implementation of PidSettings control if it is properly configured.

## Example code (Arduino)
```c
#include <pidautotuner.h>

void setup() {

    Tuner tuner = Tuner();

    // Set the target value to tuneSpeed to
    // This will depend on what you are tuning. This should be set to a value within
    // the usual range of the setpoint. For low-inertia systems, values at the lower
    // end of this range usually give better results. For anything else, start with a
    // value at the middle of the range.
    tuner.setTargetInputValue(targetInputValue);

    // Set the loop interval in microseconds
    // This must be the same as the interval the PidSettings control loop will run at
    tuner.setLoopInterval(loopInterval);

    // Set the output range
    // These are the minimum and maximum possible output values of whatever you are
    // using to control the system (Arduino analogWrite, for example, is 0-255)
    tuner.setOutputRange(0, 255);

    // Set the Ziegler-Nichols tuning mode
    // Set it to either Tuner::basic, Tuner::less_overshoot,
    // or Tuner::no_overshoot. Defaults to no_overshoot as it is the
    // safest option.
    tuner.setZNMode(Tuner::basic);

    // This must be called immediately before the tuning loop
    // Must be called with the current time in microseconds
    tuner.startTuningLoop(micros());

    // Run a loop until tuner.isFinished() returns true
    long microseconds;
    while (!tuner.isFinished()) {

        // This loop must run at the same delta_position as the PidSettings control loop being tuned
        long prevMicroseconds = microseconds;
        microseconds = micros();

        // Get input value here (temperature, encoder position, velocity, etc)
        double input = doSomethingToGetInput();

        // Call tune() with the input value and current time in microseconds
        double output = tuner.tune(input, microseconds);

        // Set the output - tunePid() will return values within the range configured
        // by setOutputRange(). Don't change the value or the tuning results will be
        // incorrect.
        doSomethingToSetOutput(output);

        // This loop must run at the same delta_position as the PidSettings control loop being tuned
        while (micros() - microseconds < loopInterval) delayMicroseconds(1);
    }

    // Turn the output off here.
    doSomethingToSetOutput(0);

    // Get PidSettings gains - set your PidSettings controller's gains to these
    double kp = tuner.getKp();
    double ki = tuner.getKi();
    double kd = tuner.getKd();
}

void loop() {

    // ...
}
```
