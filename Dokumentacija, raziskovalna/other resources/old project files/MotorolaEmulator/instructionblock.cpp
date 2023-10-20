#include "instructionblock.h"
#include <QMap>

// Definition of the instruction list
QList<InstructionBlock> instructionList;
QMap<int, int> LineAdressMap;

InstructionBlock::InstructionBlock(int _address, const QString& _instruction, const QString& _operand, int _byte1, int _byte2, int _byte3)
    : address(_address), instruction(_instruction), operand(_operand), byte1(_byte1), byte2(_byte2), byte3(_byte3)
{

}

void addInstruction(int address, const QString& instruction, const QString& operand, int byte1, int byte2, int byte3)
{
    InstructionBlock obj(address, instruction, operand, byte1, byte2, byte3);
    instructionList.append(obj);
    LineAdressMap[instructionList.size()] = address;
}

void clearInstructions(){
    instructionList.clear();
    LineAdressMap.clear();
}

int getAddressByLine(int line){
    int adr = LineAdressMap.value(line, -1);
    return adr;
}

int getLineByAddress(int adr){
    int line = LineAdressMap.key(adr, -1);
    return line;
}
