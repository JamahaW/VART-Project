//#include <Arduino.h>
//
//#include <hardware/MotorRegulator.hpp>
//
//
//hardware::MotorRegulator::MotorRegulator(
//    const MotorRegulatorConfig &state,
//    const Encoder &encoder,
//    const MotorDriverL293 &motor
//) :
//    config(state), encoder(encoder),
//    motor(motor) {
//}
//
//int hardware::MotorRegulator::calcUDelta() {
//    auto error = float(target_position_tick - encoder.current_position_ticks);
//
//    integral += error * config.pos_ki * config.update_delta_ms;
//    integral = constrain(integral, -config.pos_max_abs_i, config.pos_max_abs_i);
//
//    auto ret = int(error * config.pos_kp + integral);
//    return constrain(ret, -delta_tick, delta_tick);
//}
//
//int hardware::MotorRegulator::calcUDirPWM() const {
//    auto error = float(next_position_tick - encoder.current_position_ticks);
//    return int(error * config.pwm_kp);
//}
//
//void hardware::MotorRegulator::update() {
//    next_position_tick += calcUDelta();
//    motor.setPower(calcUDirPWM());
//}
//
//void hardware::MotorRegulator::setDelta(char new_delta) {
//    delta_tick = constrain(new_delta, 1, config.delta_ticks_max);
//}
//
//bool hardware::MotorRegulator::isReady() const {
////    return abs(target_position_tick - encoder.current_position_ticks) <= config.is_ready_ticks_deviation;
//
//}
//
//void hardware::MotorRegulator::reset() {
////    encoder.current_position_ticks = 0;
//    next_position_tick = 0;
//    integral = 0.0F;
//    target_position_tick = 0;
//    motor.setPower(0);
//    delta_tick = 0;
//}
//
//long hardware::MotorRegulator::calcTicks(int distance_mm) const {
//    return long(float(distance_mm ) * config.ticks_in_mm);
//}
//
//void hardware::MotorRegulator::setCurrent(int distance_mm) const {
////    next_position_tick = encoder.current_position_ticks = calcTicks(distance_mm);
//}
//
//void hardware::MotorRegulator::setTarget(int distance_mm) const {
//    target_position_tick = calcTicks(distance_mm);
//}
