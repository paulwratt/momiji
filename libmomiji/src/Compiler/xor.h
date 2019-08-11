#pragma once

#include "./Utils.h"
#include <momiji/Parser.h>

namespace momiji::enc
{
    void xor_instr(const momiji::ParsedInstruction& instr,
                   const momiji::LabelInfo& labels,
                   OpcodeDescription& opcode,
                   std::array<AdditionalData, 2>& additionalData);

    void xori(const momiji::ParsedInstruction& instr,
              const momiji::LabelInfo& labels,
              OpcodeDescription& opcode,
              std::array<AdditionalData, 2>& additionalData);
} // namespace momiji::enc
