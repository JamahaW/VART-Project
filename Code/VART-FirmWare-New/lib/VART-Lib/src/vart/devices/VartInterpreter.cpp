#include "vart/Devices.hpp"

#include "vart/util/Macro.hpp"


using VartResult = vart::VartIntepreter::Result;
using bytelang::Reader;
using vart::VartContext;

using namespace bytelang::primitive;

/// Считанный результат является ошибкой
static inline bool isFail(Reader::Result result) { return result == Reader::Result::Fail; }

constexpr static inline VartResult InstructionArgumentReadErrorAt(u8 at) {
    return static_cast<VartResult>(VartResult::InstructionArgumentReadError | (0x0F & at));
}

//static VartResult foo(Reader &, VartContext &) { return VartResult::Ok; }

//  0: [1B] vart::quit@0()
static VartResult quit(Reader &, VartContext &) {
    return VartResult::ExitOk;
}

//  1: [3B] vart::delay_ms@1(std::u16)
static VartResult delay_ms(Reader &reader, VartContext &) {
    u16 duration;
    if (isFail(reader.read(duration))) { return VartResult::InstructionArgumentReadError; }

    delay(duration);

    return VartResult::Ok;
}

//  2: [2B] vart::set_speed@2(std::u8)
static VartResult set_speed(Reader &reader, VartContext &context) {
    u8 speed_set;
    if (isFail(reader.read(speed_set))) { return VartResult::InstructionArgumentReadError; }

    context.planner.setSpeed(speed_set);

    return VartResult::Ok;
}

//  3: [2B] vart::set_accel@3(std::u8)
static VartResult set_accel(Reader &reader, VartContext &context) {
    u8 accel_set;
    if (isFail(reader.read(accel_set))) { return VartResult::InstructionArgumentReadError; }

    context.planner.setAccel(accel_set);

    return VartResult::Ok;
}

//  4: [2B] vart::set_planner_mode@4(std::u8)
static VartResult set_planner_mode(Reader &reader, VartContext &context) {
    u8 mode;
    if (isFail(reader.read(mode))) { return VartResult::InstructionArgumentReadError; }

    context.planner.setMode(static_cast<vart::Planner::Mode>(mode));

    return VartResult::Ok;
}

//  5: [5B] vart::set_position@5(std::i16, std::i16)
static VartResult set_position(Reader &reader, VartContext &context) {
    i16 x;
    if (isFail(reader.read(x))) { return InstructionArgumentReadErrorAt(1); }

    i16 y;
    if (isFail(reader.read(y))) { return InstructionArgumentReadErrorAt(2); }

    context.planner.moveTo({double(x), double(y)});

    return VartResult::Ok;
}

//  6: [2B] vart::set_progress@6(std::u8)
static VartResult set_progress(Reader &reader, VartContext &context) {
    u8 progress;
    if (isFail(reader.read(progress))) { return VartResult::InstructionArgumentReadError; }

    context.progress = progress;

    return VartResult::Ok;
}

//  7: [2B] vart::set_active_tool@7(std::u8)
static VartResult set_active_tool(Reader &reader, VartContext &context) {
    u8 tool_id;
    if (isFail(reader.read(tool_id))) { return VartResult::InstructionArgumentReadError; }

    context.tool.setActiveTool(static_cast<vart::MarkerPrintTool::Marker>(tool_id));

    return VartResult::Ok;
}

vart::VartIntepreter vart::interpreter = {
    .is_aborted = false,
    .is_paused = false,
    .instruction_count = 7,
    .instruction_table = {
        quit,
        delay_ms,
        set_speed,
        set_accel,
        set_planner_mode,
        set_position,
        set_progress,
        set_active_tool
    }
};
