#pragma once

#include <momiji/Decoder.h>

namespace momiji::dec
{
    DecodedInstruction not_instr(ConstExecutableMemoryView mem,
                                 std::int64_t idx);

}
