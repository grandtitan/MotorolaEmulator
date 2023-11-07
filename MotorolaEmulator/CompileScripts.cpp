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
#include <QInputDialog>


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
    currentCompilerLine = 0;
    currentCompilerAddress = 0;
    std::fill(std::begin(Memory), std::end(Memory), static_cast<uint8_t>(0));
    std::fill(std::begin(backupMemory), std::end(backupMemory), static_cast<uint8_t>(0));
    labelValMap.clear();
    callLabelMap.clear();
    callLabelRazMap.clear();
    callLabelRelMap.clear();
    instructionList.clear();
    QString code = ui->plainTextCode->toPlainText();
    QStringList lines = code.split("\n");
    int charNum = 0;
	foreach (QString line, lines) {
        currentCompilerAddress = currentCompilerAddress % 0xFFFF;
        int instructionAddress = currentCompilerAddress;
        QString label;
        QString in;
        QString op;
        charNum = 0;
        int inCode = 0;
        int opCode = 0;
        int opCode2 = 0;
        bool spec = false;
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
    //labelExtraction
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
            spec = in[0] == '.';
            if(!spec){
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
        }
        else {
            Err("Unexpected character: '" % line[charNum] % "'");
            goto end;
        }
	operationIdentification:
        if(!spec){
            op = op.toUpper();
        }
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
                for (int var = 0; var <= op.count(","); ++var) {
                    QString curOp = op.split(",")[var];
                    if(curOp.isEmpty()){ Err("Invalid syntax"); goto end;}
                    int value = 0;
                    if (curOp[0].isLetter()) {
                        Err("Cannot use label in this directive");
                        goto end;
                    }
                    else if(curOp.length() == 2 && curOp[0] == '\''){
                        ushort unicodeValue = curOp[1].unicode();
                        if (unicodeValue <= 128) {
                            curOp = QString::number(static_cast<int>(unicodeValue));
                        } else {
                            Err("Invlaid ASCII character");
                            goto end;
                        }
                    }
                        try {
                            value = getNum(curOp);
                        }
                        catch (...) {
                            Err("Invalid number: '" % curOp % "'");
                            goto end;
                        }
                        if (value > 255) {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }

                        Memory[currentCompilerAddress] = value;
                        if(label != ""){
                            if (labelValMap.count(label) == 0) {
                                labelValMap[label] = currentCompilerAddress;
                            } else {
                                Err("Label already declared: '" + label + "'");
                                goto end;
                            }
                            PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
                        }
                        currentCompilerAddress += 1;

                }
                goto skipLine;
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
                        if(op.length() == 2 && op[0] == '\''){
                                ushort unicodeValue = op[1].unicode();
                                if (unicodeValue <= 128) {
                                op = QString::number(static_cast<int>(unicodeValue));
                                } else {
                                Err("Invlaid ASCII character");
                                goto end;
                                }
                        }
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
                            PrintConsole("Assigned:'" + QString::number(value) + "' to:'" + label + "'", -1);
                        }
                        else {
                            Err("Label already declared: '" + label + "'");
                            goto end;
                        }
                        goto skipLine;
                    }
                }
            }
            else if (in == ".ORG"){
                if(label != ""){
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentCompilerAddress;
                        PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
                    } else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                }
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
                        if (value > 0x10000) {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                        if (value < currentCompilerAddress){
                            Err("Backward reference not permitted: " + QString::number(value));
                            goto end;
                        }
                        currentCompilerAddress = value;
                        goto skipLine;
                    }
                }

            }
            else if (in == ".WORD") {
                if (op == "") {
                    Err("Missing operand");
                    goto end;
                }
                for (int var = 0; var <= op.count(","); ++var) {
                    QString curOp = op.split(",")[var];
                    if(curOp.isEmpty()){ Err("Invalid syntax"); goto end;}
                    if (curOp[0].isLetter()) {
                        Err("Cannot use label in this directive");
                        goto end;
                    }
                    else if(curOp.length() == 2 && curOp[0] == '\''){
                        ushort unicodeValue = curOp[1].unicode();
                        if (unicodeValue <= 128) {
                            curOp = QString::number(static_cast<int>(unicodeValue));
                        } else {
                            Err("Invlaid ASCII character");
                            goto end;
                        }
                    }
                        int value = 0;
                        try {
                            value = getNum(curOp);
                        }
                        catch (...) {
                            Err("Invalid number: '" % curOp % "'");
                            goto end;
                        }
                        if (value > 0xFFFF) {
                            Err("Value out of range: " + QString::number(value));
                            goto end;
                        }
                        opCode = (value >> 8) & 0xFF;
                        opCode2 = value & 0xFF;
                        Memory[currentCompilerAddress] = opCode;
                        Memory[currentCompilerAddress+1] = opCode2;
                        if(label != ""){
                            if (labelValMap.count(label) == 0) {
                                labelValMap[label] = currentCompilerAddress;
                                PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
                            } else {
                                Err("Label already declared: '" + label + "'");
                                goto end;
                            }
                        }
                        currentCompilerAddress += 2;
                }
                goto skipLine;
            }
            else if(in == ".RMB"){
                if(label != ""){
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentCompilerAddress;
                        PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
                    } else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                }
                if(op.isEmpty()){ Err("Invalid syntax"); goto end;}
                if (op[0].isLetter()) {
                    Err("Cannot use label in this directive");
                    goto end;
                }
                int value = 0;
                try {
                    value = getNum(op);
                }
                catch (...) {
                    Err("Invalid number: '" % op % "'");
                    goto end;
                }
                if (value > 0xFFFF) {
                    Err("Value out of range: " + QString::number(value));
                    goto end;
                }
                currentCompilerAddress+=value;
                goto skipLine;
            } else if(in == ".SETW"){
                if (op.count(",") != 1){
                    Err("Invalid syntax");
                    goto end;
                }
                QString adrOp = op.split(",")[0];
                if(adrOp.isEmpty()){ Err("Invalid syntax"); goto end;}
                if (adrOp[0].isLetter()) {
                    Err("Cannot use label in this directive");
                    goto end;
                }
                int adr = 0;
                try {
                    adr = getNum(adrOp);
                }
                catch (...) {
                    Err("Invalid number: '" % adrOp % "'");
                    goto end;
                }
                if (adr > 0xFFFF) {
                    Err("Value out of range: " + QString::number(adr));
                    goto end;
                }
                QString curOp = op.split(",")[1];
                if(curOp.isEmpty()){ Err("Invalid syntax"); goto end;}
                if (curOp[0].isLetter()) {
                    Err("Cannot use label in this directive");
                    goto end;
                }
                else if(curOp.length() == 2 && curOp[0] == '\''){
                    ushort unicodeValue = curOp[1].unicode();
                    if (unicodeValue <= 128) {
                        curOp = QString::number(static_cast<int>(unicodeValue));
                    } else {
                        Err("Invlaid ASCII character");
                        goto end;
                    }
                }
                int value = 0;
                try {
                    value = getNum(curOp);
                }
                catch (...) {
                    Err("Invalid number: '" % curOp % "'");
                    goto end;
                }
                if (value > 0xFFFF) {
                    Err("Value out of range: " + QString::number(value));
                    goto end;
                }
                opCode = (value >> 8) & 0xFF;
                opCode2 = value & 0xFF;
                Memory[adr] = opCode;
                Memory[adr+1] = opCode2;
                if(label != ""){
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = adr;
                        PrintConsole("Assigned:'" + QString::number(adr) + "' to:'" + label + "'", -1);
                    } else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                }
                goto skipLine;
            }
            else if(in == ".SETB"){
                if (op.count(",") != 1){
                    Err("Invalid syntax");
                    goto end;
                }
                QString adrOp = op.split(",")[0];
                if(adrOp.isEmpty()){ Err("Invalid syntax"); goto end;}
                if (adrOp[0].isLetter()) {
                    Err("Cannot use label in this directive");
                    goto end;
                }
                int adr = 0;
                try {
                    adr = getNum(adrOp);
                }
                catch (...) {
                    Err("Invalid number: '" % adrOp % "'");
                    goto end;
                }
                if (adr > 0xFFFF) {
                    Err("Value out of range: " + QString::number(adr));
                    goto end;
                }
                QString curOp = op.split(",")[1];
                if(curOp.isEmpty()){ Err("Invalid syntax"); goto end;}
                if (curOp[0].isLetter()) {
                    Err("Cannot use label in this directive");
                    goto end;
                }
                else if(curOp.length() == 2 && curOp[0] == '\''){
                    ushort unicodeValue = curOp[1].unicode();
                    if (unicodeValue <= 128) {
                        curOp = QString::number(static_cast<int>(unicodeValue));
                    } else {
                        Err("Invlaid ASCII character");
                        goto end;
                    }
                }
                int value = 0;
                try {
                    value = getNum(curOp);
                }
                catch (...) {
                    Err("Invalid number: '" % curOp % "'");
                    goto end;
                }
                if (value > 0xFF) {
                    Err("Value out of range: " + QString::number(value));
                    goto end;
                }
                Memory[adr] = value;
                if(label != ""){
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = adr;
                        PrintConsole("Assigned:'" + QString::number(adr) + "' to:'" + label + "'", -1);
                    } else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                }
                goto skipLine;
            }
            else if(in == ".STR"){
                    if(op.isEmpty()){ Err("Invalid syntax"); goto end;}
                    if (op[0] != '"' || op.length() < 3){
                        Err("Invalid syntax");
                        goto end;
                    }
                    if(op.at(op.length() - 1) != '"'){
                            Err("Invalid syntax");
                            goto end;
                    }
                    op = op.mid(1, op.length() - 2);
                    for (int i = 0; i < op.length(); i++) {
                        Memory[currentCompilerAddress] = static_cast<int>(op.at(i).unicode());
                        currentCompilerAddress++;
                    }

                goto skipLine;
            }
            else{
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
                        labelValMap[label] = currentCompilerAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
                }
                Memory[currentCompilerAddress] = inCode;
                currentCompilerAddress += 1;
                goto skipLine;
            }
            else {
                if (op.isEmpty()) {
                    Err("Missing operand");
                    goto end;
                }
                if (!label.isEmpty()) {
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentCompilerAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
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
                    callLabelRelMap[currentCompilerAddress + 1] = op;
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
                Memory[currentCompilerAddress] = inCode;
                Memory[currentCompilerAddress + 1] = opCode;
                currentCompilerAddress += 2;
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
                        callLabelMap[currentCompilerAddress + 1] = op;
                    }else{
                        int value = labelValMap[op];
                        if (value > 255){
                            PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentCompilerAddress)+"' is out of instructions range. Entered last byte",1);
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
                Memory[currentCompilerAddress] = inCode;
                Memory[currentCompilerAddress + 1] = opCode;
                currentCompilerAddress += 2;
            }
            else if (op.startsWith("#")) {
                op = op.sliced(1);
                opCode = 0;
                opCode2 = 0;
                if (op[0].isLetter()) { //TAK LAB
                    if (takRazInstructionsM6800.indexOf(in) != -1) { // TAK RAZ LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentCompilerAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                opCode = (value >> 8) & 0xFF;
                                opCode2 = value & 0xFF;
                            }else{
                                opCode = value;
                            }
                        }
                        Memory[currentCompilerAddress + 1] = opCode;
                        Memory[currentCompilerAddress + 2] = opCode2;
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
                        Memory[currentCompilerAddress] = inCode;
                        currentCompilerAddress += 3;
                    }
                    else if (takInstructionsM6800.contains(in)) { //tak LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentCompilerAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentCompilerAddress)+"' is out of instructions range. Entered last byte",1);
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        currentCompilerAddress += 2;
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
                            Memory[currentCompilerAddress + 1] = opCode;
                            Memory[currentCompilerAddress + 2] = opCode2;
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
                            Memory[currentCompilerAddress] = inCode;
                            currentCompilerAddress += 3;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            currentCompilerAddress += 2;
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        currentCompilerAddress += 2;
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        Memory[currentCompilerAddress + 2] = opCode2;
                        currentCompilerAddress += 3;
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        Memory[currentCompilerAddress + 2] = opCode2;
                        currentCompilerAddress += 3;
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
                            callLabelRazMap[currentCompilerAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentCompilerAddress] = inCode;
                        currentCompilerAddress += 3;
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
                            callLabelMap[currentCompilerAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentCompilerAddress] = inCode;
                        currentCompilerAddress += 2;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            currentCompilerAddress += 2;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            Memory[currentCompilerAddress + 2] = opCode2;
                            currentCompilerAddress += 3;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            Memory[currentCompilerAddress + 2] = opCode2;
                            currentCompilerAddress += 3;
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
                        labelValMap[label] = currentCompilerAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
                }
                Memory[currentCompilerAddress] = inCode;
                currentCompilerAddress += 1;
                goto skipLine;
            }
            else {
                if (op.isEmpty()) {
                    Err("Missing operand");
                    goto end;
                }
                if (!label.isEmpty()) {
                    if (labelValMap.count(label) == 0) {
                        labelValMap[label] = currentCompilerAddress;
                    }
                    else {
                        Err("Label already declared: '" + label + "'");
                        goto end;
                    }
                    PrintConsole("Assigned:'" + QString::number(currentCompilerAddress) + "' to:'" + label + "'", -1);
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
                    callLabelRelMap[currentCompilerAddress + 1] = op;
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
                Memory[currentCompilerAddress] = inCode;
                Memory[currentCompilerAddress + 1] = opCode;
                currentCompilerAddress += 2;
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
                        callLabelMap[currentCompilerAddress + 1] = op;
                    }else{
                        int value = labelValMap[op];
                        if (value > 255){
                            PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentCompilerAddress)+"' is out of instructions range. Entered last byte",1);
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
                Memory[currentCompilerAddress] = inCode;
                Memory[currentCompilerAddress + 1] = opCode;
                currentCompilerAddress += 2;
            }
            else if (op.startsWith("#")) {
                op = op.sliced(1);
                opCode = 0;
                opCode2 = 0;
                if (op[0].isLetter()) { //TAK LAB
                    if (takRazInstructionsM6803.indexOf(in) != -1) { // TAK RAZ LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentCompilerAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                opCode = (value >> 8) & 0xFF;
                                opCode2 = value & 0xFF;
                            }else{
                                opCode = value;
                            }
                        }
                        Memory[currentCompilerAddress + 1] = opCode;
                        Memory[currentCompilerAddress + 2] = opCode2;
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
                        Memory[currentCompilerAddress] = inCode;
                        currentCompilerAddress += 3;
                    }
                    else if (takInstructionsM6803.contains(in)) { //tak LAB
                        if(labelValMap.count(op) == 0){
                            opCode = 0;
                            callLabelMap[currentCompilerAddress + 1] = op;
                        }else{
                            int value = labelValMap[op];
                            if (value > 255){
                                PrintConsole("Value ("+QString::number(value)+") called by '" +op+"'at location: '" + QString::number(currentCompilerAddress)+"' is out of instructions range. Entered last byte",1);
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        currentCompilerAddress += 2;
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
                            Memory[currentCompilerAddress + 1] = opCode;
                            Memory[currentCompilerAddress + 2] = opCode2;
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
                            Memory[currentCompilerAddress] = inCode;
                            currentCompilerAddress += 3;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            currentCompilerAddress += 2;
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        currentCompilerAddress += 2;
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        Memory[currentCompilerAddress + 2] = opCode2;
                        currentCompilerAddress += 3;
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
                        Memory[currentCompilerAddress] = inCode;
                        Memory[currentCompilerAddress + 1] = opCode;
                        Memory[currentCompilerAddress + 2] = opCode2;
                        currentCompilerAddress += 3;
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
                            callLabelRazMap[currentCompilerAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentCompilerAddress] = inCode;
                        currentCompilerAddress += 3;
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
                            callLabelMap[currentCompilerAddress + 1] = op;
                            opCode = 0;
                        }
                        Memory[currentCompilerAddress] = inCode;
                        currentCompilerAddress += 2;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            currentCompilerAddress += 2;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            Memory[currentCompilerAddress + 2] = opCode2;
                            currentCompilerAddress += 3;
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
                            Memory[currentCompilerAddress] = inCode;
                            Memory[currentCompilerAddress + 1] = opCode;
                            Memory[currentCompilerAddress + 2] = opCode2;
                            currentCompilerAddress += 3;
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
        if (currentCompilerAddress == instructionAddress) {
            instructionList.addInstruction(-1, currentCompilerLine, inCode, opCode, opCode2);
        }
        else {
            instructionList.addInstruction(instructionAddress, currentCompilerLine, inCode, opCode, opCode2);
        }
        currentCompilerLine++;
    }
    // naj compiler pokaze na katere lineje je nedifenrane onej
    for (const auto& entry : callLabelMap) {
        int location = entry.first;
        QString label = entry.second;
        if(labelValMap.count(label) == 0){
            currentCompilerLine = instructionList.getObjectByAddress(location - 1).lineNumber;
            charNum = 0xFFFF;
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
            currentCompilerLine = instructionList.getObjectByAddress(location - 1).lineNumber;
            charNum = 0xFFFF;
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
            currentCompilerLine = instructionList.getObjectByAddress(location - 1).lineNumber;
            charNum = 0xFFFF;
            Err("Use of undeclared label: " + label); goto end;
        }else{
            int location2 = labelValMap[label];
            int value;
            value = location2 - location -1;
                if (value > 127 || value < -128) {
                    Err("Relative address out of range[-128,127]: " + QString::number(value));
                    currentCompilerLine = instructionList.getObjectByAddress(location - 1).lineNumber ;
                    charNum = 0xFFFF;
                    goto end;
                }
            qint8 signedValue = static_cast<qint8>(value);
            value = signedValue & 0xFF;
            Memory[location] = value;

        }
    }
    std::memcpy(backupMemory, Memory, sizeof(Memory));

end:
    if (lines.size() != currentCompilerLine) {
        setCompileStatus(false);
        updateSelectionCompileError(charNum);
        return false;
    }else{
        setCompileStatus(true);
        return true;
    }
}

int MainWindow::inputNextAddress(int curAdr, QString err){
    bool ok;
    QString text = QInputDialog::getText(this, "Input Dialog", err + ". Missing data will be written with .BYTE. Enter decimal address of next instruction. Current address: " + QString::number(curAdr,10) + ".", QLineEdit::Normal, QString(), &ok);

    if (ok) {
        bool iok;
        int number = text.toInt(&iok);
        if(iok){
            if (number < curAdr){
                    Err("Next instruction cannot be located before the previous one");
                    return -1;
            }
            return number;
        }else{
            Err("Invalid address");
            return -1;
        }
    } else {
        PrintConsole("Decompile canceled",1);
        return -1;
    }
}

bool MainWindow::reverseCompile(int ver, int begLoc){
    QString code;
    int index = 0;
    int line = 0;
    instructionList.clear();
    int zeroCount = 0;
    int lastIndex = 0xFFFF;
    for (int i = 0xFFFF; i >= 0; i--) {
        if (Memory[i] != 0) {
            lastIndex = i;
            break;
        }
    }
    for (; index <= lastIndex; ) {
        if (index < begLoc){
            code.append("\t.BYTE " + QString::number(Memory[index]) + "\n");
            instructionList.addInstruction(index, line, 0, 0, 0);
            index++;
            line++;
        }else{
            int inSize = 1;
            int inType = -2; //  -3 m6803 -2 unkown -1 zero 0 inh 1 imm 2 dir 3 ind 4 ext 5 rel
            QString in;
            switch (Memory[index]){
            case 0x00:
                    inType = -1;
                    break;
            case 0x01:
                    in = "NOP";
                    inType = 0;
                    break;
            case 0x04:
                    if(ver < 1){ inType = -3; break; }
                    in = "LSRD";
                    inType = 0;
                    break;
            case 0x05:
                    if(ver < 1){ inType = -3; break; }
                    in = "ASLD";
                    inType = 0;
                    break;
            case 0x06:
                    in = "TAP";
                    inType = 0;
                    break;
            case 0x07:
                    in = "TPA";
                    inType = 0;
                    break;
            case 0x08:
                    in = "INX";
                    inType = 0;
                    break;
            case 0x09:
                    in = "DEX";
                    inType = 0;
                    break;
            case 0x0A:
                    in = "CLV";
                    inType = 0;
                    break;
            case 0x0B:
                    in = "SEV";
                    inType = 0;
                    break;
            case 0x0C:
                    in = "CLC";
                    inType = 0;
                    break;
            case 0x0D:
                    in = "SEC";
                    inType = 0;
                    break;
            case 0x0E:
                    in = "CLI";
                    inType = 0;
                    break;
            case 0x0F:
                    in = "SEI";
                    inType = 0;
                    break;
            case 0x10:
                    in = "SBA";
                    inType = 0;
                    break;
            case 0x11:
                    in = "CBA";
                    inType = 0;
                    break;
            case 0x16:
                    in = "TAB";
                    inType = 0;
                    break;
            case 0x17:
                    in = "TBA";
                    inType = 0;
                    break;
            case 0x19:
                    in = "DAA";
                    inType = 0;
                    break;
            case 0x1B:
                    in = "ABA";
                    inType = 0;
                    break;
            case 0x20:
                    in = "BRA";
                    inType = 5;
                    break;
            case 0x21:
                    if(ver < 1){ inType = -3; break; }
                    in = "BRN";
                    inType = 5;
                    break;
            case 0x22:
                    in = "BHI";
                    inType = 5;
                    break;
            case 0x23:
                    in = "BLS";
                    inType = 5;
                    break;
            case 0x24:
                    in = "BCC";
                    inType = 5;
                    break;
            case 0x25:
                    in = "BCS";
                    inType = 5;
                    break;
            case 0x26:
                    in = "BNE";
                    inType = 5;
                    break;
            case 0x27:
                    in = "BEQ";
                    inType = 5;
                    break;
            case 0x28:
                    in = "BVC";
                    inType = 5;
                    break;
            case 0x29:
                    in = "BVS";
                    inType = 5;
                    break;
            case 0x2A:
                    in = "BPL";
                    inType = 5;
                    break;
            case 0x2B:
                    in = "BMI";
                    inType = 5;
                    break;
            case 0x2C:
                    in = "BGE";
                    inType = 5;
                    break;
            case 0x2D:
                    in = "BLT";
                    inType = 5;
                    break;
            case 0x2E:
                    in = "BGT";
                    inType = 5;
                    break;
            case 0x2F:
                    in = "BLE";
                    inType = 5;
                    break;
            case 0x30:
                    in = "TSX";
                    inType = 0;
                    break;
            case 0x31:
                    in = "INS";
                    inType = 0;
                    break;
            case 0x32:
                    in = "PULA";
                    inType = 0;
                    break;
            case 0x33:
                    in = "PULB";
                    inType = 0;
                    break;
            case 0x34:
                    in = "DES";
                    inType = 0;
                    break;
            case 0x35:
                    in = "TXS";
                    inType = 0;
                    break;
            case 0x36:
                    in = "PSHA";
                    inType = 0;
                    break;
            case 0x37:
                    in = "PSHB";
                    inType = 0;
                    break;
            case 0x38:
                    if(ver < 1){ inType = -3; break; }
                    in = "PULX";
                    inType = 0;
                    break;
            case 0x39:
                    in = "RTS";
                    inType = 0;
                    break;
            case 0x3A:
                    if(ver < 1){ inType = -3; break; }
                    in = "ABX";
                    inType = 0;
                    break;
            case 0x3B:
                    in = "RTI";
                    inType = 0;
                    break;
            case 0x3C:
                    if(ver < 1){ inType = -3; break; }
                    in = "PSHX";
                    inType = 0;
                    break;
            case 0x3D:
                    if(ver < 1){ inType = -3; break; }
                    in = "MUL";
                    inType = 0;
                    break;
            case 0x3E:
                    in = "WAI";
                    inType = 0;
                    break;
            case 0x3F:
                    in = "SWI";
                    inType = 0;
                    break;
            case 0x40:
                    in = "NEGA";
                    inType = 0;
                    break;
            case 0x43:
                    in = "COMA";
                    inType = 0;
                    break;
            case 0x44:
                    in = "LSRA";
                    inType = 0;
                    break;
            case 0x46:
                    in = "RORA";
                    inType = 0;
                    break;
            case 0x47:
                    in = "ASRA";
                    inType = 0;
                    break;
            case 0x48:
                    in = "ASLA";
                    inType = 0;
                    break;
            case 0x49:
                    in = "ROLA";
                    inType = 0;
                    break;
            case 0x4A:
                    in = "DECA";
                    inType = 0;
                    break;
            case 0x4C:
                    in = "INCA";
                    inType = 0;
                    break;
            case 0x4D:
                    in = "TSTA";
                    inType = 0;
                    break;
            case 0x4F:
                    in = "CLRA";
                    inType = 0;
                    break;
            case 0x50:
                    in = "NEGB";
                    inType = 0;
                    break;
            case 0x53:
                    in = "COMB";
                    inType = 0;
                    break;
            case 0x54:
                    in = "LSRB";
                    inType = 0;
                    break;
            case 0x56:
                    in = "RORB";
                    inType = 0;
                    break;
            case 0x57:
                    in = "ASRB";
                    inType = 0;
                    break;
            case 0x58:
                    in = "ASLB";
                    inType = 0;
                    break;
            case 0x59:
                    in = "ROLB";
                    inType = 0;
                    break;
            case 0x5A:
                    in = "DECB";
                    inType = 0;
                    break;
            case 0x5C:
                    in = "INCB";
                    inType = 0;
                    break;
            case 0x5D:
                    in = "TSTB";
                    inType = 0;
                    break;
            case 0x5F:
                    in = "CLRB";
                    inType = 0;
                    break;
            case 0x60:
                    in = "NEG";
                    inType = 3;
                    break;
            case 0x63:
                    in = "COM";
                    inType = 3;
                    break;
            case 0x64:
                    in = "LSR";
                    inType = 3;
                    break;
            case 0x66:
                    in = "ROR";
                    inType = 3;
                    break;
            case 0x67:
                    in = "ASR";
                    inType = 3;
                    break;
            case 0x68:
                    in = "ASL";
                    inType = 3;
                    break;
            case 0x69:
                    in = "ROL";
                    inType = 3;
                    break;
            case 0x6A:
                    in = "DEC";
                    inType = 3;
                    break;
            case 0x6C:
                    in = "INC";
                    inType = 3;
                    break;
            case 0x6D:
                    in = "TST";
                    inType = 3;
                    break;
            case 0x6E:
                    in = "JMP";
                    inType = 3;
                    break;
            case 0x6F:
                    in = "CLR";
                    inType = 3;
                    break;
            case 0x70:
                    in = "NEG";
                    inType = 4;
                    break;
            case 0x73:
                    in = "COM";
                    inType = 4;
                    break;
            case 0x74:
                    in = "LSR";
                    inType = 4;
                    break;
            case 0x76:
                    in = "ROR";
                    inType = 4;
                    break;
            case 0x77:
                    in = "ASR";
                    inType = 4;
                    break;
            case 0x78:
                    in = "ASL";
                    inType = 4;
                    break;
            case 0x79:
                    in = "ROL";
                    inType = 4;
                    break;
            case 0x7A:
                    in = "DEC";
                    inType = 4;
                    break;
            case 0x7C:
                    in = "INC";
                    inType = 4;
                    break;
            case 0x7D:
                    in = "TST";
                    inType = 4;
                    break;
            case 0x7E:
                    in = "JMP";
                    inType = 4;
                    break;
            case 0x7F:
                    in = "CLR";
                    inType = 4;
                    break;
            case 0x80:
                    in = "SUBA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x81:
                    in = "CMPA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x82:
                    in = "SBCA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x83:
                    if(ver < 1){ inType = -3; break; }
                    in = "SUBD";
                    inType = 1;
                    inSize = 3;
                    break;
            case 0x84:
                    in = "ANDA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x85:
                    in = "BITA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x86:
                    in = "LDAA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x88:
                    in = "EORA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x89:
                    in = "ADCA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x8A:
                    in = "ORAA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x8B:
                    in = "ADDA";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0x8C:
                    in = "CPX";
                    inType = 1;
                    inSize = 3;
                    break;
            case 0x8D:
                    in = "BSR";
                    inType = 5;
                    break;
            case 0x8E:
                    in = "LDS";
                    inType = 1;
                    inSize = 3;
                    break;
            case 0x90:
                    in = "SUBA";
                    inType = 2;
                    break;
            case 0x91:
                    in = "CMPA";
                    inType = 2;
                    break;
            case 0x92:
                    in = "SBCA";
                    inType = 2;
                    break;
            case 0x93:
                    if(ver < 1){ inType = -3; break; }
                    in = "SUBD";
                    inType = 2;
                    break;
            case 0x94:
                    in = "ANDA";
                    inType = 2;
                    break;
            case 0x95:
                    in = "BITA";
                    inType = 2;
                    break;
            case 0x96:
                    in = "LDAA";
                    inType = 2;
                    break;
            case 0x97:
                    in = "STAA";
                    inType = 2;
                    break;
            case 0x98:
                    in = "EORA";
                    inType = 2;
                    break;
            case 0x99:
                    in = "ADCA";
                    inType = 2;
                    break;
            case 0x9A:
                    in = "ORAA";
                    inType = 2;
                    break;
            case 0x9B:
                    in = "ADDA";
                    inType = 2;
                    break;
            case 0x9C:
                    in = "CPX";
                    inType = 2;
                    break;
            case 0x9D:
                    if(ver < 1){ inType = -3; break; }
                    in = "JSR";
                    inType = 2;
                    break;
            case 0x9E:
                    in = "LDS";
                    inType = 2;
                    break;
            case 0x9F:
                    in = "STS";
                    inType = 2;
                    break;
            case 0xA0:
                    in = "SUBA";
                    inType = 3;
                    break;
            case 0xA1:
                    in = "CMPA";
                    inType = 3;
                    break;
            case 0xA2:
                    in = "SBCA";
                    inType = 3;
                    break;
            case 0xA3:
                    if(ver < 1){ inType = -3; break; }
                    in = "SUBD";
                    inType = 3;
                    break;
            case 0xA4:
                    in = "ANDA";
                    inType = 3;
                    break;
            case 0xA5:
                    in = "BITA";
                    inType = 3;
                    break;
            case 0xA6:
                    in = "LDAA";
                    inType = 3;
                    break;
            case 0xA7:
                    in = "STAA";
                    inType = 3;
                    break;
            case 0xA8:
                    in = "EORA";
                    inType = 3;
                    break;
            case 0xA9:
                    in = "ADCA";
                    inType = 3;
                    break;
            case 0xAA:
                    in = "ORAA";
                    inType = 3;
                    break;
            case 0xAB:
                    in = "ADDA";
                    inType = 3;
                    break;
            case 0xAC:
                    in = "CPX";
                    inType = 3;
                    break;
            case 0xAD:
                    in = "JSR";
                    inType = 3;
                    break;
            case 0xAE:
                    in = "LDS";
                    inType = 3;
                    break;
            case 0xAF:
                    in = "STS";
                    inType = 3;
                    break;
            case 0xB0:
                    in = "SUBA";
                    inType = 4;
                    break;
            case 0xB1:
                    in = "CMPA";
                    inType = 4;
                    break;
            case 0xB2:
                    in = "SBCA";
                    inType = 4;
                    break;
            case 0xB3:
                    if(ver < 1){ inType = -3; break; }
                    in = "SUBD";
                    inType = 4;
                    break;
            case 0xB4:
                    in = "ANDA";
                    inType = 4;
                    break;
            case 0xB5:
                    in = "BITA";
                    inType = 4;
                    break;
            case 0xB6:
                    in = "LDAA";
                    inType = 4;
                    break;
            case 0xB7:
                    in = "STAA";
                    inType = 4;
                    break;
            case 0xB8:
                    in = "EORA";
                    inType = 4;
                    break;
            case 0xB9:
                    in = "ADCA";
                    inType = 4;
                    break;
            case 0xBA:
                    in = "ORAA";
                    inType = 4;
                    break;
            case 0xBB:
                    in = "ADDA";
                    inType = 4;
                    break;
            case 0xBC:
                    in = "CPX";
                    inType = 4;
                    break;
            case 0xBD:
                    in = "JSR";
                    inType = 4;
                    break;
            case 0xBE:
                    in = "LDS";
                    inType = 4;
                    break;
            case 0xBF:
                    in = "STS";
                    inType = 4;
                    break;
            case 0xC0:
                    in = "SUBB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC1:
                    in = "CMPB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC2:
                    in = "SBCB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC3:
                    if(ver < 1){ inType = -3; break; }
                    in = "ADDD";
                    inType = 1;
                    inSize = 3;
                    break;
            case 0xC4:
                    in = "ANDB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC5:
                    in = "BITB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC6:
                    in = "LDAB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC8:
                    in = "EORB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xC9:
                    in = "ADCB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xCA:
                    in = "ORAB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xCB:
                    in = "ADDB";
                    inType = 1;
                    inSize = 2;
                    break;
            case 0xCC:
                    if(ver < 1){ inType = -3; break; }
                    in = "LDD";
                    inType = 1;
                    inSize = 3;
                    break;
            case 0xCE:
                    in = "LDX";
                    inType = 1;
                    inSize = 3;
                    break;
            case 0xD0:
                    in = "SUBB";
                    inType = 2;
                    break;
            case 0xD1:
                    in = "CMPB";
                    inType = 2;
                    break;
            case 0xD2:
                    in = "SBCB";
                    inType = 2;
                    break;
            case 0xD3:
                    if(ver < 1){ inType = -3; break; }
                    in = "ADDD";
                    inType = 2;
                    break;
            case 0xD4:
                    in = "ANDB";
                    inType = 2;
                    break;
            case 0xD5:
                    in = "BITB";
                    inType = 2;
                    break;
            case 0xD6:
                    in = "LDAB";
                    inType = 2;
                    break;
            case 0xD7:
                    in = "STAB";
                    inType = 2;
                    break;
            case 0xD8:
                    in = "EORB";
                    inType = 2;
                    break;
            case 0xD9:
                    in = "ADCB";
                    inType = 2;
                    break;
            case 0xDA:
                    in = "ORAB";
                    inType = 2;
                    break;
            case 0xDB:
                    in = "ADDB";
                    inType = 2;
                    break;
            case 0xDC:
                    if(ver < 1){ inType = -3; break; }
                    in = "LDD";
                    inType = 2;
                    break;
            case 0xDD:
                    if(ver < 1){ inType = -3; break; }
                    in = "STD";
                    inType = 2;
                    break;
            case 0xDE:
                    in = "LDX";
                    inType = 2;
                    break;
            case 0xDF:
                    in = "STX";
                    inType = 2;
                    break;
            case 0xE0:
                    in = "SUBB";
                    inType = 3;
                    break;
            case 0xE1:
                    in = "CMPB";
                    inType = 3;
                    break;
            case 0xE2:
                    in = "SBCB";
                    inType = 3;
                    break;
            case 0xE3:
                    if(ver < 1){ inType = -3; break; }
                    in = "ADDD";
                    inType = 3;
                    break;
            case 0xE4:
                    in = "ANDB";
                    inType = 3;
                    break;
            case 0xE5:
                    in = "BITB";
                    inType = 3;
                    break;
            case 0xE6:
                    in = "LDAB";
                    inType = 3;
                    break;
            case 0xE7:
                    in = "STAB";
                    inType = 3;
                    break;
            case 0xE8:
                    in = "EORB";
                    inType = 3;
                    break;
            case 0xE9:
                    in = "ADCB";
                    inType = 3;
                    break;
            case 0xEA:
                    in = "ORAB";
                    inType = 3;
                    break;
            case 0xEB:
                    in = "ADDB";
                    inType = 3;
                    break;
            case 0xEC:
                    if(ver < 1){ inType = -3; break; }
                    in = "LDD";
                    inType = 3;
                    break;
            case 0xED:
                    if(ver < 1){ inType = -3; break; }
                    in = "STD";
                    inType = 3;
                    break;
            case 0xEE:
                    in = "LDX";
                    inType = 3;
                    break;
            case 0xEF:
                    in = "STX";
                    inType = 3;
                    break;
            case 0xF0:
                    in = "SUBB";
                    inType = 4;
                    break;
            case 0xF1:
                    in = "CMPB";
                    inType = 4;
                    break;
            case 0xF2:
                    in = "SBCB";
                    inType = 4;
                    break;
            case 0xF3:
                    if(ver < 1){ inType = -3; break; }
                    in = "ADDD";
                    inType = 4;
                    break;
            case 0xF4:
                    in = "ANDB";
                    inType = 4;
                    break;
            case 0xF5:
                    in = "BITB";
                    inType = 4;
                    break;
            case 0xF6:
                    in = "LDAB";
                    inType = 4;
                    break;
            case 0xF7:
                    in = "STAB";
                    inType = 4;
                    break;
            case 0xF8:
                    in = "EORB";
                    inType = 4;
                    break;
            case 0xF9:
                    in = "ADCB";
                    inType = 4;
                    break;
            case 0xFA:
                    in = "ORAB";
                    inType = 4;
                    break;
            case 0xFB:
                    in = "ADDB";
                    inType = 4;
                    break;
            case 0xFC:
                    if(ver < 1){ inType = -3; break; }
                    in = "LDD";
                    inType = 4;
                    break;
            case 0xFD:
                    if(ver < 1){ inType = -3; break; }
                    in = "STD";
                    inType = 4;
                    break;
            case 0xFE:
                    in = "LDX";
                    inType = 4;
                    break;
            case 0xFF:
                    in = "STX";
                    inType = 4;
                    break;
            }
            if(zeroCount != 0 && inType != -1){
                    code.append("\t.RMB "+ QString::number(zeroCount) + "\n");
                    instructionList.addInstruction(index - zeroCount, line, 0, 0, 0);
                    line++;
                    zeroCount = 0;
            }
            if(inType == -3){
                PrintConsole("M6803 and higher support instruction at address: " + QString::number(index),1);
                int nextI = inputNextAddress(index, "M6803 and higher support instruction");
                if(nextI == -1){break;}
                for (; index < nextI; ++index) {
                    code.append("\t.BYTE " + QString::number(Memory[index],10) + "\n");
                    instructionList.addInstruction(index, line, 0, 0, 0);
                    line++;
                }
                continue;
            } else if(inType == -2){
                PrintConsole("Unkown instruction at address: " + QString::number(index),1);
                int nextI = inputNextAddress(index, "Unkown instruction");
                if(nextI == -1){break;}
                for (; index < nextI; ++index) {
                    code.append("\t.BYTE " + QString::number(Memory[index],10) + "\n");
                    instructionList.addInstruction(index, line, 0, 0, 0);
                    line++;
                }
                continue;
            } else if(inType == -1){
                zeroCount++;
                index++;
                continue;
            }
            int opCode = 0, opCode2 = 0;
            if (inType == 0){
                    inSize = 1;
                    code.append("\t" + in + "\n");
            }
            else if(inType == 1){
                    code.append("\t" + in + " ");
                    if(inSize == 2){
                        opCode = Memory[index+1 % 0xFFFF];
                        code.append("#" + QString::number(Memory[index+1 % 0xFFFF],10) + "\n");
                    }else{
                        opCode = Memory[index+1 % 0xFFFF];
                        opCode2 = Memory[index+2 % 0xFFFF];
                        code.append("#" + QString::number((Memory[index+1 % 0xFFFF] << 8) + Memory[index+2 % 0xFFFF],10) + "\n");
                    }
            }
            else if(inType == 2){
                    inSize = 2;
                    code.append("\t" + in + " ");
                    opCode = Memory[index+1 % 0xFFFF];
                    code.append(QString::number(Memory[index+1 % 0xFFFF],10) + "\n");
            }
            else if(inType == 3){
                    inSize = 2;
                    code.append("\t" + in + " ");
                    opCode = Memory[index+1 % 0xFFFF];
                    code.append(QString::number(Memory[index+1 % 0xFFFF],10) + ","+ "X" + "\n");
            }
            else if(inType == 4){
                    inSize = 3;
                    code.append("\t" + in + " ");
                    opCode = Memory[index+1 % 0xFFFF];
                    opCode2 = Memory[index+2 % 0xFFFF];
                    code.append(QString::number((Memory[index+1 % 0xFFFF] << 8) + Memory[index+2 % 0xFFFF],10) + "\n");
            }
            else if(inType == 5){
                    inSize = 2;
                    code.append("\t" + in + " ");
                    int8_t num = static_cast<int8_t>(Memory[index+1 % 0xFFFF]);
                    if(num == -1){
                        code.append("0 ;Relative address FF is out of bounds and cannot be reverse compiled\n");
                        PrintConsole("Relative address FF is out of bounds and cannot be reverse compiled",1);
                    }
                    else if(num < 0){
                        num+=2;
                        code.append(QString::number(num,10) + "\n");
                    } else{
                        code.append(QString::number(num,10) + "\n");
                    }
                    opCode = Memory[index+1 % 0xFFFF];
            }
            instructionList.addInstruction(index, line, Memory[index], opCode, opCode2);
            line++;
            index += inSize;
        }

    }
    if(index == lastIndex +1){
        ui->plainTextCode->setPlainText(code);
        std::memcpy(backupMemory, Memory, sizeof(Memory));
        setCompileStatus(true);
        return true;
    }else{
        setCompileStatus(false);
        return false;
    }
}
