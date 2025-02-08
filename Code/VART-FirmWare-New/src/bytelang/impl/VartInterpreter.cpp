#include "bytelang/impl/VartInterpreter.hpp"
#include "ui2/Window.hpp"
#include "vart/Device.hpp"


using namespace bytelang::primitive;
using bytelang::Reader;
using bytelang::impl::VartInterpreter;
using Result = VartInterpreter::Result;

/// Считанный результат является ошибкой
static inline bool isFail(Reader::Result result) { return result == Reader::Result::Fail; }

//  0: [1B] vart::quit@0()
static Result quit(Reader &) { return Result::ExitOk; }

//  1: [3B] vart::delay_ms@1(std::u16)
static Result delay_ms(Reader &reader) {
    u16 duration;
    if (isFail(reader.read(duration))) { return Result::InstructionArgumentReadError; }

    delay(duration);

    return Result::Ok;
}

//  2: [2B] vart::set_speed@2(std::u8)
static Result set_speed(Reader &reader) {
    u8 speed_set;
    if (isFail(reader.read(speed_set))) { return Result::InstructionArgumentReadError; }

    vart::Device::getInstance().planner.setSpeed(speed_set);

    return Result::Ok;
}

//  3: [2B] vart::set_accel@3(std::u8)
static Result set_accel(Reader &reader) {
    u8 accel_set;
    if (isFail(reader.read(accel_set))) { return Result::InstructionArgumentReadError; }

    vart::Device::getInstance().planner.setAccel(accel_set);

    return Result::Ok;
}

//  4: [2B] vart::set_planner_mode@4(std::u8)
static Result set_planner_mode(Reader &reader) {
    u8 mode;
    if (isFail(reader.read(mode))) { return Result::InstructionArgumentReadError; }

    vart::Device::getInstance().planner.setMode(static_cast<vart::Planner::Mode>(mode));

    return Result::Ok;
}

//  5: [5B] vart::set_position@5(std::i16, std::i16)
static Result set_position(Reader &reader) {
    i16 x, y;
    if (isFail(reader.read(x))) { return Result::InstructionArgumentReadError; }
    if (isFail(reader.read(y))) { return Result::InstructionArgumentReadError; }

    vart::Device::getInstance().planner.moveTo({double(x), double(y)});

    return Result::Ok;
}

//  6: [2B] vart::set_progress@6(std::u8)
static Result set_progress(Reader &reader) {
    u8 progress;
    if (isFail(reader.read(progress))) { return Result::InstructionArgumentReadError; }

    vart::Device::getInstance().context.progress = progress;
    ui2::Window::getInstance().addEvent(ui2::Event::ForceUpdate);

    return Result::Ok;
}

//  7: [2B] vart::set_active_tool@7(std::u8)
static Result set_active_tool(Reader &reader) {
    u8 tool_id;
    if (isFail(reader.read(tool_id))) { return Result::InstructionArgumentReadError; }

    auto &tool = vart::Device::getInstance().tool;
    tool.setActiveTool(static_cast<vart::MarkerPrintTool::Marker>(tool_id));

    while (not tool.servo.isReady()) { delay(10); }

    return Result::Ok;
}

VartInterpreter &VartInterpreter::getInstance() {
    static VartInterpreter::Instruction instructions[] = {
        quit,
        delay_ms,
        set_speed,
        set_accel,
        set_planner_mode,
        set_position,
        set_progress,
        set_active_tool
    };
    static VartInterpreter instance(8, instructions);
    return instance;
}
