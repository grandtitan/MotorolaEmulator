#include "InstructionList.h"
#include <stdexcept>

void InstructionList::clear() {
    instructions.clear();
}

void InstructionList::addInstruction(int address, int lineNumber, int byte1, int byte2, int byte3) {
    Instruction instruction;
    instruction.address = address;
    instruction.lineNumber = lineNumber;
    instruction.byte1 = byte1;
    instruction.byte2 = byte2;
    instruction.byte3 = byte3;
    instructions.push_back(instruction);
}

const InstructionList::Instruction& InstructionList::getObjectByAddress(int address) const {
    for (const Instruction& instruction : instructions) {
        if (instruction.address == address) {
            return instruction;
        }
    }
    static const Instruction defaultInstruction = { address, -1, 0, 0, 0 };
    return defaultInstruction;
}

const InstructionList::Instruction& InstructionList::getObjectByLine(int lineNumber) const {
    for (const Instruction& instruction : instructions) {
        if (instruction.lineNumber == lineNumber) {
            return instruction;
        }
    }
    static const Instruction defaultInstruction = { -1, lineNumber, 0, 0, 0 };
    return defaultInstruction;
}

bool InstructionList::isEmpty() const {
    return instructions.empty();
}
