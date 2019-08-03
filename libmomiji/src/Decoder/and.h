#pragma once

#include <Decoder.h>
#include <System.h>
#include <gsl/span>

namespace momiji
{
    namespace dec
    {
        DecodedInstruction and_instr(ExecutableMemoryView mem,
                                     std::int64_t idx);
        DecodedInstruction andi(ExecutableMemoryView mem, std::int64_t idx);
    } // namespace dec
} // namespace momiji
