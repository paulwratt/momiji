#include <momiji/Emulator.h>

#include <iterator>

#include <iostream>
#include <momiji/Compiler.h>
#include <momiji/Decoder.h>

#include "Instructions/bcc.h"
#include "Instructions/bra.h"
#include "Instructions/jmp.h"

namespace momiji
{
    Emulator::Emulator()
        : m_systemStates(1)
        , m_settings({ 0,
                       -1,
                       utils::make_kb(4),
                       EmulatorSettings::RetainStates::Always,
                       {} })
    {
    }

    Emulator::Emulator(EmulatorSettings settings)
        : m_systemStates(1)
        , m_settings(std::move(settings))
    {
    }

    const std::vector<momiji::System>& Emulator::getStates() const
    {
        return m_systemStates;
    }

    std::optional<momiji::ParserError>
    Emulator::newState(const std::string& str)
    {
        if (str.empty())
        {
            return std::nullopt;
        }

        auto res = momiji::parse(str, m_settings.parserSettings);

        if (res)
        {
            auto mem = momiji::compile(*res);

            if ((m_settings.stackSize & 0b1) != 0)
            {
                ++m_settings.stackSize;
            }

            mem.executableMarker.begin = 0;
            mem.executableMarker.end   = asl::ssize(mem);

            mem.stackMarker.begin = asl::ssize(mem);
            mem.stackMarker.end = mem.stackMarker.begin + m_settings.stackSize;

            mem.underlying().resize(std::size_t(mem.stackMarker.end), 0);

            auto lastSys = m_systemStates.back();
            lastSys.mem  = std::move(mem);

            lastSys.cpu.addressRegisters[7] =
                std::int32_t(lastSys.mem.size() - 2);
            m_systemStates.emplace_back(std::move(lastSys));

            return std::nullopt;
        }

        return res.error();
    }

    void Emulator::newState(momiji::ExecutableMemory binary)
    {
        auto lastSys = m_systemStates.back();
        lastSys.mem  = std::move(binary);
        auto& mem    = lastSys.mem;

        if ((m_settings.stackSize & 0b1) != 0)
        {
            ++m_settings.stackSize;
        }

        mem.executableMarker.begin = 0;
        mem.executableMarker.end   = asl::ssize(mem);

        mem.stackMarker.begin = asl::ssize(mem);
        mem.stackMarker.end   = mem.stackMarker.begin + m_settings.stackSize;

        mem.underlying().resize(std::size_t(mem.stackMarker.end), 0);
        lastSys.cpu.addressRegisters[7] = std::int32_t(lastSys.mem.size() - 2);

        m_systemStates.emplace_back(std::move(lastSys));
    }

    bool Emulator::rollback()
    {
        if (m_systemStates.size() > 1)
        {
            m_systemStates.pop_back();
            return true;
        }

        return false;
    }

    bool Emulator::step()
    {
        auto& lastSys = m_systemStates.back();

        if (lastSys.mem.empty())
        {
            return false;
        }

        const auto pc = lastSys.cpu.programCounter.raw();
        auto memview  = momiji::make_memory_view(lastSys);

        auto pcadd = memview.executableMarker.begin + pc;

        const auto membegin = memview.executableMarker.begin;
        const auto memend   = memview.executableMarker.end;

        if (pcadd < membegin || pcadd >= memend)
        {
            return false;
        }

        auto instr = momiji::decode(memview, pc);

        switch (m_settings.retainStates)
        {
        case EmulatorSettings::RetainStates::Never:
            return stepHandleMem(never_retain_states_tag {}, instr);

        case EmulatorSettings::RetainStates::Always:
            return stepHandleMem(always_retain_states_tag {}, instr);
        }

        return false;
    }

    bool Emulator::stepHandleMem(never_retain_states_tag /*unused*/,
                                 DecodedInstruction& instr)
    {
        auto& lastSys = m_systemStates.back();

        instr.exec(lastSys, instr.data);

        // handlePC(lastSys, instr);

        return true;
    }

    bool Emulator::stepHandleMem(always_retain_states_tag /*unused*/,
                                 DecodedInstruction& instr)
    {
        auto& lastSys = m_systemStates.back();

        // Copy the new state
        auto newstate = lastSys;

        instr.exec(newstate, instr.data);

        // handlePC(newstate, instr);

        m_systemStates.emplace_back(std::move(newstate));

        return true;
    }

    bool Emulator::reset()
    {
        bool ret = false;
        while (m_systemStates.size() > 1)
        {
            m_systemStates.pop_back();
            ret = true;
        }

        return ret;
    }

    void Emulator::loadNewSettings(EmulatorSettings settings)
    {
        reset();

        m_settings = settings;
    }

    [[nodiscard]] EmulatorSettings Emulator::getSettings() const noexcept
    {
        return m_settings;
    }

    void continueEmulatorExecution(Emulator& emu) noexcept
    {
        while (emu.step())
        {
        }
    }
} // namespace momiji
