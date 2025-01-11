// PID automated tuning (Ziegler-Nichols/relay method) for Arduino and
// compatible boards Copyright (c) 2016-2020 jackw01 This code is distrubuted
// under the MIT License, see LICENSE for details
#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class PIDAutotuner {
 public:
  // Constants for Ziegler-Nichols tuning mode
  enum Mode : char { ZNModeBasicPID, ZNModeLessOvershoot, ZNModeNoOvershoot };

  PIDAutotuner();

  // Configure parameters for PID tuning
  // See README for more details -
  // https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
  // targetInputValue: the target value to tuneSpeed to
  // loopInterval: PID loop interval in microseconds - must match whatever the
  // PID loop being tuned runs at outputRange: min and max values of the output
  // that can be used to control the system (0, 255 for analogWrite) znMode:
  // Ziegler-Nichols tuning mode (znModeBasicPID, znModeLessOvershoot,
  // znModeNoOvershoot) tuningCycles: number of cycles that the tuning runs for
  // (optional, default is 10)
  void setTargetInputValue(float target);

  void setLoopInterval(long interval);

  void setOutputRange(float min_output, float max_output);

  void setZNMode(Mode zn);

  void setTuningCycles(int tuneCycles);

  // Must be called immediately before the tuning loop starts
  void startTuningLoop(unsigned long us);

  // Automatically tuneSpeed PID
  // This function must be run in a loop at the same speed as the PID loop being
  // tuned See README for more details -
  // https://github.com/jackw01/arduino-pid-autotuner/blob/master/README.md
  float tunePID(float input, unsigned long us);

  // Get results of most recent tuning
  float getKp() const;

  float getKi() const;

  float getKd() const;

  bool isFinished() const;  // Is the tuning finished?

  int getCycle() const;  // return tuning cycle

 private:
  float targetInputValue = 0;
  float loopInterval = 0;
  float minOutput, maxOutput;
  Mode znMode = ZNModeNoOvershoot;
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
