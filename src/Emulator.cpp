#include "Emulator.h"

#include <iostream>
#include <iterator>

#include <Compiler.h>
#include <Decoder.h>

namespace momiji
{

    static std::uint16_t* immediateIncrPC(const momiji::DecodedInstruction& instr,
                                std::uint16_t* pc)
    {
        switch (instr.data.size)
        {
        case 1:
        case 2:
            pc += 2;
            break;

        case 4:
            pc += 3;
            break;
        }

        return pc;
    }

    Emulator::Emulator()
        : systemStates(1)
    {
    }

    const std::vector<momiji::System>& Emulator::getStates() const
    {
        return systemStates;
    }

    std::optional<momiji::ParserError>
    Emulator::newState(const std::string& str)
    {
        auto res = momiji::parse(str);

        if (res)
        {
            auto mem = momiji::compile(*res);
            auto lastSys = systemStates.back();
            lastSys.mem = std::move(mem);
            lastSys.cpu.programCounter.address = lastSys.mem.data();
            systemStates.emplace_back(std::move(lastSys));
        }

        return std::nullopt;
    }

    bool Emulator::rollbackSys()
    {
        if (systemStates.size() > 1)
        {
            systemStates.pop_back();
            return true;
        }

        return false;
    }

    bool Emulator::rollback()
    {
        auto& lastSys = systemStates.back();
        if (lastSys.cpu.programCounter.address > lastSys.mem.data())
        {
            --lastSys.cpu.programCounter.address;

            return true;
        }

        return false;
    }

    bool Emulator::step()
    {
        auto& lastSys = systemStates.back();
        const auto* pc = lastSys.cpu.programCounter.address;
        if (pc < lastSys.mem.data() ||
            pc >= (lastSys.mem.data() + lastSys.mem.size()))
        {
            return false;
        }

        auto offset = pc - lastSys.mem.data();

        gsl::span<std::uint16_t> span { lastSys.mem.data(),
                                        lastSys.mem.size() };
        const auto& instr = momiji::decode(span, pc - lastSys.mem.data());

        System newstate = instr.exec(lastSys, instr.data);
        newstate.cpu.programCounter.address = newstate.mem.data() + offset;

        /*
        switch (instr.instructionType)
        {
        case InstructionType::Jmp: [[fallthrough]];
        case InstructionType::BranchEqual: [[fallthrough]];
        case InstructionType::BranchGreaterEquals: [[fallthrough]];
        case InstructionType::BranchGreaterThan: [[fallthrough]];
        case InstructionType::BranchLessEquals: [[fallthrough]];
        case InstructionType::BranchLessThan: [[fallthrough]];
        case InstructionType::BranchNotEquals: [[fallthrough]];
        case InstructionType::BranchNotZero: [[fallthrough]];
        case InstructionType::BranchZero:
            break;

        default:
            ++newstate.cpu.programCounter.address;
        }
        */

        bool pc_incremented = false;

        if (instr.data.op1 == OperandType::Immediate)
        {
            newstate.cpu.programCounter.address =
                immediateIncrPC(instr, newstate.cpu.programCounter.address);

            pc_incremented = true;
        }

        if (instr.data.op2 == OperandType::Immediate)
        {
            newstate.cpu.programCounter.address =
                immediateIncrPC(instr, newstate.cpu.programCounter.address);

            pc_incremented = true;
        }

        if (!pc_incremented)
        {
            ++newstate.cpu.programCounter.address;
        }

        systemStates.emplace_back(std::move(newstate));

        return true;
    }

    bool Emulator::reset()
    {
        bool ret = false;
        while (systemStates.size() > 1)
        {
            systemStates.pop_back();
            ret = true;
        }

        return ret;
    }
} // namespace momiji
