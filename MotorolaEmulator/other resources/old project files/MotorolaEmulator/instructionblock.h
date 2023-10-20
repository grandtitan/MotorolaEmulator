#ifndef INSTRUCTIONBLOCK_H
#define INSTRUCTIONBLOCK_H

#include <QString>
#include <QList>
#include <QMap>
#include <optional>

class InstructionBlock
{
public:
    int address;
    QString instruction;
    QString operand;
    int byte1;
    int byte2;
    int byte3;

    InstructionBlock(int _address, const QString& _instruction, const QString& _operand,
                     int _byte1,
                     int _byte2,
                     int _byte3);
};

// Declaration of the instruction list
extern QList<InstructionBlock> instructionList;
extern QMap<int, int> LineAddressMap; // Corrected spelling

// Function to add a new entry to the instruction list
void addInstruction(int address, const QString& instruction, const QString& operand,
                    int byte1,
                    int byte2,
                    int byte3);

// Function to clear the instruction list
void clearInstructions();

// Function to get address based on line number
int getAddressByLine(int line);

// Function to get line number based on address
int getLineByAddress(int address);

#endif // INSTRUCTIONBLOCK_H
