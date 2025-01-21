#pragma once

#include <Arduino.h>


#define logMsg(msg) Serial.print(msg)

#define logFloat(x)   \
  logMsg(#x " = "); \
  Serial.println(x, 10)

#define logInt(x) \
    logMsg(#x " = "); \
    Serial.println(x);

#define logPid(pid)      \
  logMsg(#pid "\n");     \
  logFloat((pid).kp);      \
  logFloat((pid).ki);      \
  logFloat((pid).kd);      \
  logFloat((pid).abs_max_i)

#define logFunc(fn) logMsg(#fn " : Begin\n"); fn; logMsg(#fn " : End\n")

#define logFuncRet(fn, ret_log_policy) do { \
    logMsg(#fn " : Begin\n");                   \
    auto ret = fn;                              \
    logMsg(#fn " = ");                      \
    ret_log_policy(ret);\
} while (0)
