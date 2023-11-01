#ifndef INSTRUCTIONLIST_H
#define INSTRUCTIONLIST_H

#include <vector>

class InstructionList {
public:
    struct Instruction {
        int address;
        int lineNumber;
        int byte1;
        int byte2;
        int byte3;
    };

    void clear();
    void addInstruction(int address, int lineNumber, int byte1, int byte2, int byte3);

    const Instruction& getObjectByAddress(int address) const;
    const Instruction& getObjectByLine(int lineNumber) const;

    bool isEmpty() const;
private:
    std::vector<Instruction> instructions;
};

#endif // INSTRUCTIONLIST_H
