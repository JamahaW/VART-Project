#include <vart/Devices.hpp>
#include <vart/util/Macro.hpp>


using VartResult = vart::VartIntepreter::Result;
using vart::Context;

using bytelang::Reader;
using namespace bytelang::primitive;

vart::Context vart::context = {
    .progress = 0,
    .quit_code = 0
};

/// Считанный результат является ошибкой
static inline bool isFail(Reader::Result result) { return result == Reader::Result::Fail; }

constexpr static inline VartResult InstructionArgumentReadErrorAt(u8 at) {
    return static_cast<VartResult>(VartResult::InstructionArgumentReadError | (0x0F & at));
}

//  0: [1B] vart::quit@0()
static VartResult quit(Reader &, Context &) {
    return VartResult::ExitOk;
}

//  1: [3B] vart::delay_ms@1(std::u16)
static VartResult delay_ms(Reader &reader, Context &) {
    u16 duration;
    if (isFail(reader.read(duration))) { return VartResult::InstructionArgumentReadError; }

    delay(duration);

    return VartResult::Ok;
}

//  2: [2B] vart::set_speed@2(std::u8)
static VartResult set_speed(Reader &reader, Context &) {
    u8 speed_set;
    if (isFail(reader.read(speed_set))) { return VartResult::InstructionArgumentReadError; }

    vart::device.planner.setSpeed(speed_set);

    return VartResult::Ok;
}

//  3: [2B] vart::set_accel@3(std::u8)
static VartResult set_accel(Reader &reader, Context &) {
    u8 accel_set;
    if (isFail(reader.read(accel_set))) { return VartResult::InstructionArgumentReadError; }

    vart::device.planner.setAccel(accel_set);

    return VartResult::Ok;
}

//  4: [2B] vart::set_planner_mode@4(std::u8)
static VartResult set_planner_mode(Reader &reader, Context &) {
    u8 mode;
    if (isFail(reader.read(mode))) { return VartResult::InstructionArgumentReadError; }

    vart::device.planner.setMode(static_cast<vart::Planner::Mode>(mode));

    return VartResult::Ok;
}

//  5: [5B] vart::set_position@5(std::i16, std::i16)
static VartResult set_position(Reader &reader, Context &) {
    i16 x, y;
    if (isFail(reader.read(x))) { return InstructionArgumentReadErrorAt(1); }
    if (isFail(reader.read(y))) { return InstructionArgumentReadErrorAt(2); }

    vart::device.planner.moveTo({double(x), double(y)});

    return VartResult::Ok;
}

//  6: [2B] vart::set_progress@6(std::u8)
static VartResult set_progress(Reader &reader, Context &context) {
    u8 progress;
    if (isFail(reader.read(progress))) { return VartResult::InstructionArgumentReadError; }

    context.progress = progress;

    vart::window.addEvent(ui2::Event::ForceUpdate);

    return VartResult::Ok;
}

//  7: [2B] vart::set_active_tool@7(std::u8)
static VartResult set_active_tool(Reader &reader, Context &) {
    u8 tool_id;
    if (isFail(reader.read(tool_id))) { return VartResult::InstructionArgumentReadError; }

    auto &tool = vart::device.tool;

    tool.setActiveTool(static_cast<vart::MarkerPrintTool::Marker>(tool_id));

    while (not tool.servo.isReady()) {
        taskYIELD();
    }

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
