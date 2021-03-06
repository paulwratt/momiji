#include "bcc.h"

#include "./Utils.h"
#include <System.h>

namespace momiji::instr
{
    momiji::System bcc(momiji::System& sys, const InstructionData& data)
    {
        const auto pc        = sys.cpu.programCounter;
        const auto condition = utils::to_val(data.operandType[0]);

        ProgramCounter::value_type offset = utils::to_val(data.operandType[1]);

        bool skipTwoBytes = false;

        if (offset == 0)
        {
            skipTwoBytes = true;

            const auto nextloc = pc + 2;

            offset = *sys.mem.read16(nextloc.raw());
        }

        const auto& statReg = sys.cpu.statusRegister;

        const auto normalIncrement = [&]() {
            sys.cpu.programCounter += skipTwoBytes ? 4 : 2;
        };

        bool shouldBranch = false;
        switch (condition)
        {
        // NE
        case 0b0110:
            if (statReg.zero == 0)
            {
                shouldBranch = true;
            }
            break;

        // EQ
        case 0b0111:
            if (statReg.zero == 1)
            {
                shouldBranch = true;
            }
            break;

        // GE
        case 0b1100:
            if ((statReg.negative == 0 && statReg.overflow == 0) ||
                (statReg.negative == 1 && statReg.overflow == 1))
            {
                shouldBranch = true;
            }
            break;

        // LT
        case 0b1101:
            if ((statReg.negative == 0 && statReg.overflow == 1) ||
                (statReg.negative == 1 && statReg.overflow == 0))
            {
                shouldBranch = true;
            }
            break;

        // GT
        case 0b1110:
            if ((statReg.zero == 0 && statReg.negative == 0 &&
                 statReg.overflow == 0) ||
                (statReg.zero == 0 && statReg.negative == 1 &&
                 statReg.overflow == 1))
            {
                shouldBranch = true;
            }
            break;

        // LE
        case 0b1111:
            if ((statReg.negative == 0 && statReg.overflow == 1) ||
                (statReg.negative == 1 && statReg.overflow == 0) ||
                (statReg.zero == 1))
            {
                shouldBranch = true;
            }
            break;
        }

        if (shouldBranch)
        {
            sys.cpu.programCounter += offset;
        }
        else
        {
            normalIncrement();
        }

        return sys;
    }
} // namespace momiji::instr
