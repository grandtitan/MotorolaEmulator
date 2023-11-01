#ifndef INSTRUCTIONLIST_H
#define INSTRUCTIONLIST_H

#include <vector>

class InstructionList {
public:
    void clear();  // Function to clear the list

    void addInstruction(int address, int lineNumber, int byte1, int byte2, int byte3);

    int findAddressByLineNum(int lineNumber) const;
    int findLineNumByAddress(int address) const;

    void getBytesByAddress(int address, int& byte1, int& byte2, int& byte3) const;

private:
    struct Instruction {
        int address;
        int lineNumber;
        int byte1;
        int byte2;
        int byte3;
    };

    std::vector<Instruction> instructions;
};

#endif // INSTRUCTIONLIST_H
