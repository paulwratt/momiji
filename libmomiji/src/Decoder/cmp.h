#pragma once

#include <Decoder.h>
#include <System.h>
#include <gsl/span>

namespace momiji
{
    namespace dec
    {
        DecodedInstruction cmp(ExecutableMemoryView mem, std::int64_t idx);
        DecodedInstruction cmpi(ExecutableMemoryView mem, std::int64_t idx);
        DecodedInstruction cmpa(ExecutableMemoryView mem, std::int64_t idx);
    } // namespace dec
} // namespace momiji
