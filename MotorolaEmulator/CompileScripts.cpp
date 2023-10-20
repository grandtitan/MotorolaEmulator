#include "mainwindow.h"
#include "instructionblock.h"
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
int getNum(const QString& input){
    bool ok;
    int number;
    if (input.startsWith('$')) {
        QString hexValue = input.sliced(1);
        number = hexValue.toUInt(&ok, 16);
        if (!ok) {
            throw std::invalid_argument("Invalid hexadecimal number: " + input.toStdString());
        }
    }
    else if (input.startsWith('%')) {
        QString binaryValue = input.sliced(1);
        number = binaryValue.toUInt(&ok, 2);
        if (!ok) {
            throw std::invalid_argument("Invalid binary number: " + input.toStdString());
        }
    }
    else {
        number = input.toUInt(&ok);
        if (!ok) {
            throw std::invalid_argument("Invalid decimal number: " + input.toStdString());
        }
    }
    return number;
}


bool MainWindow::compileMix(int ver){
	compiled = 0;
    ui->buttonCompile->setStyleSheet("");
    ui->buttonCompile->setStyleSheet(compiledButton);
    PrintConsole("", 2);
    currentLine = 0;
    currentAddress = 0;
    std::fill(std::begin(Memory), std::end(Memory), static_cast<uint8_t>(0));
    std::fill(std::begin(backupMemory), std::end(backupMemory), static_cast<uint8_t>(0));
    labelValMap.clear();
    callLabelMap.clear();
    callLabelRazMap.clear();
    callLabelRelMap.clear();
    clearInstructions();
    QString code = ui->plainTextCode->toPlainText();
    QStringList lines = code.split("\n");
    int charNum = 0;
	foreach (QString line, lines) {
        int instructionAddress = currentAddress;
        QString label;
        QString in;
        QString op;
        charNum = 0;
        int inCode = 0;
        int opCode = 0;
        int opCode2 = 0;
        if (line.contains(";")) {
            line = line.split(";")[0];
            for (int var = line.length(); var > 0; --var) {
                if (line.endsWith(' ')) {
                    line.chop(1);
                }
                else {
                    break;
                }
            }
        }
    labelExtraction:
        if (line.isEmpty()) {
            PrintConsole("Empty line", -1);
            goto skipLine;
        }
        else if (line[0].isLetter()) {
            //extract lable
            charNum++;
            for (; charNum < line.size(); ++charNum) {
                if (line[charNum].isLetterOrNumber() || line[charNum] == '_') {
                    if (charNum == line.size() - 1) {
                        Err("Missing instruction");
                        goto end;
                    }
                }
                else if (line[charNum] == '\t') {
                    label = line.sliced(0, charNum).toUpper();
                    charNum++;
                    for (; charNum < line.size(); ++charNum) {
                        if (line[charNum] == '\t') {
                            if (charNum == line.size() - 1) {
                                Err("Missing instruction");
                                goto end;
                            }
                        }
                        else {
                            goto lineDisection;
                        }
                    }
                }
                else if (line[charNum] == ' ') {
                    Err("Label may not contain a space and must be followed by a tab.");
                    goto end;
                }
                else {
                    Err("Label may not contain: '" % line[charNum] % "'");
                    goto end;
                }
            }
        }
        else if (line[0] == '\t') {
            charNum++;
            for (; charNum < line.size(); ++charNum) {
                if (line[charNum] == '\t') {
                    if (charNum == line.size() - 1) {
                        Err("Missing instruction");
                        goto end;
                    }
                }
                else {
                    break;
                }
            }
            //no label
        }else if (line[0].isDigit()){
            Err("Label may not start with a number: '" % line[0] % "'");
            goto end;
        }
        else {
            Err("Line may not start with: '" % line[0] % "'");
            goto end;
        }
    lineDisection:
        if (line.sliced(charNum).isEmpty()) {
            Err("Missing instruction");
            goto end;
        }
        else if (line[charNum].isLetter() || line[charNum] == '.') {
            charNum++;
            if (line.sliced(charNum).isEmpty()) {
                Err("Missing instruction");
                goto end;
            }
            for (int start = charNum - 1; charNum < line.size(); ++charNum) {
                if (line[charNum].isLetter()) {
                    if (charNum == line.size() - 1) {
                        in = line.sliced(start).toUpper();
                        goto operationIdentification;
                    }
                }
                else if (line[charNum] == ' ') {
                    in = (line.sliced(start, charNum - start)).toUpper();
                    charNum++;
                    break;
                }
                else {
                    Err("Unexpected character: '" % line[charNum] % "'");
                    goto end;
                }
            }
            if (line.sliced(charNum).isEmpty()) {
                charNum--;
                Err("Unexpected space or missing operand");
                goto end;
            }
            op = line.sliced(charNum);
            if (line[charNum] == '#') {
                if (op.contains(",")) {
                    Err("Immediate and indexed data cannot be mixed");
                    goto end;
                }
                charNum++;
                if (line.sliced(charNum).isEmpty()) {
                    charNum--;
                    Err("Invalid operand");
                    goto end;
                }
                if (line[charNum].isLetter()) {
                    charNum++;
                    for (; charNum < line.size(); ++charNum) {
                        if (line[charNum].isLetterOrNumber() || line[charNum] == '_') {
                        }
                        else {
                            Err("Unexpected character: '" % line[charNum] % "'");
                            goto end;
                        }
                    }
                }
                else if (line[charNum] == '%' || line[charNum].isDigit()) {
                    charNum++;
                    for (; charNum < line.size(); ++charNum) {
                        if (line[charNum].isDigit()) {
                        }
                        else {
                            Err("Unexpected character: '" % line[charNum] % "'");
                            goto end;
                        }
                    }
                }
                else if (line[charNum] == '$') {
                    charNum++;
                    if (line.sliced(charNum).isEmpty()) {
                        Err("Missing operand");
                        goto end;
                    }
                    for (; charNum < line.size(); ++charNum) {
                        if (line[charNum].isLetterOrNumber()) {
                        }
                        else {
                            Err("Unexpected character: '" % line[charNum] % "'");
                            goto end;
                        }
                    }
                } else if (line[charNum] == '\''){
                    charNum++;
                    if(line.sliced(charNum).isEmpty()){
                        Err("Missing character");
                        goto end;
                    }
                    if(!op.sliced(3).isEmpty()){
                        charNum++;
                        Err("Cannot convert multiple characters");
                        goto end;
                    }
                    ushort unicodeValue = line[charNum].unicode();
                    if (unicodeValue <= 128) {
                        op = "#" + QString::number(static_cast<int>(unicodeValue));

                    } else {
                        Err("Invlaid ASCII character: '" % line[charNum] % "'");
                        goto end;
                    }
                }
                else {
                    Err("Unexpected character: '" % line[charNum] % "'");
                    goto end;
                }
            }
            else if (line[charNum].isLetter()) {
                charNum++;
                for (; charNum < line.size(); ++charNum) {
                    if (line.size() - 2 == charNum) {
                        if (line[charNum] == ',') {
                            if (line[charNum + 1].toUpper() == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
                                charNum++;
                                goto end;
                            }
                        }
                    }
                    else if (line[charNum].isLetterOrNumber() || line[charNum] == '_') {
                    }
                    else {
                        Err("Unexpected character: '" % line[charNum] % "'");
                        goto end;
                    }
                }
            }
            else if (line[charNum] == '%' || line[charNum] == '-' || line[charNum].isDigit()) {
                charNum++;
                for (; charNum < line.size(); ++charNum) {
                    if (line.size() - 2 == charNum) {
                        if (line[charNum] == ',') {
                            if (line[charNum + 1].toUpper() == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
                                charNum++;
                                goto end;
                            }
                        }
                    }
                    else if (line[charNum].isDigit()) {
                    }
                    else {
                        Err("Unexpected character: '" % line[charNum] % "'");
                        goto end;
                    }
                }
            }
            else if (line[charNum] == '$') {
                charNum++;
                if (line.sliced(charNum).isEmpty()) {
                    Err("Missing operand");
                    goto end;
                }
                for (; charNum < line.size(); ++charNum) {
                    if (line.size() - 2 == charNum) {
                        if (line[charNum] == ',') {
                            if (line[charNum + 1].toUpper() == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
                                charNum++;
                                goto end;
                            }
                        }
                    }
                    else if (line[charNum].isLetterOrNumber()) {
                    }
                    else {
                        Err("Unexpected character: '" % line[charNum] % "'");
                        goto end;
                    }
                }
            } else if (line[charNum] == '\''){
                charNum++;
                if(line.sliced(charNum).isEmpty()){
                    Err("Missing character");
                    goto end;
                }
                if(!op.sliced(2).isEmpty()){
                    charNum++;
                    Err("Cannot convert multiple characters");
                    goto end;
                }
                ushort unicodeValue = line[charNum].unicode();
                if (unicodeValue <= 128) {
                    op = QString::number(static_cast<int>(unicodeValue));
                } else {
                    Err("Invlaid ASCII character");
                    goto end;
                }

            }
            else {
                Err("unexpected character: '" % line[charNum] % "'");
                goto end;
            }
        }
        else {
            Err("Unexpected character: '" % line[charNum] % "'");
            goto end;
        }
	operationIdentification:
        op = op.toUpper();
        charNum = 1000;
		if(ver == 0){
            if (allInstructionsM6800.indexOf(in) == -1 && specialInstructions.indexOf(in) == -1) {
				Err("Unknown instruction");
				goto end;
			}
		}else if(ver == 1){
            if (allInstructionsM6803.indexOf(in) == -1 && specialInstructions.indexOf(in) == -1) {
				Err("Unknown instruction");
				goto end;
			}
		}
		if (in[0] == '.') {
            if (in == ".BYTE") {
                if (op == "") {
                    Err("Missing operand");
                    goto end;
                }
                else {
                    if (op[0].isLetter()) {
                        Err("Cannot use label in this directive");
                        goto end;
                    }
                    else {
                        int value = 0;
                        try {
                            value = getNum(op);
                        }
                        catch (...) {
                            Err("Invalid number: '" % op % "'");
                            goto end;
                        }
                        if (value > 255) {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                        Memory[currentAddress] = value;
                        if(label != ""){
                            if (labelValMap.count(label) == 0) {
                                labelValMap[label] = currentAddress;
                            } else {
                                Err("Label already declared: '" + label + "'");
                                goto end;
                            }
                        }


                        PrintConsole("Assigned:'" + QString::number(currentAddress) + "' to:'" + label + "'", -1);
                        currentAddress++;
                        goto skipLine;
                    }
                }
            }
            else if (in == ".EQU") {
                if (label == "") {
                    Err("Missing label");
                    goto end;
                }
                else if (op == "") {
                    Err("Missing operand");
                    goto end;
                }
                else {
                    if (op[0].isLetter()) {
                        Err("Cannot use label in this directive");
                        goto end;
                    }
                    else {
                        int value = 0;
                        try {
                            value = getNum(op);
                        }
                        catch (...) {
                            Err("Invalid number: '" % op % "'");
                            goto end;
                        }
                        if (value > 0x10000) {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                        if (labelValMap.count(label) == 0) {
                            labelValMap[label] = value;
                        }
                        else {
                            Err("Label already declared: '" + label + "'");
                            goto end;
                        }
                        PrintConsole("Assigned:'" + QString::number(value) + "' to:'" + label + "'", -1);
                        goto skipLine;
                    }
                }
            }
            else if (in == ".ORG"){
                if (label != "") {
                    Err("Cannot label this instruction");
                    goto end;
                }
                else if (op == "") {
                    Err("Missing operand");
                    goto end;
                }
                else {
                    if (op[0].isLetter()) {
                        Err("Cannot use label in this directive");
                        goto end;
                    }
                    else {
                        int value = 0;
                        try {
                            value = getNum(op);
                        }
                        catch (...) {
                            Err("Invalid number: '" % op % "'");
                            goto end;
                        }
                        if (value > 0x10000) {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                        if (value < currentAddress){
                            Err("Backward reference not permitted: " + QString::number(value));
                            goto end;
                        }
                        currentAddress = value;
                        goto skipLine;
                    }
                }
            }
            else {
                Err("Unknown instruction");
                goto end;
            }
        }
        if(ver == 0){
            if (vseInstructionsM6800.indexOf(in) != -1) { //VSE
                if (op != "") {
                    Err("Unexpected operand");
                    goto end;
                }
                if (true) {
                    if (in == "ABA") {
                        inCode = 0x1B;
                    }
                    else if (in == "ASLA") {
                        inCode = 0x48;
                    }
                    else if (in == "ASLB") {
                        inCode = 0x58;
                    }
                    else if (in == "ASRA") {
                        inCode = 0x47;
                    }
                    else if (in == "ASRB") {
                        inCode = 0x57;
                    }
                    else if (in == "CBA") {
                        inCode = 0x11;
                    }
                    else if (in == "CLC") {
                        inCode = 0x0C;
                    }
                    else if (in == "CLI") {
                        inCode = 0x0E;
                    }
                    else if (in == "CLRA") {
                        inCode = 0x4F;
                    }
                    else if (in == "CLRB") {
                        inCode = 0x5F;
                    }
                    else if (in == "CLV") {
                        inCode = 0x0A;
                    }
                    else if (in == "COMA") {
                        inCode = 0x43;
                    }
                    else if (in == "COMB") {
                        inCode = 0x53;
                    }
                    else if (in == "DAA") {
                        inCode = 0x19;
                    }
                    else if (in == "DECA") {
                        inCode = 0x4A;
                    }
                    else if (in == "DECB") {
                        inCode = 0x5A;
                    }
                    else if (in == "DES") {
                        inCode = 0x34;
                    }
                    else if (in == "DEX") {
                        inCode = 0x09;
                    }
                    else if (in == "INCA") {
                        inCode = 0x4C;
                    }
                    else if (in == "INCB") {
                        inCode = 0x5C;
                    }
                    else if (in == "INS") {
                        inCode = 0x31;
                    }
                    else if (in == "INX") {
                        inCode = 0x08;
                    }
                    else if (in == "LSRA") {
                        inCode = 0x44;
                    }
                    else if (in == "LSRB") {
                        inCode = 0x54;
                    }
                    else if (in == "NEGA") {
                        inCode = 0x40;
                    }
                    else if (in == "NEGB") {
                        inCode = 0x50;
                    }
                    else if (in == "NOP") {
                        inCode = 0x01;
                    }
                    else if (in == "PSHA") {
                        inCode = 0x36;
                    }
                    else if (in == "PSHB") {
                        inCode = 0x37;
                    }
                    else if (in == "PULA") {
                        inCode = 0x32;
                    }
                    else if (in == "PULB") {
                        inCode = 0x33;
                    }
                    else if (in == "ROLA") {
                        inCode = 0x49;
                    }
                    else if (in == "ROLB") {
                        inCode = 0x59;
                    }
                    else if (in == "RORA") {
                        inCode = 0x46;
                    }
                    else if (in == "RORB") {
                        inCode = 0x56;
                    }
                    else if (in == "RTI") {
                        inCode = 0x3B;
                    }
                    else if (in == "RTS") {
                        inCode = 0x39;
                    }
                    else if (in == "SBA") {
                        inCode = 0x10;
                    }
                    else if (in == "SEC") {
                        inCode = 0x0D;
                    }
                    else if (in == "SEI") {
                        inCode = 0x0F;
                    }
                    else if (in == "SEV") {
                        inCode = 0x0B;
                    }
                    else if (in == "SWI") {
                        inCode = 0x3F;
                    }
                    else if (in == "TAB") {
                        inCode = 0x16;
                    }
                    else if (in == "TAP") {
                        inCode = 0x06;
                    }
                    else if (in == "TBA") {
                        inCode = 0x17;
                    }
                    else if (in == "TPA") {
                        inCode = 0x07;
                    }
                    else if (in == "TSTA") {
                        inCode = 0x4D;
                    }
                    else if (in == "TSTB") {
                        inCode = 0x5D;
                    }
                    else if (in == "TSX") {
                        inCode = 0x30;
                    }
                    else if (in == "TXS") {
                        inCode = 0x35;
                    }
                    else if (in == "WAI") {
                        inCode = 0x3E;
                    }
                    else {
                        Err("Unknown instruction");
                        goto end;
                    }
                }
                if (!label.isEmpty()) {
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentAddress) + "' to:'" + label + "'", -1);
                }
                Memory[currentAddress] = inCode;
                currentAddress += 1;
                goto skipLine;
            }
            else {
                if (op.isEmpty()) {
                    Err("Missing operand");
                    goto end;
                }
                if (!label.isEmpty()) {
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentAddress) + "' to:'" + label + "'", -1);
                }
            }
            if (relInstructionsM6800.indexOf(in) != -1) { //REL
                if (op.startsWith("#")) {
                    Err("Instruction cannot take immediate data");
                    goto end;
                }
                if (op.contains(",")) {
                    Err("Instruction cannot take indexed data");
                    goto end;
                }
                if (true) {
                    if (in == "BCC") {
                        inCode = 0x24;
                    }
                    else if (in == "BCS") {
                        inCode = 0x25;
                    }
                    else if (in == "BEQ") {
                        inCode = 0x27;
                    }
                    else if (in == "BGE") {
                        inCode = 0x2C;
                    }
                    else if (in == "BGT") {
                        inCode = 0x2E;
                    }
                    else if (in == "BHI") {
                        inCode = 0x22;
                    }
                    else if (in == "BLE") {
                        inCode = 0x2F;
                    }
                    else if (in == "BLS") {
                        inCode = 0x23;
                    }
                    else if (in == "BLT") {
                        inCode = 0x2D;
                    }
                    else if (in == "BMI") {
                        inCode = 0x2B;
                    }
                    else if (in == "BNE") {
                        inCode = 0x26;
                    }
                    else if (in == "BPL") {
                        inCode = 0x2A;
                    }
                    else if (in == "BRA") {
                        inCode = 0x20;
                    }
                    else if (in == "BSR") {
                        inCode = 0x8D;
                    }
                    else if (in == "BVC") {
                        inCode = 0x28;
                    }
                    else if (in == "BVS") {
                        inCode = 0x29;
                    }
                    else {
                        Err("Unknown instruction");
                        goto end;
                    }
                }
                if (op == "") {
                    Err("Missing operand");
                    goto end;
                }
                bool ok;
                if (op[0].isLetter()) {
                    callLabelRelMap[currentAddress + 1] = op;
                    opCode = 0;
                }
                else if (op.startsWith('$')) {
                    QString hexValue = op.sliced(1);
                    opCode = hexValue.toInt(&ok, 16);
                    if (!ok) {
                        Err("Invalid hexadecimal number: " + op);
                        goto end;
                    }
                    if (opCode > 0xFD) {
                        Err("Relative address out of range[-128,127]: " + hexValue);
                        goto end;
                    }
                }
                else if (op.startsWith('%')) {
                    QString binaryValue = op.sliced(1);
                    opCode = binaryValue.toInt(&ok, 2);
                    if (!ok) {
                        Err("Invalid binary number: " + op);
                        goto end;
                    }
                    if (opCode > 0xFD) {
                        Err("Relative address out of range[-128,127]: " + binaryValue);
                        goto end;
                    }
                }
                else {
                    opCode = op.toInt(&ok);
                    if (!ok) {
                        Err("Invalid decimal number: " + op);
                        goto end;
                    }
                    if (opCode < 0)
                        opCode -= 2;
                    if (opCode > 127 || opCode < -128) {
                        Err("Relative address out of range[-128,127]: " + QString::number(opCode));
                        goto end;
                    }
                    qint8 signedValue = static_cast<qint8>(opCode);
                    opCode = signedValue & 0xFF;
                }
                Memory[currentAddress] = inCode;
                Memory[currentAddress + 1] = opCode;
                currentAddress += 2;
            }
            else if (op.contains(",")) { // IND
                if (!indInstructionsM6800.contains(in)) {
                    Err("Instruction does not take indexed data");
                    goto end;
                }
                op.chop(2);
                if (op[0].isLetter()) {
                    if(labelValMap.count(op) == 0){
                        opCode = 0;
                        callLabelMap[currentAddress + 1] = op;
                    }else{
                        int value = labelValMap[op];
                        if (value > 255){
                            PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentAddress)+"' is out of instructions range. Entered last byte",1);
                            value = value & 0xFF;
                            opCode = value;
                        }else{
                            value = value & 0xFF;
                            opCode = value;
                        }
                    }
                }
                else {
                    int value = 0;
                    try {
                        value = getNum(op);
                    }
                    catch (...) {
                        Err("Invalid number: " + op);
                        goto end;
                    }
                    if (value > 255) {
                        Err("Value out of range: " + QString::number(value));
                        goto end;
                    }
                    opCode = value;
                }
                if (true) {
                    if (in == "ADCA") {
                        inCode = 0xA9;
                    }
                    else if (in == "ADCB") {
                        inCode = 0xE9;
                    }
                    else if (in == "ADDA") {
                        inCode = 0xAB;
                    }
                    else if (in == "ADDB") {
                        inCode = 0xEB;
                    }
                    else if (in == "ANDA") {
                        inCode = 0xA4;
                    }
                    else if (in == "ANDB") {
                        inCode = 0xE4;
                    }
                    else if (in == "ASL") {
                        inCode = 0x68;
                    }
                    else if (in == "ASR") {
                        inCode = 0x67;
                    }
                    else if (in == "BITA") {
                        inCode = 0xA5;
                    }
                    else if (in == "BITB") {
                        inCode = 0xE5;
                    }
                    else if (in == "CLR") {
                        inCode = 0x6F;
                    }
                    else if (in == "CMPA") {
                        inCode = 0xA1;
                    }
                    else if (in == "CMPB") {
                        inCode = 0xE1;
                    }
                    else if (in == "COM") {
                        inCode = 0x63;
                    }
                    else if (in == "CPX") {
                        inCode = 0xAC;
                    }
                    else if (in == "DEC") {
                        inCode = 0x6A;
                    }
                    else if (in == "EORA") {
                        inCode = 0xA8;
                    }
                    else if (in == "EORB") {
                        inCode = 0xE8;
                    }
                    else if (in == "INC") {
                        inCode = 0x6C;
                    }
                    else if (in == "JMP") {
                        inCode = 0x6E;
                    }
                    else if (in == "JSR") {
                        inCode = 0xAD;
                    }
                    else if (in == "LDAA") {
                        inCode = 0xA6;
                    }
                    else if (in == "LDAB") {
                        inCode = 0xE6;
                    }
                    else if (in == "LDS") {
                        inCode = 0xAE;
                    }
                    else if (in == "LDX") {
                        inCode = 0xEE;
                    }
                    else if (in == "LSR") {
                        inCode = 0x64;
                    }
                    else if (in == "NEG") {
                        inCode = 0x60;
                    }
                    else if (in == "ORAA") {
                        inCode = 0xAA;
                    }
                    else if (in == "ORAB") {
                        inCode = 0xEA;
                    }
                    else if (in == "ROL") {
                        inCode = 0x69;
                    }
                    else if (in == "ROR") {
                        inCode = 0x66;
                    }
                    else if (in == "SBCA") {
                        inCode = 0xA2;
                    }
                    else if (in == "SBCB") {
                        inCode = 0xE2;
                    }
                    else if (in == "STAA") {
                        inCode = 0xA7;
                    }
                    else if (in == "STAB") {
                        inCode = 0xE7;
                    }
                    else if (in == "STS") {
                        inCode = 0xAF;
                    }
                    else if (in == "STX") {
                        inCode = 0xEF;
                    }
                    else if (in == "SUBA") {
                        inCode = 0xA0;
                    }
                    else if (in == "SUBB") {
                        inCode = 0xE0;
                    }
                    else if (in == "TST") {
                        inCode = 0x6D;
                    }
                    else {
                        Err("Unknown instruction");
                        goto end;
                    }
                }
                Memory[currentAddress] = inCode;
                Memory[currentAddress + 1] = opCode;
                currentAddress += 2;
            }
            else if (op.startsWith("#")) {
                op = op.sliced(1);
                opCode = 0;
                opCode2 = 0;
                if (op[0].isLetter()) { //TAK LAB
                    if (takRazInstructionsM6800.indexOf(in) != -1) { // TAK RAZ LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                opCode = (value >> 8) & 0xFF;
                                opCode2 = value & 0xFF;
                            }else{
                                opCode = value;
                            }
                        }
                        Memory[currentAddress + 1] = opCode;
                        Memory[currentAddress + 2] = opCode2;
                        if (true) {
                            if (in == "CPX") {
                                inCode = 0x8C;
                            }
                            else if (in == "LDS") {
                                inCode = 0x8E;
                            }
                            else if (in == "LDX") {
                                inCode = 0xCE;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        Memory[currentAddress] = inCode;
                        currentAddress += 3;
                    }
                    else if (takInstructionsM6800.contains(in)) { //tak LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentAddress)+"' is out of instructions range. Entered last byte",1);
                                value = value & 0xFF;
                                opCode = value;
                            }else{
                                value = value & 0xFF;
                                opCode = value;
                            }
                        }
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0x89;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xC9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0x8B;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xCB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0x84;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xC4;
                            }
                            else if (in == "BITA") {
                                inCode = 0x85;
                            }
                            else if (in == "BITB") {
                                inCode = 0xC5;
                            }
                            else if (in == "CMPA") {
                                inCode = 0x81;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xC1;
                            }
                            else if (in == "EORA") {
                                inCode = 0x88;
                            }
                            else if (in == "EORB") {
                                inCode = 0xC8;
                            }
                            else if (in == "LDAA") {
                                inCode = 0x86;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xC6;
                            }
                            else if (in == "ORAA") {
                                inCode = 0x8A;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xCA;
                            }
                            else if (in == "SBCA") {
                                inCode = 0x82;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xC2;
                            }
                            else if (in == "SUBA") {
                                inCode = 0x80;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xC0;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        currentAddress += 2;
                    }
                    else {
                        Err("Instruction does not take immediate data");
                        goto end;
                    }
                }
                else {
                    int value = 0;
                    try {
                        value = getNum(op);
                    }
                    catch (...) {
                        Err("Invalid number: " + op);
                        goto end;
                    }
                    if (takRazInstructionsM6800.indexOf(in) != -1) { // TAK RAZ
                        if (value < 0x10000) {
                            opCode = (value >> 8) & 0xFF;
                            opCode2 = value & 0xFF;
                            Memory[currentAddress + 1] = opCode;
                            Memory[currentAddress + 2] = opCode2;
                            if (true) {
                                if (in == "CPX") {
                                    inCode = 0x8C;
                                }
                                else if (in == "LDS") {
                                    inCode = 0x8E;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xCE;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            Memory[currentAddress] = inCode;
                            currentAddress += 3;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else if (takInstructionsM6800.contains(in)) { //tak
                        if (value < 256) {
                            opCode = value;
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0x89;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xC9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0x8B;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xCB;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0x84;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xC4;
                                }
                                else if (in == "BITA") {
                                    inCode = 0x85;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xC5;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0x81;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xC1;
                                }
                                else if (in == "EORA") {
                                    inCode = 0x88;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xC8;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0x86;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xC6;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0x8A;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xCA;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0x82;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xC2;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0x80;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xC0;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            currentAddress += 2;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else {
                        Err("Instruction does not take immediate data");
                        goto end;
                    }
                }
            }
            else if (op[0].isDigit() || op[0] == '$' || op[0] == '%') {
                int value = 0;
                try {
                    value = getNum(op);
                }
                catch (...) {
                    Err("Invalid number: " + op);
                    goto end;
                }
                if (dirInstructionsM6800.contains(in)) {
                    if (value < 256) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0x99;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xD9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0x9B;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xDB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0x94;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xD4;
                            }
                            else if (in == "BITA") {
                                inCode = 0x95;
                            }
                            else if (in == "BITB") {
                                inCode = 0xD5;
                            }
                            else if (in == "CMPA") {
                                inCode = 0x91;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xD1;
                            }
                            else if (in == "CPX") {
                                inCode = 0x9C;
                            }
                            else if (in == "EORA") {
                                inCode = 0x98;
                            }
                            else if (in == "EORB") {
                                inCode = 0xD8;
                            }
                            else if (in == "LDAA") {
                                inCode = 0x96;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xD6;
                            }
                            else if (in == "LDS") {
                                inCode = 0x9E;
                            }
                            else if (in == "LDX") {
                                inCode = 0xDE;
                            }
                            else if (in == "ORAA") {
                                inCode = 0x9A;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xDA;
                            }
                            else if (in == "SBCA") {
                                inCode = 0x92;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xD2;
                            }
                            else if (in == "STAA") {
                                inCode = 0x97;
                            }
                            else if (in == "STAB") {
                                inCode = 0xD7;
                            }
                            else if (in == "STS") {
                                inCode = 0x9F;
                            }
                            else if (in == "STX") {
                                inCode = 0xDF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0x90;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xD0;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        opCode = value;
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        currentAddress += 2;
                    }
                    else if (value < 0x10000) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0xB9;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xF9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0xBB;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xFB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0xB4;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xF4;
                            }
                            else if (in == "ASL") {
                                inCode = 0x78;
                            }
                            else if (in == "ASR") {
                                inCode = 0x77;
                            }
                            else if (in == "BITA") {
                                inCode = 0xB5;
                            }
                            else if (in == "BITB") {
                                inCode = 0xF5;
                            }
                            else if (in == "CLR") {
                                inCode = 0x7F;
                            }
                            else if (in == "CMPA") {
                                inCode = 0xB1;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xF1;
                            }
                            else if (in == "COM") {
                                inCode = 0x73;
                            }
                            else if (in == "CPX") {
                                inCode = 0xBC;
                            }
                            else if (in == "DEC") {
                                inCode = 0x7A;
                            }
                            else if (in == "EORA") {
                                inCode = 0xB8;
                            }
                            else if (in == "EORB") {
                                inCode = 0xF8;
                            }
                            else if (in == "INC") {
                                inCode = 0x7C;
                            }
                            else if (in == "JMP") {
                                inCode = 0x7E;
                            }
                            else if (in == "JSR") {
                                inCode = 0xBD;
                            }
                            else if (in == "LDAA") {
                                inCode = 0xB6;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xF6;
                            }
                            else if (in == "LDS") {
                                inCode = 0xBE;
                            }
                            else if (in == "LDX") {
                                inCode = 0xFE;
                            }
                            else if (in == "LSR") {
                                inCode = 0x74;
                            }
                            else if (in == "NEG") {
                                inCode = 0x70;
                            }
                            else if (in == "ORAA") {
                                inCode = 0xBA;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xFA;
                            }
                            else if (in == "ROL") {
                                inCode = 0x79;
                            }
                            else if (in == "ROR") {
                                inCode = 0x76;
                            }
                            else if (in == "SBCA") {
                                inCode = 0xB2;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xF2;
                            }
                            else if (in == "STAA") {
                                inCode = 0xB7;
                            }
                            else if (in == "STAB") {
                                inCode = 0xF7;
                            }
                            else if (in == "STS") {
                                inCode = 0xBF;
                            }
                            else if (in == "STX") {
                                inCode = 0xFF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0xB0;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xF0;
                            }
                            else if (in == "TST") {
                                inCode = 0x7D;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        opCode = (value >> 8) & 0xFF;
                        opCode2 = value & 0xFF;
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        Memory[currentAddress + 2] = opCode2;
                        currentAddress += 3;
                    }
                    else {
                        Err("Value out of range: " + QString::number(value));
                        goto end;
                    }
                }
                else if (razInstructionsM6800.contains(in)) {
                    if (value < 0x10000) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0xB9;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xF9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0xBB;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xFB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0xB4;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xF4;
                            }
                            else if (in == "ASL") {
                                inCode = 0x78;
                            }
                            else if (in == "ASR") {
                                inCode = 0x77;
                            }
                            else if (in == "BITA") {
                                inCode = 0xB5;
                            }
                            else if (in == "BITB") {
                                inCode = 0xF5;
                            }
                            else if (in == "CLR") {
                                inCode = 0x7F;
                            }
                            else if (in == "CMPA") {
                                inCode = 0xB1;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xF1;
                            }
                            else if (in == "COM") {
                                inCode = 0x73;
                            }
                            else if (in == "CPX") {
                                inCode = 0xBC;
                            }
                            else if (in == "DEC") {
                                inCode = 0x7A;
                            }
                            else if (in == "EORA") {
                                inCode = 0xB8;
                            }
                            else if (in == "EORB") {
                                inCode = 0xF8;
                            }
                            else if (in == "INC") {
                                inCode = 0x7C;
                            }
                            else if (in == "JMP") {
                                inCode = 0x7E;
                            }
                            else if (in == "JSR") {
                                inCode = 0xBD;
                            }
                            else if (in == "LDAA") {
                                inCode = 0xB6;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xF6;
                            }
                            else if (in == "LDS") {
                                inCode = 0xBE;
                            }
                            else if (in == "LDX") {
                                inCode = 0xFE;
                            }
                            else if (in == "LSR") {
                                inCode = 0x74;
                            }
                            else if (in == "NEG") {
                                inCode = 0x70;
                            }
                            else if (in == "ORAA") {
                                inCode = 0xBA;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xFA;
                            }
                            else if (in == "ROL") {
                                inCode = 0x79;
                            }
                            else if (in == "ROR") {
                                inCode = 0x76;
                            }
                            else if (in == "SBCA") {
                                inCode = 0xB2;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xF2;
                            }
                            else if (in == "STAA") {
                                inCode = 0xB7;
                            }
                            else if (in == "STAB") {
                                inCode = 0xF7;
                            }
                            else if (in == "STS") {
                                inCode = 0xBF;
                            }
                            else if (in == "STX") {
                                inCode = 0xFF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0xB0;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xF0;
                            }
                            else if (in == "TST") {
                                inCode = 0x7D;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        opCode = (value >> 8) & 0xFF;
                        opCode2 = value & 0xFF;
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        Memory[currentAddress + 2] = opCode2;
                        currentAddress += 3;
                    }
                    else {
                        Err("Value out of range: " + QString::number(value));
                        goto end;
                    }
                }
                else {
                    Err("Instruction does not take direct data");
                    goto end;
                }
            }
            else if (op[0].isLetter()) {
                if(labelValMap.count(op) == 0){ // unknown
                    if (razInstructionsM6800.contains(in)) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0xB9;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xF9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0xBB;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xFB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0xB4;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xF4;
                            }
                            else if (in == "ASL") {
                                inCode = 0x78;
                            }
                            else if (in == "ASR") {
                                inCode = 0x77;
                            }
                            else if (in == "BITA") {
                                inCode = 0xB5;
                            }
                            else if (in == "BITB") {
                                inCode = 0xF5;
                            }
                            else if (in == "CLR") {
                                inCode = 0x7F;
                            }
                            else if (in == "CMPA") {
                                inCode = 0xB1;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xF1;
                            }
                            else if (in == "COM") {
                                inCode = 0x73;
                            }
                            else if (in == "CPX") {
                                inCode = 0xBC;
                            }
                            else if (in == "DEC") {
                                inCode = 0x7A;
                            }
                            else if (in == "EORA") {
                                inCode = 0xB8;
                            }
                            else if (in == "EORB") {
                                inCode = 0xF8;
                            }
                            else if (in == "INC") {
                                inCode = 0x7C;
                            }
                            else if (in == "JMP") {
                                inCode = 0x7E;
                            }
                            else if (in == "JSR") {
                                inCode = 0xBD;
                            }
                            else if (in == "LDAA") {
                                inCode = 0xB6;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xF6;
                            }
                            else if (in == "LDS") {
                                inCode = 0xBE;
                            }
                            else if (in == "LDX") {
                                inCode = 0xFE;
                            }
                            else if (in == "LSR") {
                                inCode = 0x74;
                            }
                            else if (in == "NEG") {
                                inCode = 0x70;
                            }
                            else if (in == "ORAA") {
                                inCode = 0xBA;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xFA;
                            }
                            else if (in == "ROL") {
                                inCode = 0x79;
                            }
                            else if (in == "ROR") {
                                inCode = 0x76;
                            }
                            else if (in == "SBCA") {
                                inCode = 0xB2;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xF2;
                            }
                            else if (in == "STAA") {
                                inCode = 0xB7;
                            }
                            else if (in == "STAB") {
                                inCode = 0xF7;
                            }
                            else if (in == "STS") {
                                inCode = 0xBF;
                            }
                            else if (in == "STX") {
                                inCode = 0xFF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0xB0;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xF0;
                            }
                            else if (in == "TST") {
                                inCode = 0x7D;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        if (op[0].isLetter()) {
                            callLabelRazMap[currentAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentAddress] = inCode;
                        currentAddress += 3;
                    }
                    else if (dirInstructionsM6800.contains(in)) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0x99;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xD9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0x9B;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xDB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0x94;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xD4;
                            }
                            else if (in == "BITA") {
                                inCode = 0x95;
                            }
                            else if (in == "BITB") {
                                inCode = 0xD5;
                            }
                            else if (in == "CMPA") {
                                inCode = 0x91;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xD1;
                            }
                            else if (in == "CPX") {
                                inCode = 0x9C;
                            }
                            else if (in == "EORA") {
                                inCode = 0x98;
                            }
                            else if (in == "EORB") {
                                inCode = 0xD8;
                            }
                            else if (in == "LDAA") {
                                inCode = 0x96;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xD6;
                            }
                            else if (in == "LDS") {
                                inCode = 0x9E;
                            }
                            else if (in == "LDX") {
                                inCode = 0xDE;
                            }
                            else if (in == "ORAA") {
                                inCode = 0x9A;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xDA;
                            }
                            else if (in == "SBCA") {
                                inCode = 0x92;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xD2;
                            }
                            else if (in == "STAA") {
                                inCode = 0x97;
                            }
                            else if (in == "STAB") {
                                inCode = 0xD7;
                            }
                            else if (in == "STS") {
                                inCode = 0x9F;
                            }
                            else if (in == "STX") {
                                inCode = 0xDF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0x90;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xD0;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        if (op[0].isLetter()) {
                            callLabelMap[currentAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentAddress] = inCode;
                        currentAddress += 2;
                    }
                    else {
                        Err("Instruction does not take direct data");
                        goto end;
                    }
                }else{
                    int value = labelValMap[op];
                    if (dirInstructionsM6800.contains(in)) {
                        if (value < 256) {
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0x99;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xD9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0x9B;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xDB;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0x94;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xD4;
                                }
                                else if (in == "BITA") {
                                    inCode = 0x95;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xD5;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0x91;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xD1;
                                }
                                else if (in == "CPX") {
                                    inCode = 0x9C;
                                }
                                else if (in == "EORA") {
                                    inCode = 0x98;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xD8;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0x96;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xD6;
                                }
                                else if (in == "LDS") {
                                    inCode = 0x9E;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xDE;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0x9A;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xDA;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0x92;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xD2;
                                }
                                else if (in == "STAA") {
                                    inCode = 0x97;
                                }
                                else if (in == "STAB") {
                                    inCode = 0xD7;
                                }
                                else if (in == "STS") {
                                    inCode = 0x9F;
                                }
                                else if (in == "STX") {
                                    inCode = 0xDF;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0x90;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xD0;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            opCode = value;
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            currentAddress += 2;
                        }
                        else if (value < 0x10000) {
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0xB9;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xF9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0xBB;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xFB;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0xB4;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xF4;
                                }
                                else if (in == "ASL") {
                                    inCode = 0x78;
                                }
                                else if (in == "ASR") {
                                    inCode = 0x77;
                                }
                                else if (in == "BITA") {
                                    inCode = 0xB5;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xF5;
                                }
                                else if (in == "CLR") {
                                    inCode = 0x7F;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0xB1;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xF1;
                                }
                                else if (in == "COM") {
                                    inCode = 0x73;
                                }
                                else if (in == "CPX") {
                                    inCode = 0xBC;
                                }
                                else if (in == "DEC") {
                                    inCode = 0x7A;
                                }
                                else if (in == "EORA") {
                                    inCode = 0xB8;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xF8;
                                }
                                else if (in == "INC") {
                                    inCode = 0x7C;
                                }
                                else if (in == "JMP") {
                                    inCode = 0x7E;
                                }
                                else if (in == "JSR") {
                                    inCode = 0xBD;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0xB6;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xF6;
                                }
                                else if (in == "LDS") {
                                    inCode = 0xBE;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xFE;
                                }
                                else if (in == "LSR") {
                                    inCode = 0x74;
                                }
                                else if (in == "NEG") {
                                    inCode = 0x70;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0xBA;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xFA;
                                }
                                else if (in == "ROL") {
                                    inCode = 0x79;
                                }
                                else if (in == "ROR") {
                                    inCode = 0x76;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0xB2;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xF2;
                                }
                                else if (in == "STAA") {
                                    inCode = 0xB7;
                                }
                                else if (in == "STAB") {
                                    inCode = 0xF7;
                                }
                                else if (in == "STS") {
                                    inCode = 0xBF;
                                }
                                else if (in == "STX") {
                                    inCode = 0xFF;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0xB0;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xF0;
                                }
                                else if (in == "TST") {
                                    inCode = 0x7D;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            opCode = (value >> 8) & 0xFF;
                            opCode2 = value & 0xFF;
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            Memory[currentAddress + 2] = opCode2;
                            currentAddress += 3;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else if (razInstructionsM6800.contains(in)) {
                        if (value < 0x10000) {
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0xB9;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xF9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0xBB;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xFB;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0xB4;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xF4;
                                }
                                else if (in == "ASL") {
                                    inCode = 0x78;
                                }
                                else if (in == "ASR") {
                                    inCode = 0x77;
                                }
                                else if (in == "BITA") {
                                    inCode = 0xB5;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xF5;
                                }
                                else if (in == "CLR") {
                                    inCode = 0x7F;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0xB1;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xF1;
                                }
                                else if (in == "COM") {
                                    inCode = 0x73;
                                }
                                else if (in == "CPX") {
                                    inCode = 0xBC;
                                }
                                else if (in == "DEC") {
                                    inCode = 0x7A;
                                }
                                else if (in == "EORA") {
                                    inCode = 0xB8;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xF8;
                                }
                                else if (in == "INC") {
                                    inCode = 0x7C;
                                }
                                else if (in == "JMP") {
                                    inCode = 0x7E;
                                }
                                else if (in == "JSR") {
                                    inCode = 0xBD;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0xB6;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xF6;
                                }
                                else if (in == "LDS") {
                                    inCode = 0xBE;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xFE;
                                }
                                else if (in == "LSR") {
                                    inCode = 0x74;
                                }
                                else if (in == "NEG") {
                                    inCode = 0x70;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0xBA;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xFA;
                                }
                                else if (in == "ROL") {
                                    inCode = 0x79;
                                }
                                else if (in == "ROR") {
                                    inCode = 0x76;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0xB2;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xF2;
                                }
                                else if (in == "STAA") {
                                    inCode = 0xB7;
                                }
                                else if (in == "STAB") {
                                    inCode = 0xF7;
                                }
                                else if (in == "STS") {
                                    inCode = 0xBF;
                                }
                                else if (in == "STX") {
                                    inCode = 0xFF;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0xB0;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xF0;
                                }
                                else if (in == "TST") {
                                    inCode = 0x7D;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            opCode = (value >> 8) & 0xFF;
                            opCode2 = value & 0xFF;
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            Memory[currentAddress + 2] = opCode2;
                            currentAddress += 3;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else {
                        Err("Instruction does not take direct data");
                        goto end;
                    }
                }

            }
            else {
                Err("Instruction does not take relative data");
                goto end;
            }
        } else if(ver == 1){
            if (vseInstructionsM6803.indexOf(in) != -1) { //VSE
                if (op != "") {
                    Err("Unexpected operand");
                    goto end;
                }
                if (true) {
                    if (in == "ABA") {
                        inCode = 0x1B;
                    }
                    else if (in == "ABX") {
                        inCode = 0x3A;
                    }
                    else if (in == "ASLA" || in == "LSLA") {
                        inCode = 0x48;
                    }
                    else if (in == "ASLB" || in == "LSLB") {
                        inCode = 0x58;
                    }
                    else if (in == "ASLD" || in == "LSLD") {
                        inCode = 0x05;
                    }
                    else if (in == "ASRA") {
                        inCode = 0x47;
                    }
                    else if (in == "ASRB") {
                        inCode = 0x57;
                    }
                    else if (in == "CBA") {
                        inCode = 0x11;
                    }
                    else if (in == "CLC") {
                        inCode = 0x0C;
                    }
                    else if (in == "CLI") {
                        inCode = 0x0E;
                    }
                    else if (in == "CLRA") {
                        inCode = 0x4F;
                    }
                    else if (in == "CLRB") {
                        inCode = 0x5F;
                    }
                    else if (in == "CLV") {
                        inCode = 0x0A;
                    }
                    else if (in == "COMA") {
                        inCode = 0x43;
                    }
                    else if (in == "COMB") {
                        inCode = 0x53;
                    }
                    else if (in == "DAA") {
                        inCode = 0x19;
                    }
                    else if (in == "DECA") {
                        inCode = 0x4A;
                    }
                    else if (in == "DECB") {
                        inCode = 0x5A;
                    }
                    else if (in == "DES") {
                        inCode = 0x34;
                    }
                    else if (in == "DEX") {
                        inCode = 0x09;
                    }
                    else if (in == "INCA") {
                        inCode = 0x4C;
                    }
                    else if (in == "INCB") {
                        inCode = 0x5C;
                    }
                    else if (in == "INS") {
                        inCode = 0x31;
                    }
                    else if (in == "INX") {
                        inCode = 0x08;
                    }
                    else if (in == "LSRA") {
                        inCode = 0x44;
                    }
                    else if (in == "LSRB") {
                        inCode = 0x54;
                    }
                    else if (in == "LSRD") {
                        inCode = 0x04;
                    }
                    else if (in == "MUL") {
                        inCode = 0x3D;
                    }
                    else if (in == "NEGA") {
                        inCode = 0x40;
                    }
                    else if (in == "NEGB") {
                        inCode = 0x50;
                    }
                    else if (in == "NOP") {
                        inCode = 0x01;
                    }
                    else if (in == "PSHA") {
                        inCode = 0x36;
                    }
                    else if (in == "PSHX") {
                        inCode = 0x3C;
                    }
                    else if (in == "PSHB") {
                        inCode = 0x37;
                    }
                    else if (in == "PULA") {
                        inCode = 0x32;
                    }
                    else if (in == "PULB") {
                        inCode = 0x33;
                    }
                    else if (in == "PULX") {
                        inCode = 0x38;
                    }
                    else if (in == "ROLA") {
                        inCode = 0x49;
                    }
                    else if (in == "ROLB") {
                        inCode = 0x59;
                    }
                    else if (in == "RORA") {
                        inCode = 0x46;
                    }
                    else if (in == "RORB") {
                        inCode = 0x56;
                    }
                    else if (in == "RTI") {
                        inCode = 0x3B;
                    }
                    else if (in == "RTS") {
                        inCode = 0x39;
                    }
                    else if (in == "SBA") {
                        inCode = 0x10;
                    }
                    else if (in == "SEC") {
                        inCode = 0x0D;
                    }
                    else if (in == "SEI") {
                        inCode = 0x0F;
                    }
                    else if (in == "SEV") {
                        inCode = 0x0B;
                    }
                    else if (in == "SWI") {
                        inCode = 0x3F;
                    }
                    else if (in == "TAB") {
                        inCode = 0x16;
                    }
                    else if (in == "TAP") {
                        inCode = 0x06;
                    }
                    else if (in == "TBA") {
                        inCode = 0x17;
                    }
                    else if (in == "TPA") {
                        inCode = 0x07;
                    }
                    else if (in == "TSTA") {
                        inCode = 0x4D;
                    }
                    else if (in == "TSTB") {
                        inCode = 0x5D;
                    }
                    else if (in == "TSX") {
                        inCode = 0x30;
                    }
                    else if (in == "TXS") {
                        inCode = 0x35;
                    }
                    else if (in == "WAI") {
                        inCode = 0x3E;
                    }
                    else {
                        Err("Unknown instruction");
                        goto end;
                    }
                }
                if (!label.isEmpty()) {
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentAddress) + "' to:'" + label + "'", -1);
                }
                Memory[currentAddress] = inCode;
                currentAddress += 1;
                goto skipLine;
            }
            else {
                if (op.isEmpty()) {
                    Err("Missing operand");
                    goto end;
                }
                if (!label.isEmpty()) {
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentAddress) + "' to:'" + label + "'", -1);
                }
            }
            if (relInstructionsM6803.indexOf(in) != -1) { //REL
                if (op.startsWith("#")) {
                    Err("Instruction won't take immediate data");
                    goto end;
                }
                if (op.contains(",")) {
                    Err("Instruction won't take indexed data");
                    goto end;
                }
                if (true) {
                    if (in == "BCC" || in == "BHS") {
                        inCode = 0x24;
                    }
                    else if (in == "BCS" || in == "BLO") {
                        inCode = 0x25;
                    }
                    else if (in == "BEQ") {
                        inCode = 0x27;
                    }
                    else if (in == "BGE") {
                        inCode = 0x2C;
                    }
                    else if (in == "BGT") {
                        inCode = 0x2E;
                    }
                    else if (in == "BHI") {
                        inCode = 0x22;
                    }
                    else if (in == "BLE") {
                        inCode = 0x2F;
                    }
                    else if (in == "BLS") {
                        inCode = 0x23;
                    }
                    else if (in == "BLT") {
                        inCode = 0x2D;
                    }
                    else if (in == "BMI") {
                        inCode = 0x2B;
                    }
                    else if (in == "BNE") {
                        inCode = 0x26;
                    }
                    else if (in == "BPL") {
                        inCode = 0x2A;
                    }
                    else if (in == "BRA") {
                        inCode = 0x20;
                    }
                    else if (in == "BRN") {
                        inCode = 0x21;
                    }
                    else if (in == "BSR") {
                        inCode = 0x8D;
                    }
                    else if (in == "BVC") {
                        inCode = 0x28;
                    }
                    else if (in == "BVS") {
                        inCode = 0x29;
                    }
                    else {
                        Err("Unknown instruction");
                        goto end;
                    }
                }
                if (op == "") {
                    Err("Missing operand");
                    goto end;
                }
                bool ok;
                if (op[0].isLetter()) {
                    callLabelRelMap[currentAddress + 1] = op;
                    opCode = 0;
                }
                else if (op.startsWith('$')) {
                    QString hexValue = op.sliced(1);
                    opCode = hexValue.toInt(&ok, 16);
                    if (!ok) {
                        Err("Invalid hexadecimal number: " + op);
                        goto end;
                    }
                    if (opCode > 0xFD) {
                        Err("Relative address out of range[-128,127]: " + hexValue);
                        goto end;
                    }
                }
                else if (op.startsWith('%')) {
                    QString binaryValue = op.sliced(1);
                    opCode = binaryValue.toInt(&ok, 2);
                    if (!ok) {
                        Err("Invalid binary number: " + op);
                        goto end;
                    }
                    if (opCode > 0xFD) {
                        Err("Relative address out of range[-128,127]: " + binaryValue);
                        goto end;
                    }
                }
                else {
                    opCode = op.toInt(&ok);
                    if (!ok) {
                        Err("Invalid decimal number: " + op);
                        goto end;
                    }
                    if (opCode < 0)
                        opCode -= 2;
                    if (opCode > 127 || opCode < -128) {
                        Err("Relative address out of range[-128,127]: " + QString::number(opCode));
                        goto end;
                    }
                    qint8 signedValue = static_cast<qint8>(opCode);
                    opCode = signedValue & 0xFF;
                }
                Memory[currentAddress] = inCode;
                Memory[currentAddress + 1] = opCode;
                currentAddress += 2;
            }
            else if (op.contains(",")) { // IND
                if (!indInstructionsM6803.contains(in)) {
                    Err("Instruction does not take indexed data");
                    goto end;
                }
                op.chop(2);
                if (op[0].isLetter()) {
                    if(labelValMap.count(op) == 0){
                        opCode = 0;
                        callLabelMap[currentAddress + 1] = op;
                    }else{
                        int value = labelValMap[op];
                        if (value > 255){
                            PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentAddress)+"' is out of instructions range. Entered last byte",1);
                            value = value & 0xFF;
                            opCode = value;
                        }else{
                            value = value & 0xFF;
                            opCode = value;
                        }
                    }
                }
                else {
                    int value = 0;
                    try {
                        value = getNum(op);
                    }
                    catch (...) {
                        Err("Invalid number: " + op);
                        goto end;
                    }
                    if (value > 255) {
                        Err("Value out of range: " + QString::number(value));
                        goto end;
                    }
                    opCode = value;
                }
                if (true) {
                    if (in == "ADCA") {
                        inCode = 0xA9;
                    }
                    else if (in == "ADCB") {
                        inCode = 0xE9;
                    }
                    else if (in == "ADDA") {
                        inCode = 0xAB;
                    }
                    else if (in == "ADDB") {
                        inCode = 0xEB;
                    }
                    else if (in == "ADDD") {
                        inCode = 0xE3;
                    }
                    else if (in == "ANDA") {
                        inCode = 0xA4;
                    }
                    else if (in == "ANDB") {
                        inCode = 0xE4;
                    }
                    else if (in == "ASL" || in == "LSL") {
                        inCode = 0x68;
                    }
                    else if (in == "ASR") {
                        inCode = 0x67;
                    }
                    else if (in == "BITA") {
                        inCode = 0xA5;
                    }
                    else if (in == "BITB") {
                        inCode = 0xE5;
                    }
                    else if (in == "CLR") {
                        inCode = 0x6F;
                    }
                    else if (in == "CMPA") {
                        inCode = 0xA1;
                    }
                    else if (in == "CMPB") {
                        inCode = 0xE1;
                    }
                    else if (in == "COM") {
                        inCode = 0x63;
                    }
                    else if (in == "CPX") {
                        inCode = 0xAC;
                    }
                    else if (in == "DEC") {
                        inCode = 0x6A;
                    }
                    else if (in == "EORA") {
                        inCode = 0xA8;
                    }
                    else if (in == "EORB") {
                        inCode = 0xE8;
                    }
                    else if (in == "INC") {
                        inCode = 0x6C;
                    }
                    else if (in == "JMP") {
                        inCode = 0x6E;
                    }
                    else if (in == "JSR") {
                        inCode = 0xAD;
                    }
                    else if (in == "LDAA") {
                        inCode = 0xA6;
                    }
                    else if (in == "LDAB") {
                        inCode = 0xE6;
                    }
                    else if (in == "LDD") {
                        inCode = 0xEC;
                    }
                    else if (in == "LDS") {
                        inCode = 0xAE;
                    }
                    else if (in == "LDX") {
                        inCode = 0xEE;
                    }
                    else if (in == "LSR") {
                        inCode = 0x64;
                    }
                    else if (in == "NEG") {
                        inCode = 0x60;
                    }
                    else if (in == "ORAA") {
                        inCode = 0xAA;
                    }
                    else if (in == "ORAB") {
                        inCode = 0xEA;
                    }
                    else if (in == "ROL") {
                        inCode = 0x69;
                    }
                    else if (in == "ROR") {
                        inCode = 0x66;
                    }
                    else if (in == "SBCA") {
                        inCode = 0xA2;
                    }
                    else if (in == "SBCB") {
                        inCode = 0xE2;
                    }
                    else if (in == "STAA") {
                        inCode = 0xA7;
                    }
                    else if (in == "STAB") {
                        inCode = 0xE7;
                    }
                    else if (in == "STD") {
                        inCode = 0xED;
                    }
                    else if (in == "STS") {
                        inCode = 0xAF;
                    }
                    else if (in == "STX") {
                        inCode = 0xEF;
                    }
                    else if (in == "SUBA") {
                        inCode = 0xA0;
                    }
                    else if (in == "SUBB") {
                        inCode = 0xE0;
                    }
                    else if (in == "SUBD") {
                        inCode = 0xA3;
                    }
                    else if (in == "TST") {
                        inCode = 0x6D;
                    }
                    else {
                        Err("Unknown instruction");
                        goto end;
                    }
                }
                Memory[currentAddress] = inCode;
                Memory[currentAddress + 1] = opCode;
                currentAddress += 2;
            }
            else if (op.startsWith("#")) {
                op = op.sliced(1);
                opCode = 0;
                opCode2 = 0;
                if (op[0].isLetter()) { //TAK LAB
                    if (takRazInstructionsM6803.indexOf(in) != -1) { // TAK RAZ LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                opCode = (value >> 8) & 0xFF;
                                opCode2 = value & 0xFF;
                            }else{
                                opCode = value;
                            }
                        }
                        Memory[currentAddress + 1] = opCode;
                        Memory[currentAddress + 2] = opCode2;
                        if (true) {
                            if (in == "ADDD") {
                                inCode = 0xC3;
                            }
                            else if (in == "CPX") {
                                inCode = 0x8C;
                            }
                            else if (in == "LDD") {
                                inCode = 0xCC;
                            }
                            else if (in == "LDS") {
                                inCode = 0x8E;
                            }
                            else if (in == "LDX") {
                                inCode = 0xCE;
                            }
                            else if (in == "SUBD") {
                                inCode = 0x83;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        Memory[currentAddress] = inCode;
                        currentAddress += 3;
                    }
                    else if (takInstructionsM6803.contains(in)) { //tak LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentAddress)+"' is out of instructions range. Entered last byte",1);
                                value = value & 0xFF;
                                opCode = value;
                            }else{
                                value = value & 0xFF;
                                opCode = value;
                            }
                        }
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0x89;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xC9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0x8B;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xCB;
                            }
                            else if (in == "ANDA") {
                                inCode = 0x84;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xC4;
                            }
                            else if (in == "BITA") {
                                inCode = 0x85;
                            }
                            else if (in == "BITB") {
                                inCode = 0xC5;
                            }
                            else if (in == "CMPA") {
                                inCode = 0x81;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xC1;
                            }
                            else if (in == "EORA") {
                                inCode = 0x88;
                            }
                            else if (in == "EORB") {
                                inCode = 0xC8;
                            }
                            else if (in == "LDAA") {
                                inCode = 0x86;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xC6;
                            }
                            else if (in == "ORAA") {
                                inCode = 0x8A;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xCA;
                            }
                            else if (in == "SBCA") {
                                inCode = 0x82;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xC2;
                            }
                            else if (in == "SUBA") {
                                inCode = 0x80;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xC0;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        currentAddress += 2;
                    }
                    else {
                        Err("Instruction does not take immediate data");
                        goto end;
                    }
                }
                else {
                    int value = 0;
                    try {
                        value = getNum(op);
                    }
                    catch (...) {
                        Err("Invalid number: " + op);
                        goto end;
                    }
                    if (takRazInstructionsM6803.indexOf(in) != -1) { // TAK RAZ
                        if (value < 0x10000) {
                            opCode = (value >> 8) & 0xFF;
                            opCode2 = value & 0xFF;
                            Memory[currentAddress + 1] = opCode;
                            Memory[currentAddress + 2] = opCode2;
                            if (true) {
                                if (in == "ADDD") {
                                    inCode = 0xC3;
                                }
                                else if (in == "CPX") {
                                    inCode = 0x8C;
                                }
                                else if (in == "LDD") {
                                    inCode = 0xCC;
                                }
                                else if (in == "LDS") {
                                    inCode = 0x8E;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xCE;
                                }
                                else if (in == "SUBD") {
                                    inCode = 0x83;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            Memory[currentAddress] = inCode;
                            currentAddress += 3;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else if (takInstructionsM6803.contains(in)) { //tak
                        if (value < 256) {
                            opCode = value;
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0x89;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xC9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0x8B;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xCB;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0x84;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xC4;
                                }
                                else if (in == "BITA") {
                                    inCode = 0x85;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xC5;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0x81;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xC1;
                                }
                                else if (in == "EORA") {
                                    inCode = 0x88;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xC8;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0x86;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xC6;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0x8A;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xCA;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0x82;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xC2;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0x80;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xC0;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            currentAddress += 2;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else {
                        Err("Instruction does not take immediate data");
                        goto end;
                    }
                }
            }
            else if (op[0].isDigit() || op[0] == '$' || op[0] == '%') {
                int value = 0;
                try {
                    value = getNum(op);
                }
                catch (...) {
                    Err("Invalid number: " + op);
                    goto end;
                }
                if (dirInstructionsM6803.contains(in)) {
                    if (value < 256) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0x99;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xD9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0x9B;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xDB;
                            }
                            else if (in == "ADDD") {
                                inCode = 0xD3;
                            }
                            else if (in == "LDD") {
                                inCode = 0xDC;
                            }
                            else if (in == "STD") {
                                inCode = 0xDD;
                            }
                            else if (in == "SUBD") {
                                inCode = 0x93;
                            }
                            else if (in == "ANDA") {
                                inCode = 0x94;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xD4;
                            }
                            else if (in == "BITA") {
                                inCode = 0x95;
                            }
                            else if (in == "BITB") {
                                inCode = 0xD5;
                            }
                            else if (in == "CMPA") {
                                inCode = 0x91;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xD1;
                            }
                            else if (in == "CPX") {
                                inCode = 0x9C;
                            }
                            else if (in == "EORA") {
                                inCode = 0x98;
                            }
                            else if (in == "EORB") {
                                inCode = 0xD8;
                            }
                            else if (in == "JSR") {
                                inCode = 0x9D;
                            }
                            else if (in == "LDAA") {
                                inCode = 0x96;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xD6;
                            }
                            else if (in == "LDS") {
                                inCode = 0x9E;
                            }
                            else if (in == "LDX") {
                                inCode = 0xDE;
                            }
                            else if (in == "ORAA") {
                                inCode = 0x9A;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xDA;
                            }
                            else if (in == "SBCA") {
                                inCode = 0x92;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xD2;
                            }
                            else if (in == "STAA") {
                                inCode = 0x97;
                            }
                            else if (in == "STAB") {
                                inCode = 0xD7;
                            }
                            else if (in == "STS") {
                                inCode = 0x9F;
                            }
                            else if (in == "STX") {
                                inCode = 0xDF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0x90;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xD0;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        opCode = value;
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        currentAddress += 2;
                    }
                    else if (value < 0x10000) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0xB9;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xF9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0xBB;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xFB;
                            }
                            else if (in == "ADDD") {
                                inCode = 0xF3;
                            }
                            else if (in == "LDD") {
                                inCode = 0xFC;
                            }
                            else if (in == "STD") {
                                inCode = 0xFD;
                            }
                            else if (in == "SUBD") {
                                inCode = 0xB3;
                            }
                            else if (in == "ANDA") {
                                inCode = 0xB4;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xF4;
                            }
                            else if (in == "ASL" || in == "LSL") {
                                inCode = 0x78;
                            }
                            else if (in == "ASR") {
                                inCode = 0x77;
                            }
                            else if (in == "BITA") {
                                inCode = 0xB5;
                            }
                            else if (in == "BITB") {
                                inCode = 0xF5;
                            }
                            else if (in == "CLR") {
                                inCode = 0x7F;
                            }
                            else if (in == "CMPA") {
                                inCode = 0xB1;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xF1;
                            }
                            else if (in == "COM") {
                                inCode = 0x73;
                            }
                            else if (in == "CPX") {
                                inCode = 0xBC;
                            }
                            else if (in == "DEC") {
                                inCode = 0x7A;
                            }
                            else if (in == "EORA") {
                                inCode = 0xB8;
                            }
                            else if (in == "EORB") {
                                inCode = 0xF8;
                            }
                            else if (in == "INC") {
                                inCode = 0x7C;
                            }
                            else if (in == "JMP") {
                                inCode = 0x7E;
                            }
                            else if (in == "JSR") {
                                inCode = 0xBD;
                            }
                            else if (in == "LDAA") {
                                inCode = 0xB6;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xF6;
                            }
                            else if (in == "LDS") {
                                inCode = 0xBE;
                            }
                            else if (in == "LDX") {
                                inCode = 0xFE;
                            }
                            else if (in == "LSR") {
                                inCode = 0x74;
                            }
                            else if (in == "NEG") {
                                inCode = 0x70;
                            }
                            else if (in == "ORAA") {
                                inCode = 0xBA;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xFA;
                            }
                            else if (in == "ROL") {
                                inCode = 0x79;
                            }
                            else if (in == "ROR") {
                                inCode = 0x76;
                            }
                            else if (in == "SBCA") {
                                inCode = 0xB2;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xF2;
                            }
                            else if (in == "STAA") {
                                inCode = 0xB7;
                            }
                            else if (in == "STAB") {
                                inCode = 0xF7;
                            }
                            else if (in == "STS") {
                                inCode = 0xBF;
                            }
                            else if (in == "STX") {
                                inCode = 0xFF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0xB0;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xF0;
                            }
                            else if (in == "TST") {
                                inCode = 0x7D;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        opCode = (value >> 8) & 0xFF;
                        opCode2 = value & 0xFF;
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        Memory[currentAddress + 2] = opCode2;
                        currentAddress += 3;
                    }
                    else {
                        Err("Value out of range: " + QString::number(value));
                        goto end;
                    }
                }
                else if (razInstructionsM6803.contains(in)) {
                    if (value < 0x10000) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0xB9;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xF9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0xBB;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xFB;
                            }
                            else if (in == "ADDD") {
                                inCode = 0xF3;
                            }
                            else if (in == "LDD") {
                                inCode = 0xFC;
                            }
                            else if (in == "STD") {
                                inCode = 0xFD;
                            }
                            else if (in == "SUBD") {
                                inCode = 0xB3;
                            }
                            else if (in == "ANDA") {
                                inCode = 0xB4;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xF4;
                            }
                            else if (in == "ASL" || in == "LSL") {
                                inCode = 0x78;
                            }
                            else if (in == "ASR") {
                                inCode = 0x77;
                            }
                            else if (in == "BITA") {
                                inCode = 0xB5;
                            }
                            else if (in == "BITB") {
                                inCode = 0xF5;
                            }
                            else if (in == "CLR") {
                                inCode = 0x7F;
                            }
                            else if (in == "CMPA") {
                                inCode = 0xB1;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xF1;
                            }
                            else if (in == "COM") {
                                inCode = 0x73;
                            }
                            else if (in == "CPX") {
                                inCode = 0xBC;
                            }
                            else if (in == "DEC") {
                                inCode = 0x7A;
                            }
                            else if (in == "EORA") {
                                inCode = 0xB8;
                            }
                            else if (in == "EORB") {
                                inCode = 0xF8;
                            }
                            else if (in == "INC") {
                                inCode = 0x7C;
                            }
                            else if (in == "JMP") {
                                inCode = 0x7E;
                            }
                            else if (in == "JSR") {
                                inCode = 0xBD;
                            }
                            else if (in == "LDAA") {
                                inCode = 0xB6;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xF6;
                            }
                            else if (in == "LDS") {
                                inCode = 0xBE;
                            }
                            else if (in == "LDX") {
                                inCode = 0xFE;
                            }
                            else if (in == "LSR") {
                                inCode = 0x74;
                            }
                            else if (in == "NEG") {
                                inCode = 0x70;
                            }
                            else if (in == "ORAA") {
                                inCode = 0xBA;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xFA;
                            }
                            else if (in == "ROL") {
                                inCode = 0x79;
                            }
                            else if (in == "ROR") {
                                inCode = 0x76;
                            }
                            else if (in == "SBCA") {
                                inCode = 0xB2;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xF2;
                            }
                            else if (in == "STAA") {
                                inCode = 0xB7;
                            }
                            else if (in == "STAB") {
                                inCode = 0xF7;
                            }
                            else if (in == "STS") {
                                inCode = 0xBF;
                            }
                            else if (in == "STX") {
                                inCode = 0xFF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0xB0;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xF0;
                            }
                            else if (in == "TST") {
                                inCode = 0x7D;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        opCode = (value >> 8) & 0xFF;
                        opCode2 = value & 0xFF;
                        Memory[currentAddress] = inCode;
                        Memory[currentAddress + 1] = opCode;
                        Memory[currentAddress + 2] = opCode2;
                        currentAddress += 3;
                    }
                    else {
                        Err("Value out of range: " + QString::number(value));
                        goto end;
                    }
                }
                else {
                    Err("Instruction does not take direct data");
                    goto end;
                }
            }
            else if (op[0].isLetter()) {
                if(labelValMap.count(op) == 0){ // unknown
                    if (razInstructionsM6803.contains(in)) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0xB9;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xF9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0xBB;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xFB;
                            }
                            else if (in == "ADDD") {
                                inCode = 0xF3;
                            }
                            else if (in == "LDD") {
                                inCode = 0xFC;
                            }
                            else if (in == "STD") {
                                inCode = 0xFD;
                            }
                            else if (in == "SUBD") {
                                inCode = 0xB3;
                            }
                            else if (in == "ANDA") {
                                inCode = 0xB4;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xF4;
                            }
                            else if (in == "ASL" || in == "LSL") {
                                inCode = 0x78;
                            }
                            else if (in == "ASR") {
                                inCode = 0x77;
                            }
                            else if (in == "BITA") {
                                inCode = 0xB5;
                            }
                            else if (in == "BITB") {
                                inCode = 0xF5;
                            }
                            else if (in == "CLR") {
                                inCode = 0x7F;
                            }
                            else if (in == "CMPA") {
                                inCode = 0xB1;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xF1;
                            }
                            else if (in == "COM") {
                                inCode = 0x73;
                            }
                            else if (in == "CPX") {
                                inCode = 0xBC;
                            }
                            else if (in == "DEC") {
                                inCode = 0x7A;
                            }
                            else if (in == "EORA") {
                                inCode = 0xB8;
                            }
                            else if (in == "EORB") {
                                inCode = 0xF8;
                            }
                            else if (in == "INC") {
                                inCode = 0x7C;
                            }
                            else if (in == "JMP") {
                                inCode = 0x7E;
                            }
                            else if (in == "JSR") {
                                inCode = 0xBD;
                            }
                            else if (in == "LDAA") {
                                inCode = 0xB6;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xF6;
                            }
                            else if (in == "LDS") {
                                inCode = 0xBE;
                            }
                            else if (in == "LDX") {
                                inCode = 0xFE;
                            }
                            else if (in == "LSR") {
                                inCode = 0x74;
                            }
                            else if (in == "NEG") {
                                inCode = 0x70;
                            }
                            else if (in == "ORAA") {
                                inCode = 0xBA;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xFA;
                            }
                            else if (in == "ROL") {
                                inCode = 0x79;
                            }
                            else if (in == "ROR") {
                                inCode = 0x76;
                            }
                            else if (in == "SBCA") {
                                inCode = 0xB2;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xF2;
                            }
                            else if (in == "STAA") {
                                inCode = 0xB7;
                            }
                            else if (in == "STAB") {
                                inCode = 0xF7;
                            }
                            else if (in == "STS") {
                                inCode = 0xBF;
                            }
                            else if (in == "STX") {
                                inCode = 0xFF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0xB0;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xF0;
                            }
                            else if (in == "TST") {
                                inCode = 0x7D;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        if (op[0].isLetter()) {
                            callLabelRazMap[currentAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentAddress] = inCode;
                        currentAddress += 3;
                    }
                    else if (dirInstructionsM6803.contains(in)) {
                        if (true) {
                            if (in == "ADCA") {
                                inCode = 0x99;
                            }
                            else if (in == "ADCB") {
                                inCode = 0xD9;
                            }
                            else if (in == "ADDA") {
                                inCode = 0x9B;
                            }
                            else if (in == "ADDB") {
                                inCode = 0xDB;
                            }
                            else if (in == "ADDD") {
                                inCode = 0xD3;
                            }
                            else if (in == "LDD") {
                                inCode = 0xDC;
                            }
                            else if (in == "STD") {
                                inCode = 0xDD;
                            }
                            else if (in == "SUBD") {
                                inCode = 0x93;
                            }
                            else if (in == "ANDA") {
                                inCode = 0x94;
                            }
                            else if (in == "ANDB") {
                                inCode = 0xD4;
                            }
                            else if (in == "BITA") {
                                inCode = 0x95;
                            }
                            else if (in == "BITB") {
                                inCode = 0xD5;
                            }
                            else if (in == "CMPA") {
                                inCode = 0x91;
                            }
                            else if (in == "CMPB") {
                                inCode = 0xD1;
                            }
                            else if (in == "CPX") {
                                inCode = 0x9C;
                            }
                            else if (in == "EORA") {
                                inCode = 0x98;
                            }
                            else if (in == "EORB") {
                                inCode = 0xD8;
                            }
                            else if (in == "JSR") {
                                inCode = 0x9D;
                            }
                            else if (in == "LDAA") {
                                inCode = 0x96;
                            }
                            else if (in == "LDAB") {
                                inCode = 0xD6;
                            }
                            else if (in == "LDS") {
                                inCode = 0x9E;
                            }
                            else if (in == "LDX") {
                                inCode = 0xDE;
                            }
                            else if (in == "ORAA") {
                                inCode = 0x9A;
                            }
                            else if (in == "ORAB") {
                                inCode = 0xDA;
                            }
                            else if (in == "SBCA") {
                                inCode = 0x92;
                            }
                            else if (in == "SBCB") {
                                inCode = 0xD2;
                            }
                            else if (in == "STAA") {
                                inCode = 0x97;
                            }
                            else if (in == "STAB") {
                                inCode = 0xD7;
                            }
                            else if (in == "STS") {
                                inCode = 0x9F;
                            }
                            else if (in == "STX") {
                                inCode = 0xDF;
                            }
                            else if (in == "SUBA") {
                                inCode = 0x90;
                            }
                            else if (in == "SUBB") {
                                inCode = 0xD0;
                            }
                            else {
                                Err("Unknown instruction");
                                goto end;
                            }
                        }
                        if (op[0].isLetter()) {
                            callLabelMap[currentAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentAddress] = inCode;
                        currentAddress += 2;
                    }
                    else {
                        Err("Instruction does not take direct data");
                        goto end;
                    }
                }else{
                    int value = labelValMap[op];
                    if (dirInstructionsM6803.contains(in)) {
                        if (value < 256) {
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0x99;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xD9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0x9B;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xDB;
                                }
                                else if (in == "ADDD") {
                                    inCode = 0xD3;
                                }
                                else if (in == "LDD") {
                                    inCode = 0xDC;
                                }
                                else if (in == "STD") {
                                    inCode = 0xDD;
                                }
                                else if (in == "SUBD") {
                                    inCode = 0x93;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0x94;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xD4;
                                }
                                else if (in == "BITA") {
                                    inCode = 0x95;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xD5;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0x91;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xD1;
                                }
                                else if (in == "CPX") {
                                    inCode = 0x9C;
                                }
                                else if (in == "EORA") {
                                    inCode = 0x98;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xD8;
                                }
                                else if (in == "JSR") {
                                    inCode = 0x9D;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0x96;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xD6;
                                }
                                else if (in == "LDS") {
                                    inCode = 0x9E;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xDE;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0x9A;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xDA;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0x92;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xD2;
                                }
                                else if (in == "STAA") {
                                    inCode = 0x97;
                                }
                                else if (in == "STAB") {
                                    inCode = 0xD7;
                                }
                                else if (in == "STS") {
                                    inCode = 0x9F;
                                }
                                else if (in == "STX") {
                                    inCode = 0xDF;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0x90;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xD0;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            opCode = value;
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            currentAddress += 2;
                        }
                        else if (value < 0x10000) {
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0xB9;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xF9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0xBB;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xFB;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0xB4;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xF4;
                                }
                                else if (in == "ASL") {
                                    inCode = 0x78;
                                }
                                else if (in == "ASR") {
                                    inCode = 0x77;
                                }
                                else if (in == "BITA") {
                                    inCode = 0xB5;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xF5;
                                }
                                else if (in == "CLR") {
                                    inCode = 0x7F;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0xB1;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xF1;
                                }
                                else if (in == "COM") {
                                    inCode = 0x73;
                                }
                                else if (in == "CPX") {
                                    inCode = 0xBC;
                                }
                                else if (in == "DEC") {
                                    inCode = 0x7A;
                                }
                                else if (in == "EORA") {
                                    inCode = 0xB8;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xF8;
                                }
                                else if (in == "INC") {
                                    inCode = 0x7C;
                                }
                                else if (in == "JMP") {
                                    inCode = 0x7E;
                                }
                                else if (in == "JSR") {
                                    inCode = 0xBD;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0xB6;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xF6;
                                }
                                else if (in == "LDS") {
                                    inCode = 0xBE;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xFE;
                                }
                                else if (in == "LSR") {
                                    inCode = 0x74;
                                }
                                else if (in == "NEG") {
                                    inCode = 0x70;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0xBA;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xFA;
                                }
                                else if (in == "ROL") {
                                    inCode = 0x79;
                                }
                                else if (in == "ROR") {
                                    inCode = 0x76;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0xB2;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xF2;
                                }
                                else if (in == "STAA") {
                                    inCode = 0xB7;
                                }
                                else if (in == "STAB") {
                                    inCode = 0xF7;
                                }
                                else if (in == "STS") {
                                    inCode = 0xBF;
                                }
                                else if (in == "STX") {
                                    inCode = 0xFF;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0xB0;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xF0;
                                }
                                else if (in == "TST") {
                                    inCode = 0x7D;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            opCode = (value >> 8) & 0xFF;
                            opCode2 = value & 0xFF;
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            Memory[currentAddress + 2] = opCode2;
                            currentAddress += 3;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else if (razInstructionsM6803.contains(in)) {
                        if (value < 0x10000) {
                            if (true) {
                                if (in == "ADCA") {
                                    inCode = 0xB9;
                                }
                                else if (in == "ADCB") {
                                    inCode = 0xF9;
                                }
                                else if (in == "ADDA") {
                                    inCode = 0xBB;
                                }
                                else if (in == "ADDB") {
                                    inCode = 0xFB;
                                }
                                else if (in == "ADDD") {
                                    inCode = 0xF3;
                                }
                                else if (in == "LDD") {
                                    inCode = 0xFC;
                                }
                                else if (in == "STD") {
                                    inCode = 0xFD;
                                }
                                else if (in == "SUBD") {
                                    inCode = 0xB3;
                                }
                                else if (in == "ANDA") {
                                    inCode = 0xB4;
                                }
                                else if (in == "ANDB") {
                                    inCode = 0xF4;
                                }
                                else if (in == "ASL" || in == "LSL") {
                                    inCode = 0x78;
                                }
                                else if (in == "ASR") {
                                    inCode = 0x77;
                                }
                                else if (in == "BITA") {
                                    inCode = 0xB5;
                                }
                                else if (in == "BITB") {
                                    inCode = 0xF5;
                                }
                                else if (in == "CLR") {
                                    inCode = 0x7F;
                                }
                                else if (in == "CMPA") {
                                    inCode = 0xB1;
                                }
                                else if (in == "CMPB") {
                                    inCode = 0xF1;
                                }
                                else if (in == "COM") {
                                    inCode = 0x73;
                                }
                                else if (in == "CPX") {
                                    inCode = 0xBC;
                                }
                                else if (in == "DEC") {
                                    inCode = 0x7A;
                                }
                                else if (in == "EORA") {
                                    inCode = 0xB8;
                                }
                                else if (in == "EORB") {
                                    inCode = 0xF8;
                                }
                                else if (in == "INC") {
                                    inCode = 0x7C;
                                }
                                else if (in == "JMP") {
                                    inCode = 0x7E;
                                }
                                else if (in == "JSR") {
                                    inCode = 0xBD;
                                }
                                else if (in == "LDAA") {
                                    inCode = 0xB6;
                                }
                                else if (in == "LDAB") {
                                    inCode = 0xF6;
                                }
                                else if (in == "LDS") {
                                    inCode = 0xBE;
                                }
                                else if (in == "LDX") {
                                    inCode = 0xFE;
                                }
                                else if (in == "LSR") {
                                    inCode = 0x74;
                                }
                                else if (in == "NEG") {
                                    inCode = 0x70;
                                }
                                else if (in == "ORAA") {
                                    inCode = 0xBA;
                                }
                                else if (in == "ORAB") {
                                    inCode = 0xFA;
                                }
                                else if (in == "ROL") {
                                    inCode = 0x79;
                                }
                                else if (in == "ROR") {
                                    inCode = 0x76;
                                }
                                else if (in == "SBCA") {
                                    inCode = 0xB2;
                                }
                                else if (in == "SBCB") {
                                    inCode = 0xF2;
                                }
                                else if (in == "STAA") {
                                    inCode = 0xB7;
                                }
                                else if (in == "STAB") {
                                    inCode = 0xF7;
                                }
                                else if (in == "STS") {
                                    inCode = 0xBF;
                                }
                                else if (in == "STX") {
                                    inCode = 0xFF;
                                }
                                else if (in == "SUBA") {
                                    inCode = 0xB0;
                                }
                                else if (in == "SUBB") {
                                    inCode = 0xF0;
                                }
                                else if (in == "TST") {
                                    inCode = 0x7D;
                                }
                                else {
                                    Err("Unknown instruction");
                                    goto end;
                                }
                            }
                            opCode = (value >> 8) & 0xFF;
                            opCode2 = value & 0xFF;
                            Memory[currentAddress] = inCode;
                            Memory[currentAddress + 1] = opCode;
                            Memory[currentAddress + 2] = opCode2;
                            currentAddress += 3;
                        }
                        else {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                    }
                    else {
                        Err("Instruction does not take direct data");
                        goto end;
                    }
                }

            }
            else {
                Err("Instruction does not take relative data");
                goto end;
            }
		}
    skipLine:
        if (currentAddress == instructionAddress) {
            addInstruction(-1, in, op, inCode, opCode, opCode2);
        }
        else {
            addInstruction(instructionAddress, in, op, inCode, opCode, opCode2);
        }
        currentLine++;
    }
    // naj compiler pokaze na katere lineje je nedifenrane onej
    for (const auto& entry : callLabelMap) {
        int location = entry.first;
        QString label = entry.second;
        if(labelValMap.count(label) == 0){
            currentLine = getLineByAddress(location - 1) - 1;
            Err("Use of undeclared label: '" + label + "'"); goto end;
        }else{
            int value = labelValMap[label];
            if (value > 255){
                PrintConsole("Value ("+QString::number(value)+") called by '" +label+"'at location: '" + QString::number(location)+"' is out of instructions range. Entered last byte",1);
                value = value & 0xFF;
                Memory[location] = value;
            }else{
                value = value & 0xFF;
                Memory[location] = value;
            }
        }
    }
    for (const auto& entry : callLabelRazMap) {
        int location = entry.first;
        QString label = entry.second;
        if(labelValMap.count(label) == 0){
            currentLine = getLineByAddress(location - 1) - 1;
            Err("Use of undeclared label: " + label); goto end;
        }else{
            Memory[location] = (labelValMap[label] >> 8) & 0xFF;
            Memory[location + 1] = labelValMap[label] & 0xFF;
        }
    }
    for (const auto& entry : callLabelRelMap) {
        int location = entry.first;
        QString label = entry.second;
        if(labelValMap.count(label) == 0){
            currentLine = getLineByAddress(location - 1) - 1;
            Err("Use of undeclared label: " + label); goto end;
        }else{
            int location2 = labelValMap[label];
            int value;
            value = location2 - location -1;
                if (value > 127 || value < -128) {
                    Err("Relative address out of range[-128,127]: " + QString::number(value));
                    currentLine = getLineByAddress(location - 1) - 1;
                    goto end;
                }
            qint8 signedValue = static_cast<qint8>(value);
            value = signedValue & 0xFF;
            Memory[location] = value;

        }
    }
    std::memcpy(backupMemory, Memory, sizeof(Memory));
    updateLinesBox();
    updateMemoryTab();
    compiled = 1;
end:
    if (lines.size() != currentLine) {
        updateSelectionCompileError(charNum);
        return false;
    }else{
        return true;
    }
}
