#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringBuilder>
#include <QScrollBar>
#include <iostream>
#include <map>
#include <iomanip>
#include <sstream>
#include <QTextBlock>
#include <QString>
#include <QTimer>
#include <cmath>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMouseEvent>
#include <QTableWidget>
inline bool bit(int variable, int bitNum){
    return (variable & (1 << bitNum)) != 0;
}

bool MainWindow::executeInstructionM6800(){
    uint8_t uInt8 = 0;
    uint8_t uInt82 = 0;
    int8_t sInt8 = 0;
    uint16_t uInt16 = 0;
    uint16_t uInt162 = 0;
    uint16_t adr = 0;
    uint16_t* curIndReg = &xRegister;
    if(ui->plainTextDisplay->hasFocus()){
        QPoint position = QCursor::pos();
        position.setX(position.x() - 4);
        QPoint localMousePos = ui->plainTextDisplay->mapFromGlobal(position);
        QTextCursor cursor = ui->plainTextDisplay->cursorForPosition(localMousePos);
        int x = cursor.position() % 55;
        if(x > 53) x = 53;
        int y = cursor.position() / 55;
        if(y > 19) y = 19;
        Memory[0xFFF2] = x;
        Memory[0xFFF3] = y;
        updateMemoryCell(0xFFF2);
        updateMemoryCell(0xFFF3);
    }
    if (!indexRegister) {
        //curIndReg = &yRegister;
    }
    switch(Memory[PC]){
    case 0x00:
        if (running){
            stopExecution();
        }else{
            PC++;
        }
        break;
    case 0x01:
        PC++;
        //NOP
        break;
    case 0x06:
        updateFlags(HalfCarry, bit(aReg, 5));
        updateFlags(InterruptMask, bit(aReg, 4));
        updateFlags(Negative, bit(aReg, 3));
        updateFlags(Zero, bit(aReg, 2));
        updateFlags(Overflow, bit(aReg, 1));
        updateFlags(Carry, bit(aReg, 0));
        PC++;
        break;
    case 0x07:
        aReg = flags;
        updateElement(regA);
        PC++;
        break;
    case 0x08:
        (*curIndReg)++;
        updateFlags(Zero, (*curIndReg) == 0);
        updateElement(regX);
        PC++;
        break;
    case 0x09:
        (*curIndReg)--;
        updateFlags(Zero, (*curIndReg) == 0);
        updateElement(regX);
        PC++;
        break;
    case 0x0A:
        updateFlags(Overflow, 0);
        PC++;
        break;
    case 0x0B:
        updateFlags(Overflow, 1);
        PC++;
        break;
    case 0x0C:
        updateFlags(Carry, 0);
        PC++;
        break;
    case 0x0D:
        updateFlags(Carry, 1);
        PC++;
        break;
    case 0x0E:
        updateFlags(InterruptMask, 0);
        PC++;
        break;
    case 0x0F:
        updateFlags(InterruptMask, 1);
        PC++;
        break;
    case 0x10:
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7)  && bit(bReg, 7) && bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
        aReg = uInt8;
        updateElement(regA);
        PC++;
        break;
    case 0x11:
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7) && bit(bReg, 7) && bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
        PC++;
        break;
    case 0x16:
        bReg = aReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC++;
        break;
    case 0x17:
        aReg = bReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC++;
        break;
    case 0x19:
        uInt8 = flags & 0x01;
        uInt82 = bit(flags, 5);
        uInt16 = aReg >> 4;
        uInt162 = aReg & 0xF;
        if (uInt8 != 1 && uInt16 <= 9 && uInt82 != 1 && uInt162 <= 9){
            aReg+=0x0;
            updateFlags(Carry,0);
        }else if (uInt8 != 1 && uInt16 <= 8 && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (uInt8 != 1 && uInt16 <= 9 && uInt82 && uInt162 <= 3){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (uInt8 != 1 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 != 1 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (uInt8 != 1 && uInt16 >= 9 && uInt16 <= 0xF && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 != 1 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 && uInt162 <= 3){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 <= 2 && uInt82 != 1 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 <= 2 && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 <= 3 && uInt82 && uInt162 <= 3){
            aReg+=0x66;
            updateFlags(Carry,1);
        }
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x1B:
        uInt16 = aReg + bReg;
        updateFlags(Carry, (bit(aReg, 7) && bit(bReg, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(bReg, 7) && !bit(uInt16, 7)));
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(bReg, 3)) || (bit(aReg, 3) && !bit(uInt16, 3)) || (bit(bReg, 3) && !bit(uInt16, 3)));
        updateFlags(Overflow, (bit(aReg, 7) && bit(bReg, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(bReg, 7) && bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x20:
        sInt8 = Memory[(PC + 1) % 0x10000];
        PC += sInt8 + 2;
        break;
    case 0x22:
        if ((bit(flags, 2) || (flags & 0x01)) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x23:
        if (bit(flags, 2) || (flags & 0x01))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x24:
        if ((flags & 0x01) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x25:
        if ((flags & 0x01)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x26:
        if (bit(flags, 2) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x27:
        if (bit(flags, 2)) {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x28:
        if (bit(flags, 1) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x29:
        if (bit(flags, 1)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2A:
        if (bit(flags, 3) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2B:
        if (bit(flags, 3)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2C:
        if ((bit(flags, 3) ^ bit(flags, 1)) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2D:
        if (bit(flags, 3) ^ bit(flags, 1))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2E:
        if ((bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1))) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2F:
        if (bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1)))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x30:
        (*curIndReg) = SP + 1;
        updateElement(regX);
        PC++;
        break;
    case 0x31:
        SP++;
        updateElement(regSP);
        PC++;
        break;
    case 0x32:
        SP++;
        aReg = Memory[SP];
        updateElement(regSP);
        updateElement(regA);
        PC++;
        break;
    case 0x33:
        SP++;
        bReg = Memory[SP];
        updateElement(regSP);
        updateElement(regB);
        PC++;
        break;
    case 0x34:
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x35:
        SP = (*curIndReg) - 1;
        updateElement(regSP);
        PC++;
        break;
    case 0x36:
        Memory[SP] = aReg;
        updateMemoryCell(SP);
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x37:
        Memory[SP] = bReg;
        updateMemoryCell(SP);
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x39:
        SP++;
        PC = Memory[SP] << 8;
        SP++;
        PC += Memory[SP];
        updateElement(regSP);
        break;
    case 0x3B:
        SP++;
        flags = Memory[SP];

        SP++;
        bReg = Memory[SP];

        SP++;
        aReg = Memory[SP];

        SP++;
        xRegister = (Memory[SP] << 8) + Memory[(SP+ 1) % 0x10000];

        SP+=2;
        PC = (Memory[SP] << 8) + Memory[(SP+ 1) % 0x10000];
        SP++;
        updateElement(regSP);
        updateElement(regA);
        updateElement(regB);
        updateElement(regX);
        updateElement(regPC);
        updateElement(allFlags);
        break;
    case 0x3E:
        if(lastInput != -1){
            PC++;
            lastInput = -1;
        }
        break;
    case 0x3F:
        PC++;
        Memory[SP] = PC & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = (PC >> 8) & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = xRegister & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = (xRegister >> 8) & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = aReg;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = bReg;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = flags;
        updateMemoryCell(SP);
        SP--;
        updateFlags(InterruptMask, 1);
        PC = (Memory[(interruptLocations - 5)] << 8) + Memory[(interruptLocations - 4)];
        updateElement(regSP);
        break;
    case 0x40:
        aReg = 0x0 - aReg;
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, aReg == 0x80);
        updateFlags(Carry, aReg != 0);
        updateElement(regA);
        PC++;
        break;
    case 0x43:
        aReg = 0xFF - aReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateElement(regA);
        PC++;
        break;
    case 0x44:
        uInt8 = (aReg & 0x1);
        aReg = (aReg >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateElement(regA);
        PC++;
        break;
    case 0x46:
        uInt8 = (aReg & 0x01);
        aReg = aReg >> 1;
        aReg += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x47:
        uInt8 = aReg & 0x01;
        updateFlags(Carry, uInt8);
        aReg = (aReg >> 1) + (aReg & 0x80);

        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x48:
        uInt8 = bit(aReg, 7);
        updateFlags(Carry, uInt8);
        aReg = aReg << 1;
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x49:
        uInt8 = bit(aReg, 7);
        aReg = (aReg << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x4A:
        updateFlags(Overflow, aReg == 0x80);
        aReg--;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x4C:
        updateFlags(Overflow, aReg == 0x7F);
        aReg++;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x4D:
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC++;
        break;
    case 0x4F:
        aReg = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateElement(regA);
        PC++;
        break;
    case 0x50:
        bReg = 0x0 - bReg;
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, bReg == 0x80);
        updateFlags(Carry, bReg != 0);
        updateElement(regB);
        PC++;
        break;
    case 0x53:
        bReg = 0xFF - bReg;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateElement(regB);
        PC++;
        break;
    case 0x54:
        uInt8 = (bReg & 0x1);
        bReg = (bReg >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateElement(regB);
        PC++;
        break;
    case 0x56:
        uInt8 = (bReg & 0x01);
        bReg = bReg >> 1;
        bReg += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x57:
        uInt8 = bReg & 0x01;
        updateFlags(Carry, uInt8);
        bReg = (bReg >> 1) + (bReg & 0x80);
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x58:
        uInt8 = bit(bReg, 7);
        updateFlags(Carry, uInt8);
        bReg = bReg << 1;
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x59:
        uInt8 = bit(bReg, 7);
        bReg = (bReg << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x5A:
        updateFlags(Overflow, bReg == 0x80);
        bReg--;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC++;
        break;
    case 0x5C:
        updateFlags(Overflow, bReg == 0x7F);
        bReg++;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC++;
        break;
    case 0x5D:
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC++;
        break;
    case 0x5F:
        bReg = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateElement(regB);
        PC++;
        break;
    case 0x60:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = 0x0 - Memory[adr];
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, Memory[adr] == 0x80);
        updateFlags(Carry, Memory[adr] != 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x63:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = 0xFF - Memory[adr];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x64:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = (Memory[adr] & 0x1);
        Memory[adr] = (Memory[adr] >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x66:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = (Memory[adr] & 0x01);
        Memory[adr] = Memory[adr] >> 1;
        Memory[adr] += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x67:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = (Memory[adr] & 0x01);
        updateFlags(Carry, uInt8);
        Memory[adr] = (Memory[adr] >> 1) + (Memory[adr] & 0x80);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x68:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = bit(Memory[adr], 7);
        updateFlags(Carry, uInt8);
        Memory[adr] = Memory[adr] << 1;
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x69:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = bit(Memory[adr], 7);
        Memory[adr] = (Memory[adr] << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6A:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Overflow, Memory[adr] == 0x80);
        Memory[adr]--;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6C:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Overflow, Memory[adr] == 0x7F);
        Memory[adr]++;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6D:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC+=2;
        break;
    case 0x6E:
        PC = ((Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000);
        break;
    case 0x6F:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x70:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = 0x0 - Memory[adr];
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, Memory[adr] == 0x80);
        updateFlags(Carry, Memory[adr] != 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x73:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = 0xFF - Memory[adr];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x74:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = (Memory[adr] & 0x1);
        Memory[adr] = (Memory[adr] >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x76:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = (Memory[adr] & 0x01);
        Memory[adr] = Memory[adr] >> 1;
        Memory[adr] += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x77:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = (Memory[adr] & 0x01);
        updateFlags(Carry, uInt8);
        Memory[adr] = (Memory[adr] >> 1) + (Memory[adr] & 0x80);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x78:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = bit(Memory[adr], 7);
        updateFlags(Carry, uInt8);
        Memory[adr] = Memory[adr] << 1;
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x79:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = bit(Memory[adr], 7);
        Memory[adr] = (Memory[adr] << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7A:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Overflow, Memory[adr] == 0x80);
        Memory[adr]--;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7C:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Overflow, Memory[adr] == 0x7F);
        Memory[adr]++;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7D:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC+=3;
        break;
    case 0x7E:
        PC = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        break;
    case 0x7F:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x80:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x81:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0x82:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x84:
        aReg = (aReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x85:
        uInt8 = (aReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0x86:
        aReg = Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x88:
        aReg = aReg ^ Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x89:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8A:
        aReg = aReg | Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8B:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8C:
        uInt16 = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=3;
        break;
    case 0x8D:
        sInt8 = Memory[(PC+1) % 0x10000];
        PC+=2;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC += sInt8;
        updateElement(regSP);
        break;
    case 0x8E:
        SP = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regSP);
        break;
    case 0x90:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x91:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0x92:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x94:
        aReg = (aReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x95:
        uInt8 = (aReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0x96:
        aReg = Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x97:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x98:
        aReg = aReg ^ Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x99:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9A:
        aReg = aReg | Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9B:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9C:
        adr = Memory[(PC+1) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=2;
        break;
    case 0x9E:
        adr = Memory[(PC+1) % 0x10000];
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regSP);
        break;
    case 0x9F:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = SP >> 8;
        Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xA0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0xA1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0xA2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0xA4:
        aReg = (aReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA5:
        uInt8 = (aReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xA6:
        aReg = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA7:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xA8:
        aReg = aReg ^ Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAA:
        aReg = aReg | Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAC:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=2;
        break;
    case 0xAD:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;;
        PC+=2;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC = adr;
        updateElement(regSP);
        break;
    case 0xAE:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regSP);
        break;
    case 0xAF:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = SP >> 8;
        Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xB0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=3;
        updateElement(regA);
        break;
    case 0xB1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=3;
        break;
    case 0xB2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=3;
        updateElement(regA);
        break;
    case 0xB4:
        aReg = (aReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB5:
        uInt8 = (aReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=3;
        break;
    case 0xB6:
        aReg = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB7:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0xB8:
        aReg = aReg ^ Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBA:
        aReg = aReg | Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBC:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=3;
        break;
    case 0xBD:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        PC+=3;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC = adr;
        updateElement(regSP);
        break;
    case 0xBE:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regSP);
        break;
    case 0xBF:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = SP >> 8;
        Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=3;
        break;
    case 0xC0:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xC1:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xC2:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xC4:
        bReg = (bReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC5:
        uInt8 = (bReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xC6:
        bReg = Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC8:
        bReg = bReg ^ Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC9:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCA:
        bReg = bReg | Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCB:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCE:
        (*curIndReg) = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regX);
        break;
    case 0xD0:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xD1:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xD2:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xD4:
        bReg =( bReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD5:
        uInt8 = (bReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xD6:
        bReg = Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD7:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xD8:
        bReg = bReg ^ Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD9:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDA:
        bReg = bReg | Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDB:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDE:
        adr = Memory[(PC+1) % 0x10000];
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regX);
        break;
    case 0xDF:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = (*curIndReg) >> 8;
        Memory[(adr + 1) % 0x10000] = ((*curIndReg) & 0xFF);
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xE0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xE1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xE2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xE4:
        bReg = (bReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE5:
        uInt8 = (bReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xE6:
        bReg = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE7:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xE8:
        bReg = bReg ^ Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEA:
        bReg = bReg | Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEE:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regX);
        break;
    case 0xEF:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = (*curIndReg) >> 8;
        Memory[(adr + 1) % 0x10000] = ((*curIndReg) & 0xFF);
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xF0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=3;
        updateElement(regB);
        break;
    case 0xF1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=3;
        break;
    case 0xF2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=3;
        updateElement(regB);
        break;
    case 0xF4:
        bReg = (bReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF5:
        uInt8 = (bReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=3;
        break;
    case 0xF6:
        bReg = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF7:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0xF8:
        bReg = bReg ^ Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFA:
        bReg = bReg | Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFE:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regX);
        break;
    case 0xFF:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = (*curIndReg) >> 8;
        Memory[(adr + 1) % 0x10000] = ((*curIndReg) & 0xFF);
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=3;
        break;
    default:
        PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
        PC++;
        break;
    }

    PC = PC % 0x10000;
    updateElement(regPC);
    return true;
}
bool MainWindow::executeInstructionM6803(){
    uint8_t uInt8 = 0;
    uint8_t uInt82 = 0;
    int8_t sInt8 = 0;
    uint16_t uInt16 = 0;
    uint16_t uInt162 = 0;
    uint16_t adr = 0;
    uint16_t* curIndReg = &xRegister;
    if(ui->plainTextDisplay->hasFocus()){
        QPoint position = QCursor::pos();
        position.setX(position.x() - 4);
        QPoint localMousePos = ui->plainTextDisplay->mapFromGlobal(position);
        QTextCursor cursor = ui->plainTextDisplay->cursorForPosition(localMousePos);
        int x = cursor.position() % 55;
        if(x > 53) x = 53;
        int y = cursor.position() / 55;
        if(y > 19) y = 19;
        Memory[0xFFF2] = x;
        Memory[0xFFF3] = y;
        updateMemoryCell(0xFFF2);
        updateMemoryCell(0xFFF3);
    }
    if (!indexRegister) {
        //curIndReg = &yRegister;
    }
    switch(Memory[PC]){
    case 0x00:
        if (running){
            stopExecution();
        }else{
            PC++;
        }
        break;
    case 0x01:
        PC++;
        //NOP
        break;
    case 0x04:
        uInt8 = bReg & 0x01;
        uInt16 = (aReg << 8) + bReg;
        uInt16 = (uInt16 >> 1);
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateFlags(Negative,0);
        updateFlags(Zero, uInt16 == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateElement(regA);
        updateElement(regB);
        PC++;
        break;
    case 0x05:
        uInt8 = (bit(aReg, 7));
        uInt16 = (aReg << 8) + bReg;
        uInt16 = uInt16 << 1;
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, uInt16 == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateFlags(Carry, uInt8);
        updateElement(regA);
        updateElement(regB);
        PC++;
        break;
    case 0x06:
        updateFlags(HalfCarry, bit(aReg, 5));
        updateFlags(InterruptMask, bit(aReg, 4));
        updateFlags(Negative, bit(aReg, 3));
        updateFlags(Zero, bit(aReg, 2));
        updateFlags(Overflow, bit(aReg, 1));
        updateFlags(Carry, bit(aReg, 0));
        PC++;
        break;
    case 0x07:
        aReg = flags;
        updateElement(regA);
        PC++;
        break;
    case 0x08:
        (*curIndReg)++;
        updateFlags(Zero, (*curIndReg) == 0);
        updateElement(regX);
        PC++;
        break;
    case 0x09:
        (*curIndReg)--;
        updateFlags(Zero, (*curIndReg) == 0);
        updateElement(regX);
        PC++;
        break;
    case 0x0A:
        updateFlags(Overflow, 0);
        PC++;
        break;
    case 0x0B:
        updateFlags(Overflow, 1);
        PC++;
        break;
    case 0x0C:
        updateFlags(Carry, 0);
        PC++;
        break;
    case 0x0D:
        updateFlags(Carry, 1);
        PC++;
        break;
    case 0x0E:
        updateFlags(InterruptMask, 0);
        PC++;
        break;
    case 0x0F:
        updateFlags(InterruptMask, 1);
        PC++;
        break;
    case 0x10:
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7)  && bit(bReg, 7) && bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
        aReg = uInt8;
        updateElement(regA);
        PC++;
        break;
    case 0x11:
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7) && bit(bReg, 7) && bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
        PC++;
        break;
    case 0x16:
        bReg = aReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC++;
        break;
    case 0x17:
        aReg = bReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC++;
        break;
    case 0x19:
        uInt8 = flags & 0x01;
        uInt82 = bit(flags, 5);
        uInt16 = aReg >> 4;
        uInt162 = aReg & 0xF;
        if (uInt8 != 1 && uInt16 <= 9 && uInt82 != 1 && uInt162 <= 9){
            aReg+=0x0;
            updateFlags(Carry,0);
        }else if (uInt8 != 1 && uInt16 <= 8 && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (uInt8 != 1 && uInt16 <= 9 && uInt82 && uInt162 <= 3){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (uInt8 != 1 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 != 1 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (uInt8 != 1 && uInt16 >= 9 && uInt16 <= 0xF && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 != 1 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 && uInt162 <= 3){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 <= 2 && uInt82 != 1 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 <= 2 && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 <= 3 && uInt82 && uInt162 <= 3){
            aReg+=0x66;
            updateFlags(Carry,1);
        }
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x1B:
        uInt16 = aReg + bReg;
        updateFlags(Carry, (bit(aReg, 7) && bit(bReg, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(bReg, 7) && !bit(uInt16, 7)));
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(bReg, 3)) || (bit(aReg, 3) && !bit(uInt16, 3)) || (bit(bReg, 3) && !bit(uInt16, 3)));
        updateFlags(Overflow, (bit(aReg, 7) && bit(bReg, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(bReg, 7) && bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x20:
        sInt8 = Memory[(PC + 1) % 0x10000];
        PC += sInt8 + 2;
        break;
    case 0x21:
        PC += 2;
        break;
    case 0x22:
        if ((bit(flags, 2) || (flags & 0x01)) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x23:
        if (bit(flags, 2) || (flags & 0x01))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x24:
        if ((flags & 0x01) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x25:
        if ((flags & 0x01)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x26:
        if (bit(flags, 2) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x27:
        if (bit(flags, 2)) {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x28:
        if (bit(flags, 1) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x29:
        if (bit(flags, 1)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2A:
        if (bit(flags, 3) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2B:
        if (bit(flags, 3)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2C:
        if ((bit(flags, 3) ^ bit(flags, 1)) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2D:
        if (bit(flags, 3) ^ bit(flags, 1))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2E:
        if ((bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1))) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2F:
        if (bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1)))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x30:
        (*curIndReg) = SP + 1;
        updateElement(regX);
        PC++;
        break;
    case 0x31:
        SP++;
        updateElement(regSP);
        PC++;
        break;
    case 0x32:
        SP++;
        aReg = Memory[SP];
        updateElement(regSP);
        updateElement(regA);
        PC++;
        break;
    case 0x33:
        SP++;
        bReg = Memory[SP];
        updateElement(regSP);
        updateElement(regB);
        PC++;
        break;
    case 0x34:
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x35:
        SP = (*curIndReg) - 1;
        updateElement(regSP);
        PC++;
        break;
    case 0x36:
        Memory[SP] = aReg;
        updateMemoryCell(SP);
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x37:
        Memory[SP] = bReg;
        updateMemoryCell(SP);
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x38:
        SP++;
        xRegister = (Memory[SP] << 8);
        SP++;
        xRegister += Memory[SP];
        updateElement(regSP);
        updateElement(regX);
        PC++;
        break;
    case 0x39:
        SP++;
        PC = Memory[SP] << 8;
        SP++;
        PC += Memory[SP];
        updateElement(regSP);
        break;
    case 0x3A:
        xRegister = xRegister + bReg;
        PC++;
        updateElement(regX);
        break;
    case 0x3B:
        SP++;
        flags = Memory[SP];

        SP++;
        bReg = Memory[SP];

        SP++;
        aReg = Memory[SP];

        SP++;
        xRegister = (Memory[SP] << 8) + Memory[(SP+ 1) % 0x10000];

        SP+=2;
        PC = (Memory[SP] << 8) + Memory[(SP+ 1) % 0x10000];
        SP++;
        updateElement(regSP);
        updateElement(regA);
        updateElement(regB);
        updateElement(regX);
        updateElement(allFlags);
        break;
    case 0x3C:
        Memory[SP] = (xRegister & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((xRegister >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        updateElement(regSP);
        PC++;
        break;
    case 0x3D:
        uInt16 = static_cast<uint16_t>(aReg) * static_cast<uint16_t>(bReg);
        updateFlags(Carry, (uInt16 >> 8) != 0);
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC++;
        break;
    case 0x3E:
        if(lastInput != -1){
            PC++;
            lastInput = -1;
        }
        break;
    case 0x3F:
        PC++;
        Memory[SP] = PC & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = (PC >> 8) & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = xRegister & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = (xRegister >> 8) & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = aReg;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = bReg;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = flags;
        updateMemoryCell(SP);
        SP--;
        updateFlags(InterruptMask, 1);
        PC = (Memory[(interruptLocations - 5)] << 8) + Memory[(interruptLocations - 4)];
        updateElement(regSP);
        break;
    case 0x40:
        aReg = 0x0 - aReg;
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, aReg == 0x80);
        updateFlags(Carry, aReg != 0);
        updateElement(regA);
        PC++;
        break;
    case 0x43:
        aReg = 0xFF - aReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateElement(regA);
        PC++;
        break;
    case 0x44:
        uInt8 = (aReg & 0x1);
        aReg = (aReg >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateElement(regA);
        PC++;
        break;
    case 0x46:
        uInt8 = (aReg & 0x01);
        aReg = aReg >> 1;
        aReg += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x47:
        uInt8 = aReg & 0x01;
        updateFlags(Carry, uInt8);
        aReg = (aReg >> 1) + (aReg & 0x80);

        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x48:
        uInt8 = bit(aReg, 7);
        updateFlags(Carry, uInt8);
        aReg = aReg << 1;
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x49:
        uInt8 = bit(aReg, 7);
        aReg = (aReg << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
        updateElement(regA);
        PC++;
        break;
    case 0x4A:
        updateFlags(Overflow, aReg == 0x80);
        aReg--;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x4C:
        updateFlags(Overflow, aReg == 0x7F);
        aReg++;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x4D:
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC++;
        break;
    case 0x4F:
        aReg = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateElement(regA);
        PC++;
        break;
    case 0x50:
        bReg = 0x0 - bReg;
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, bReg == 0x80);
        updateFlags(Carry, bReg != 0);
        updateElement(regB);
        PC++;
        break;
    case 0x53:
        bReg = 0xFF - bReg;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateElement(regB);
        PC++;
        break;
    case 0x54:
        uInt8 = (bReg & 0x1);
        bReg = (bReg >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateElement(regB);
        PC++;
        break;
    case 0x56:
        uInt8 = (bReg & 0x01);
        bReg = bReg >> 1;
        bReg += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x57:
        uInt8 = bReg & 0x01;
        updateFlags(Carry, uInt8);
        bReg = (bReg >> 1) + (bReg & 0x80);
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x58:
        uInt8 = bit(bReg, 7);
        updateFlags(Carry, uInt8);
        bReg = bReg << 1;
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x59:
        uInt8 = bit(bReg, 7);
        bReg = (bReg << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
        updateElement(regB);
        PC++;
        break;
    case 0x5A:
        updateFlags(Overflow, bReg == 0x80);
        bReg--;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC++;
        break;
    case 0x5C:
        updateFlags(Overflow, bReg == 0x7F);
        bReg++;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC++;
        break;
    case 0x5D:
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC++;
        break;
    case 0x5F:
        bReg = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateElement(regB);
        PC++;
        break;
    case 0x60:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = 0x0 - Memory[adr];
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, Memory[adr] == 0x80);
        updateFlags(Carry, Memory[adr] != 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x63:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = 0xFF - Memory[adr];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x64:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = (Memory[adr] & 0x1);
        Memory[adr] = (Memory[adr] >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x66:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = (Memory[adr] & 0x01);
        Memory[adr] = Memory[adr] >> 1;
        Memory[adr] += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x67:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = (Memory[adr] & 0x01);
        updateFlags(Carry, uInt8);
        Memory[adr] = (Memory[adr] >> 1) + (Memory[adr] & 0x80);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x68:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = bit(Memory[adr], 7);
        updateFlags(Carry, uInt8);
        Memory[adr] = Memory[adr] << 1;
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x69:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt8 = bit(Memory[adr], 7);
        Memory[adr] = (Memory[adr] << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6A:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Overflow, Memory[adr] == 0x80);
        Memory[adr]--;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6C:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Overflow, Memory[adr] == 0x7F);
        Memory[adr]++;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6D:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC+=2;
        break;
    case 0x6E:
        PC = ((Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000);
        break;
    case 0x6F:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x70:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = 0x0 - Memory[adr];
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, Memory[adr] == 0x80);
        updateFlags(Carry, Memory[adr] != 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x73:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = 0xFF - Memory[adr];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x74:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = (Memory[adr] & 0x1);
        Memory[adr] = (Memory[adr] >> 1);
        updateFlags(Negative,0);
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,uInt8);
        updateFlags(Carry,uInt8);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x76:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = (Memory[adr] & 0x01);
        Memory[adr] = Memory[adr] >> 1;
        Memory[adr] += (flags & 0x01) << 7;
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x77:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = (Memory[adr] & 0x01);
        updateFlags(Carry, uInt8);
        Memory[adr] = (Memory[adr] >> 1) + (Memory[adr] & 0x80);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x78:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = bit(Memory[adr], 7);
        updateFlags(Carry, uInt8);
        Memory[adr] = Memory[adr] << 1;
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x79:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt8 = bit(Memory[adr], 7);
        Memory[adr] = (Memory[adr] << 1) + (flags & 0x01);
        updateFlags(Carry, uInt8);
        updateFlags(Negative,bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7A:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Overflow, Memory[adr] == 0x80);
        Memory[adr]--;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7C:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Overflow, Memory[adr] == 0x7F);
        Memory[adr]++;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7D:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC+=3;
        break;
    case 0x7E:
        PC = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        break;
    case 0x7F:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x80:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x81:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0x82:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x83:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt16 = (aReg << 8) + bReg;
        uInt162 = uInt16 - adr;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(adr,15) && !bit(uInt162,15)) || (!bit(aReg,7) && bit(adr,15) && bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) && bit(adr,15)) || (bit(adr,15) && bit(uInt162,15)) || (bit(uInt162,15) && !bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=3;
        break;
    case 0x84:
        aReg = (aReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x85:
        uInt8 = (aReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0x86:
        aReg = Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x88:
        aReg = aReg ^ Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x89:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8A:
        aReg = aReg | Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8B:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8C:
        uInt16 = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=3;
        break;
    case 0x8D:
        sInt8 = Memory[(PC+1) % 0x10000];
        PC+=2;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC += sInt8;
        updateElement(regSP);
        break;
    case 0x8E:
        SP = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regSP);
        break;
    case 0x90:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x91:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0x92:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x93:
        uInt8 = Memory[(PC+1) % 0x10000];
        adr = (Memory[uInt8] << 8) + Memory[uInt8 + 1];
        uInt16 = (aReg << 8) + bReg;
        uInt162 = uInt16 - adr;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(adr,15) && !bit(uInt162,15)) || (!bit(aReg,7) && bit(adr,15) && bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) && bit(adr,15)) || (bit(adr,15) && bit(uInt162,15)) || (bit(uInt162,15) && !bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=2;
        break;
    case 0x94:
        aReg = (aReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x95:
        uInt8 = (aReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0x96:
        aReg = Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x97:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x98:
        aReg = aReg ^ Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x99:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9A:
        aReg = aReg | Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9B:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9C:
        adr = Memory[(PC+1) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=2;
        break;
    case 0x9D:
        adr = Memory[(PC+1) % 0x10000];
        PC+=2;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC = adr;
        updateElement(regSP);
        break;
    case 0x9E:
        adr = Memory[(PC+1) % 0x10000];
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regSP);
        break;
    case 0x9F:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = SP >> 8;
        Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xA0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0xA1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0xA2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0xA3:
        uInt8 = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        adr = (Memory[uInt8] << 8) + Memory[uInt8 + 1];
        uInt16 = (aReg << 8) + bReg;
        uInt162 = uInt16 - adr;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(adr,15) && !bit(uInt162,15)) || (!bit(aReg,7) && bit(adr,15) && bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) && bit(adr,15)) || (bit(adr,15) && bit(uInt162,15)) || (bit(uInt162,15) && !bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=2;
        break;
    case 0xA4:
        aReg = (aReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA5:
        uInt8 = (aReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xA6:
        aReg = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA7:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xA8:
        aReg = aReg ^ Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAA:
        aReg = aReg | Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAC:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=2;
        break;
    case 0xAD:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        PC+=2;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC = adr;
        updateElement(regSP);
        break;
    case 0xAE:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regSP);
        break;
    case 0xAF:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = SP >> 8;
        Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xB0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=3;
        updateElement(regA);
        break;
    case 0xB1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=3;
        break;
    case 0xB2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        aReg = uInt82;
        PC+=3;
        updateElement(regA);
        break;
    case 0xB3:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        adr = (Memory[adr] << 8) + Memory[adr + 1];
        uInt16 = (aReg << 8) + bReg;
        uInt162 = uInt16 - adr;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(adr,15) && !bit(uInt162,15)) || (!bit(aReg,7) && bit(adr,15) && bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) && bit(adr,15)) || (bit(adr,15) && bit(uInt162,15)) || (bit(uInt162,15) && !bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=3;
        break;
    case 0xB4:
        aReg = (aReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB5:
        uInt8 = (aReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=3;
        break;
    case 0xB6:
        aReg = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB7:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0xB8:
        aReg = aReg ^ Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = aReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBA:
        aReg = aReg | Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = aReg + uInt8;
        updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        aReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBC:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=3;
        break;
    case 0xBD:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        PC+=3;
        Memory[SP] = (PC & 0xFF);
        updateMemoryCell(SP);
        Memory[SP - 1] = ((PC >> 8) & 0xFF);
        updateMemoryCell(SP-1);
        SP-=2;
        PC = adr;
        updateElement(regSP);
        break;
    case 0xBE:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regSP);
        break;
    case 0xBF:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = SP >> 8;
        Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=3;
        break;
    case 0xC0:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xC1:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xC2:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xC3:
        uInt16 = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt162 = (aReg << 8) + bReg;
        uInt162 = uInt162 + uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && bit(uInt16,15) && !bit(uInt162,12)) || (!bit(aReg,7) && !bit(uInt16,15) && bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) && bit(uInt16,15)) || (bit(uInt16,15) && !bit(uInt162,12)) || (!bit(uInt162,12) && bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=3;
        break;
    case 0xC4:
        bReg = (bReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC5:
        uInt8 = (bReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xC6:
        bReg = Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC8:
        bReg = bReg ^ Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC9:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCA:
        bReg = bReg | Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCB:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCC:
        uInt16 = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(uInt16,15));
        updateFlags(Zero, uInt16 == 0);
        updateFlags(Overflow, 0);
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=3;
        break;
    case 0xCE:
        (*curIndReg) = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regX);
        break;
    case 0xD0:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xD1:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xD2:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xD3:
        adr = Memory[(PC+1) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (aReg << 8) + bReg;
        uInt162 = uInt162 + uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && bit(uInt16,15) && !bit(uInt162,12)) || (!bit(aReg,7) && !bit(uInt16,15) && bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) && bit(uInt16,15)) || (bit(uInt16,15) && !bit(uInt162,12)) || (!bit(uInt162,12) && bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD4:
        bReg =( bReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD5:
        uInt8 = (bReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xD6:
        bReg = Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD7:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xD8:
        bReg = bReg ^ Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD9:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDA:
        bReg = bReg | Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDB:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDC:
        adr = Memory[(PC+1) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(uInt16,15));
        updateFlags(Zero, uInt16 == 0);
        updateFlags(Overflow, 0);
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDD:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = aReg;
        Memory[adr+1] = bReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, bReg + aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell(adr+1);
        PC+=2;
        break;
    case 0xDE:
        adr = Memory[(PC+1) % 0x10000];
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regX);
        break;
    case 0xDF:
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = (*curIndReg) >> 8;
        Memory[(adr + 1) % 0x10000] = ((*curIndReg) & 0xFF);
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xE0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xE1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xE2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xE3:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (aReg << 8) + bReg;
        uInt162 = uInt162 + uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && bit(uInt16,15) && !bit(uInt162,12)) || (!bit(aReg,7) && !bit(uInt16,15) && bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) && bit(uInt16,15)) || (bit(uInt16,15) && !bit(uInt162,12)) || (!bit(uInt162,12) && bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE4:
        bReg = (bReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE5:
        uInt8 = (bReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xE6:
        bReg = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE7:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xE8:
        bReg = bReg ^ Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEA:
        bReg = bReg | Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEC:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(uInt16,15));
        updateFlags(Zero, uInt16 == 0);
        updateFlags(Overflow, 0);
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=2;
        break;
    case 0xED:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = aReg;
        Memory[adr+1] = bReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, bReg + aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell(adr+1);
        PC+=2;
        break;
    case 0xEE:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regX);
        break;
    case 0xEF:
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = (*curIndReg) >> 8;
        Memory[(adr + 1) % 0x10000] = ((*curIndReg) & 0xFF);
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=2;
        break;
    case 0xF0:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=3;
        updateElement(regB);
        break;
    case 0xF1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=3;
        break;
    case 0xF2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        bReg = uInt82;
        PC+=3;
        updateElement(regB);
        break;
    case 0xF3:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (aReg << 8) + bReg;
        uInt162 = uInt162 + uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit(aReg,7) && bit(uInt16,15) && !bit(uInt162,12)) || (!bit(aReg,7) && !bit(uInt16,15) && bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) && bit(uInt16,15)) || (bit(uInt16,15) && !bit(uInt162,12)) || (!bit(uInt162,12) && bit(aReg,7)));
        aReg = (uInt162 >> 8);
        bReg = (uInt162 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF4:
        bReg = (bReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF5:
        uInt8 = (bReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=3;
        break;
    case 0xF6:
        bReg = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF7:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0xF8:
        bReg = bReg ^ Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF9:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = bReg + uInt8 + (flags & 0x01);
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFA:
        bReg = bReg | Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFB:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt16 = bReg + uInt8;
        updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
        bReg = static_cast<uint8_t>(uInt16);
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFC:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(uInt16,15));
        updateFlags(Zero, uInt16 == 0);
        updateFlags(Overflow, 0);
        aReg = (uInt16 >> 8);
        bReg = (uInt16 & 0xFF);
        updateElement(regA);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFD:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = aReg;
        Memory[adr+1] = bReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, bReg + aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell(adr+1);
        PC+=3;
        break;
    case 0xFE:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regX);
        break;
    case 0xFF:
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = (*curIndReg) >> 8;
        Memory[(adr + 1) % 0x10000] = ((*curIndReg) & 0xFF);
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        updateMemoryCell((adr + 1) % 0x10000);
        PC+=3;
        break;
    default:
        PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
        PC++;
        break;
    }

    PC = PC % 0x10000;
    updateElement(regPC);
    return true;
}
