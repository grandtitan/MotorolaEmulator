#include "InstructionList.h"

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

int InstructionList::findAddressByLineNum(int lineNumber) const {
    for (const Instruction& instruction : instructions) {
        if (instruction.lineNumber == lineNumber) {
            return instruction.address;
        }
    }
    return -1;
}

int InstructionList::findLineNumByAddress(int address) const {
    for (const Instruction& instruction : instructions) {
        if (instruction.address == address) {
            return instruction.lineNumber;
        }
    }
    return -1;
}

void InstructionList::getBytesByAddress(int address, int& byte1, int& byte2, int& byte3) const {
    for (const Instruction& instruction : instructions) {
        if (instruction.address == address) {
            byte1 = instruction.byte1;
            byte2 = instruction.byte2;
            byte3 = instruction.byte3;
            return;
        }
    }
    byte1 = byte2 = byte3 = -1;
}
