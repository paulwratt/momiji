#pragma once

#include <Decoder.h>
#include <System.h>

namespace momiji::dec
{
    DecodedInstruction exg(ExecutableMemoryView mem, std::int64_t idx);
}
