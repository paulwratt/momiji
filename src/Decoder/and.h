#pragma once

#include <System.h>
#include <Decoder.h>
#include <gsl/span>

namespace momiji
{
    namespace dec
    {
        DecodedInstruction and_instr(ExecutableMemoryView mem, std::uint64_t idx);
        DecodedInstruction andi(ExecutableMemoryView mem, std::uint64_t idx);
    }
}

