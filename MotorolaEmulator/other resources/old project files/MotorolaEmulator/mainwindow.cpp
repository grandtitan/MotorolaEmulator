#include "mainwindow.h"
#include "instructionblock.h"
#include "ui_mainwindow.h"
#include "./ui_mainwindow.h"
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
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    clearInstructions();
    ui->setupUi(this);
    QWidget::setWindowTitle("Motorola M68XX Emulator-"+softwareVersion);
    updateMemoryTab();

    ui->tableWidget->setVisible(false);
    ui->tableWidget->setEnabled(false);
    /*/for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            if (item) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }
    }
    ui->tableWidget->setColumnWidth(0, 150);

    for (int col = 1; col < ui->tableWidget->columnCount(); ++col) {
        ui->tableWidget->setColumnWidth(col, 40);
    }/*/
    connect(ui->plainTextCode->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleVerticalScrollBarValueChanged);
    connect(ui->plainTextLines->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleLinesScroll);
    connect(ui->plainTextDisplay->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleDisplayScrollVertical);
    connect(ui->plainTextDisplay->horizontalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleDisplayScrollHorizontal);
    connect(ui->plainTextMemory->horizontalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleMemoryScrollHorizontal);
    connect(this, &MainWindow::resized, this, &MainWindow::handleMainWindowSizeChanged);

    ui->groupBoxSimpleMemory->setVisible(false);
    ui->groupBoxSimpleMemory->setEnabled(false);


    ui->plainTextDisplay->installEventFilter(this);
    ui->plainTextLines->installEventFilter(this);
    ui->plainTextMemory->installEventFilter(this);

    ui->buttonSwitchWrite->setVisible(false);
    ui->labelWritingMode->setVisible(false);
    ui->buttonSwitchWrite->setEnabled(false);
    ui->labelWritingMode->setEnabled(false);
    ui->buttonFSDisplay->setVisible(false);

    ui->labelAt->setVisible(false);
    ui->spinBoxBreakAt->setVisible(false);

    executionTimer = new QTimer(this);
    connect(executionTimer, &QTimer::timeout, this, &MainWindow::executeInstruction);

    ui->plainTextDisplay->setTextInteractionFlags(Qt::NoTextInteraction);
}
MainWindow::~MainWindow()
{
    delete ui;
}
QString uncompiledButton = "QPushButton {"
                           "    border: 2px solid red;"
                           "}";
QString compiledButton = "QPushButton {"
                           "    border: 2px solid green;"
                           "}";
std::unordered_map<QString, int> labelValMap;
std::unordered_map<int, QString> callLabelMap;
std::unordered_map<int, QString> callLabelRelMap;
std::unordered_map<int, QString> callLabelRazMap;

int currentLine;
int currentAddress;

QStringList specialInstructions = { ".EQU", ".BYTE", ".ORG" };
QStringList allInstructionsM6800 = { "ABA", "ADCA", "ADCB", "ADDA", "ADDB", "ANDA", "ANDB", "ASL", "ASLA", "ASLB", "ASR", "ASRA", "ASRB", "BCC", "BCS", "BEQ", "BGE", "BGT", "BHI", "BITA", "BITB", "BLE", "BLS", "BLT", "BMI", "BNE", "BPL", "BRA", "BSR", "BVC", "BVS", "CBA", "CLC", "CLI", "CLR", "CLRA", "CLRB", "CLV", "CMPA", "CMPB", "COM", "COMA", "COMB", "CPX", "DAA", "DEC", "DECA", "DECB", "DES", "DEX", "EORA", "EORB", "INC", "INCA", "INCB", "INS", "INX", "JMP", "JSR", "LDAA", "LDAB", "LDS", "LDX", "LSR", "LSRA", "LSRB", "NEG", "NEGA", "NEGB", "NOP", "ORAA", "ORAB", "PSHA", "PSHB", "PULA", "PULB", "ROL", "ROLA", "ROLB", "ROR", "RORA", "RORB", "RTI", "RTS", "SBA", "SBCA", "SBCB", "SEC", "SEI", "SEV", "STAA", "STAB", "STS", "STX", "SUBA", "SUBB", "SWI", "TAB", "TAP", "TBA", "TPA", "TST", "TSTA", "TSTB", "TSX", "TXS", "WAI" };
QStringList vseInstructionsM6800 = { "ABA", "ASLA", "ASLB", "ASRA", "ASRB", "CBA", "CLC", "CLI", "CLRA", "CLRB", "CLV", "COMA", "COMB", "DAA", "DECA", "DECB", "DES", "DEX", "INCA", "INCB", "INS", "INX", "LSRA", "LSRB", "NEGA", "NEGB", "NOP", "PSHA", "PSHB", "PULA", "PULB", "ROLA", "ROLB", "RORA", "RORB", "RTI", "RTS", "SBA", "SEC", "SEI", "SEV", "SWI", "TAB", "TAP", "TBA", "TPA", "TSTA", "TSTB", "TSX", "TXS", "WAI" };
QStringList takInstructionsM6800 = { "ADCA", "ADCB", "ADDA", "ADDB", "ANDA", "ANDB", "BITA", "BITB", "CMPA", "CMPB", "EORA", "EORB", "LDAA", "LDAB", "ORAA", "ORAB", "SBCA", "SBCB", "SUBA", "SUBB" };
QStringList takRazInstructionsM6800 = { "CPX", "LDS", "LDX" };
QStringList dirInstructionsM6800 = { "ADCA", "ADCB", "ADDA", "ADDB", "ANDA", "ANDB", "BITA", "BITB", "CMPA", "CMPB", "CPX", "EORA", "EORB", "LDAA", "LDAB", "LDS", "LDX", "ORAA", "ORAB", "SBCA", "SBCB", "STAA", "STAB", "STS", "STX", "SUBA", "SUBB" };
QStringList razInstructionsM6800 = { "ADCA", "ADCB", "ADDA", "ADDB", "ANDA", "ANDB", "ASL", "ASR", "BITA", "BITB", "CLR", "CMPA", "CMPB", "COM", "CPX", "DEC", "EORA", "EORB", "INC", "JMP", "JSR", "LDAA", "LDAB", "LDS", "LDX", "LSR", "NEG", "ORAA", "ORAB", "ROL", "ROR", "SBCA", "SBCB", "STAA", "STAB", "STS", "STX", "SUBA", "SUBB", "TST" };
QStringList indInstructionsM6800 = { "ADCA", "ADCB", "ADDA", "ADDB", "ANDA", "ANDB", "ASL", "ASR", "BITA", "BITB", "CLR", "CMPA", "CMPB", "COM", "CPX", "DEC", "EORA", "EORB", "INC", "JMP", "JSR", "LDAA", "LDAB", "LDS", "LDX", "LSR", "NEG", "ORAA", "ORAB", "ROL", "ROR", "SBCA", "SBCB", "STAA", "STAB", "STS", "STX", "SUBA", "SUBB", "TST" };
QStringList relInstructionsM6800 = { "BCC", "BCS", "BEQ", "BGE", "BGT", "BHI", "BLE", "BLS", "BLT", "BMI", "BNE", "BPL", "BRA", "BSR", "BVC", "BVS" };


QStringList allInstructionsM6803 = { "ABA", "ABX", "ADCA", "ADCB", "ADDA", "ADDB", "ADDD", "ANDA", "ANDB", "ASL", "ASLA", "ASLB", "ASLD", "ASR", "ASRA", "ASRB", "BCC", "BCS", "BEQ", "BGE", "BGT", "BHI", "BHS", "BITA", "BITB", "BLE", "BLO", "BLS", "BLT", "BMI", "BNE", "BPL", "BRA", "BRN", "BSR", "BVC", "BVS", "CBA", "CLC", "CLI", "CLR", "CLRA", "CLRB", "CLV", "CMPA", "CMPB", "COM", "COMA", "COMB", "CPX", "DAA", "DEC", "DECA", "DECB", "DES", "DEX", "EORA", "EORB", "INC", "INCA", "INCB", "INS", "INX", "JMP", "JSR", "LDAA", "LDAB", "LDD", "LDS", "LDX", "LSL", "LSLA", "LSLB", "LSLD", "LSR", "LSRA", "LSRB", "LSRD", "MUL", "NEG", "NEGA", "NEGB", "NOP", "ORAA", "ORAB", "PSHA", "PSHB", "PSHX", "PULA", "PULB", "PULX", "ROL", "ROLA", "ROLB", "ROR", "RORA", "RORB", "RTI", "RTS", "SBA", "SBCA", "SBCB", "SEC", "SEI", "SEV", "STAA", "STAB", "STD", "STS", "STX", "SUBA", "SUBB", "SUBD", "SWI", "TAB", "TAP", "TBA", "TPA", "TST", "TSTA", "TSTB", "TSX", "TXS", "WAI" };
QStringList vseInstructionsM6803 = { "ABA", "ABX", "ASLA", "ASLB", "ASLD", "ASRA", "ASRB", "CBA", "CLC", "CLI", "CLRA", "CLRB", "CLV", "COMA", "COMB", "DAA", "DECA", "DECB", "DES", "DEX", "INCA", "INCB", "INS", "INX", "LSLA", "LSLB", "LSLD", "LSRA", "LSRB", "LSRD", "MUL", "NEGA", "NEGB", "NOP", "PSHA", "PSHB", "PSHX", "PULA", "PULB", "PULX", "ROLA", "ROLB", "RORA", "RORB", "RTI", "RTS", "SBA", "SEC", "SEI", "SEV", "SWI", "TAB", "TAP", "TBA", "TPA", "TSTA", "TSTB", "TSX", "TXS", "WAI" };
QStringList takInstructionsM6803 = { "ADCA", "ADCB", "ADDA", "ADDB",  "ANDA", "ANDB", "BITA", "BITB", "CMPA", "CMPB", "EORA", "EORB", "LDAA", "LDAB", "ORAA", "ORAB", "SBCA", "SBCB", "SUBA", "SUBB" };
QStringList takRazInstructionsM6803 = { "ADDD", "CPX", "LDD", "LDS", "LDX", "SUBD" };
QStringList dirInstructionsM6803 = { "ADCA", "ADCB", "ADDA", "ADDB", "ADDD", "ANDA", "ANDB", "BITA", "BITB", "CMPA", "CMPB", "CPX", "EORA", "EORB", "JSR", "LDAA", "LDAB", "LDD", "LDS", "LDX", "ORAA", "ORAB", "SBCA", "SBCB", "STAA", "STAB", "STD", "STS", "STX", "SUBA", "SUBB", "SUBD" };
QStringList razInstructionsM6803 = { "ADCA", "ADCB", "ADDA", "ADDB", "ADDD", "ANDA", "ANDB", "ASL", "ASR", "BITA", "BITB", "CLR", "CMPA", "CMPB", "COM", "CPX", "DEC", "EORA", "EORB", "INC", "JMP", "JSR", "LDAA", "LDAB", "LDD", "LDS", "LDX", "LSL", "LSR", "NEG", "ORAA", "ORAB", "ROL", "ROR", "SBCA", "SBCB", "STAA", "STAB", "STD", "STS", "STX", "SUBA", "SUBB", "SUBD", "TST" };
QStringList indInstructionsM6803 = { "ADCA", "ADCB", "ADDA", "ADDB", "ADDD", "ANDA", "ANDB", "ASL", "ASR", "BITA", "BITB", "CLR", "CMPA", "CMPB", "COM", "CPX", "DEC", "EORA", "EORB", "INC", "JMP", "JSR", "LDAA", "LDAB", "LDD", "LDS", "LDX", "LSL", "LSR", "NEG", "ORAA", "ORAB", "ROL", "ROR", "SBCA", "SBCB", "STAA", "STAB", "STD", "STS", "STX", "SUBA", "SUBB", "SUBD", "TST" };
QStringList relInstructionsM6803 = { "BCC", "BCS", "BEQ", "BGE", "BGT", "BHI", "BHS", "BLE", "BLO", "BLS", "BLT", "BMI", "BNE", "BPL", "BRA", "BRN", "BSR", "BVC", "BVS" };

bool compiled = false;
bool compileOnRun = true;
bool displayActive = false;
int lastInput = -1;
int interruptLocations = 0xFFFF;
int currentSMScroll = 0;

QString convertToQString(int number, int width)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(width) << number;
    std::string str = stream.str();
    return QString::fromStdString(str);
}
int getNum(const QString& input)
{
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
inline bool bit(int variable, int bitNum){
    return (variable & (1 << bitNum)) != 0;
}

void MainWindow::updateMemoryTab()
{
    if(simpleMemory){
        for (int i = 0; i < 20; ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString("%1").arg(currentSMScroll + i, 4, 16, QChar('0')).toUpper());
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidgetSM->setItem(i,0,item);
            item = new QTableWidgetItem(QString("%1").arg(static_cast<quint8>(Memory[currentSMScroll + i]), 2, 16, QChar('0').toUpper()));
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidgetSM->setItem(i,1,item);
        }
    }else{
        const int memorySize = 0x10000;
        const int lineSize = 16;
        QString text;
        text.reserve((memorySize / lineSize) * (lineSize * 3 + 12));
        int scrollPosition = ui->plainTextMemory->verticalScrollBar()->value();
        text += "      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n";
        for (int i = 0; i < memorySize; i++) {
            if (i % lineSize == 0) {
                text += QString("%1: ").arg(i, 4, 16, QChar('0'));
            }
            text += QString("%1 ").arg(static_cast<quint8>(Memory[i]), 2, 16, QChar('0'));
            if (i % lineSize == lineSize - 1) {
                text += '\n';
            }
        }
        ui->plainTextMemory->setPlainText(text);
        ui->plainTextMemory->verticalScrollBar()->setValue(scrollPosition);
    }
}
void MainWindow::updateMemoryCell(int address) {
    if(simpleMemory){
        for (int i = 0; i < 20; ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString("%1").arg(currentSMScroll + i, 4, 16, QChar('0')).toUpper());
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidgetSM->setItem(i,0,item);
            item = new QTableWidgetItem(QString("%1").arg(static_cast<quint8>(Memory[currentSMScroll + i]), 2, 16, QChar('0').toUpper()));
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidgetSM->setItem(i,1,item);
        }
    }else{
    int line = std::floor(address / 16)+1;
    int position = (address % 16) * 3 + 4;
    QTextCursor cursor(ui->plainTextMemory->document());
    cursor.setPosition(line * 55 + position);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
    cursor.removeSelectedText();
    cursor.insertText(QString("%1").arg(Memory[address], 2, 16, QChar('0')));
    if(address >= 0xFB00 && address <= 0xFF37){
        int relativeAddress = address-0xFB00;
        int line = std::floor(relativeAddress / 54);
        int position = (relativeAddress % 54);
        QTextCursor cursor(ui->plainTextDisplay->document());
        cursor.setPosition(line * 55 + position);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
        cursor.removeSelectedText();
        ushort charValue = Memory[address];
        if (charValue < 32 || charValue == 127) {
            cursor.removeSelectedText();
            cursor.insertText(" "); // Replace with a space
        } else {
            cursor.removeSelectedText();
            cursor.insertText(QChar(static_cast<ushort>(charValue)));
        }
    }
    }
}
void MainWindow::updateLinesBox()
{
    if (ui->checkBoxAdvancedInfo->isChecked()) {
        if (instructionList.empty()) {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i < code.count("\n") + 1; i++) {
                text = text % convertToQString(i, 5) % "\n";
            }
            ui->plainTextLines->setPlainText(text);
        }
        else {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i < code.count("\n") + 1; i++) {
                if (instructionList[i].address == -1) {
                    text = text % convertToQString(i, 5) % ":----\n";
                }
                else {
                    text = text % convertToQString(i, 5)
                           % ":" % QString("%1").arg(instructionList[i].address, 4, 16, QChar('0'))
                           % ":" % QString("%1").arg(instructionList[i].byte1, 2, 16, QChar('0'))
                           % ":" % QString("%1").arg(instructionList[i].byte2, 2, 16, QChar('0'))
                           % ":" % QString("%1").arg(instructionList[i].byte3, 2, 16, QChar('0'))
                           % "\n";
                }
            }
            ui->plainTextLines->setPlainText(text);
        }
    }
    else {
        if (instructionList.empty()) {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i < code.count("\n") + 1; i++) {
                text = text % convertToQString(i, 5) % "\n";
            }
            ui->plainTextLines->setPlainText(text);
        }
        else {
            QString code = ui->plainTextCode->toPlainText();
            QString text;

            for (int i = 0; i < code.count("\n") + 1; i++) {

                if (instructionList[i].address == -1) {
                    text = text % convertToQString(i, 5) % ":----\n";
                }
                else {
                    text = text % convertToQString(i, 5) % ":" % QString("%1").arg(instructionList[i].address, 4, 16, QChar('0')) % "\n";
                }
            }
            ui->plainTextLines->setPlainText(text);
        }
    }
}
void MainWindow::updateFlags(FlagToUpdate flag, bool value){
    switch (flag) {
    case HalfCarry:
        flags = (flags & ~(1 << 5)) | (value << 5);
        ui->lineEditHValue->setText(QString::number(value));
        break;
    case InterruptMask:
        flags = (flags & ~(1 << 4)) | (value << 4);
        ui->lineEditIValue->setText(QString::number(value));
        break;
    case Negative:
        flags = (flags & ~(1 << 3)) | (value << 3);
        ui->lineEditNValue->setText(QString::number(value));
        break;
    case Zero:
        flags = (flags & ~(1 << 2)) | (value << 2);
        ui->lineEditZValue->setText(QString::number(value));
        break;
    case Overflow:
        flags = (flags & ~(1 << 1)) | (value << 1);
        ui->lineEditVValue->setText(QString::number(value));
        break;
    case Carry:
        flags = (flags & ~(1)) | (value);
        ui->lineEditCValue->setText(QString::number(value));
        break;
    default:
        throw;
    }
}
void MainWindow::updateElement(elementToUpdate element){
    switch (element) {
    case regPC:
        if(hexReg){
            ui->lineEditPCValue->setText(QString("%1").arg(PC, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            ui->lineEditPCValue->setText(QString::number(PC));
        }
        break;
    case regSP:
        if(hexReg){
            ui->lineEditSPValue->setText(QString("%1").arg(SP, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            ui->lineEditSPValue->setText(QString::number(SP));
        }
        break;
    case regA:
        if(hexReg){
            ui->lineEditAValue->setText(QString("%1").arg(aReg, 2, 16, QLatin1Char('0')).toUpper());
        }else{
            ui->lineEditAValue->setText(QString::number(aReg));
        }
        break;
    case regB:
        if(hexReg){
            ui->lineEditBValue->setText(QString("%1").arg(bReg, 2, 16, QLatin1Char('0')).toUpper());
        }else{
            ui->lineEditBValue->setText(QString::number(bReg));
        }
        break;
    case regX:
        if(hexReg){
            ui->lineEditXValue->setText(QString("%1").arg(xRegister, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            ui->lineEditXValue->setText(QString::number(xRegister));
        }
        break;
    case allFlags:
        ui->lineEditHValue->setText(QString::number(bit(flags,5)));
        ui->lineEditIValue->setText(QString::number(bit(flags,4)));
        ui->lineEditNValue->setText(QString::number(bit(flags,3)));
        ui->lineEditZValue->setText(QString::number(bit(flags,2)));
        ui->lineEditVValue->setText(QString::number(bit(flags,1)));
        ui->lineEditCValue->setText(QString::number(bit(flags,0)));
        break;
    default:
        throw;
    }
}

void MainWindow::PrintConsole(const QString& text, int type)
{
    QString consoleText;
    if (type == -1) { // DEBUG
        consoleText = "DEBUG: " + ("Ln:" + QString::number(currentLine)) + " " + text;
    }
    else if (type == 0 && ui->checkBoxError->isChecked()) { // ERROR
        consoleText = "ERROR: " + text;
    }
    else if (type == 1 && ui->checkBoxWarn->isChecked()) { //WARN
        consoleText = "WARN: " + text;
    }
    else {
        consoleText = text;
    }
    ui->plainTextConsole->appendPlainText(consoleText);
}
void MainWindow::Err(const QString& text)
{
    PrintConsole("Ln:" + QString::number(currentLine) + " " + text, 0);
}

QList<QTextEdit::ExtraSelection> linesSelectionsRunTime;
QList<QTextEdit::ExtraSelection> codeSelectionsRunTime;
QList<QTextEdit::ExtraSelection> memorySelectionsRunTime;
QList<QTextEdit::ExtraSelection> linesSelectionsLines;
QList<QTextEdit::ExtraSelection> codeSelectionsLines;
QList<QTextEdit::ExtraSelection> memorySelectionsLines;
int previousScrollCode = 0;
int previousScrollMemory = 0;
int autoScrollUpLimit = 20;
int autoScrollDownLimit = 5;
void MainWindow::updateSelectionsRunTime(int address){

    linesSelectionsRunTime.clear();
    codeSelectionsRunTime.clear();
    memorySelectionsRunTime.clear();
    int lineNum = getLineByAddress(address) - 1;
    QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(lineNum);
    QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(lineNum);
    QTextCursor codeCursor(codeBlock);
    codeCursor.select(QTextCursor::LineUnderCursor);
    QTextCursor linesCursor(linesBlock);
    linesCursor.select(QTextCursor::LineUnderCursor);

    int line = std::floor(address / 16)+1;
    int position = (address % 16) * 3 + 4;
    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
    QTextCursor memoryCursor(memoryBlock);
    memoryCursor.setPosition(line*55 + position);
    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);

    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::yellow);

    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    lineSelection.cursor = linesCursor;
    linesSelectionsRunTime.append(lineSelection);

    lineSelection.cursor = codeCursor;
    codeSelectionsRunTime.append(lineSelection);

    lineSelection.cursor = memoryCursor;
    memorySelectionsRunTime.append(lineSelection);
    QList<QTextEdit::ExtraSelection> combinedLinesSelections;
    QList<QTextEdit::ExtraSelection> combinedCodeSelections;
    QList<QTextEdit::ExtraSelection> combinedMemorySelections;
    combinedLinesSelections.append(linesSelectionsRunTime);
    combinedLinesSelections.append(linesSelectionsLines);
    combinedCodeSelections.append(codeSelectionsRunTime);
    combinedCodeSelections.append(codeSelectionsLines);
    combinedMemorySelections.append(memorySelectionsRunTime);
    combinedMemorySelections.append(memorySelectionsLines);

    ui->plainTextLines->setExtraSelections(combinedLinesSelections);
    ui->plainTextCode->setExtraSelections(combinedCodeSelections);
    ui->plainTextMemory->setExtraSelections(combinedMemorySelections);

    if (lineNum > previousScrollCode + autoScrollUpLimit){
        previousScrollCode = lineNum - autoScrollUpLimit;
        ui->plainTextLines->verticalScrollBar()->setValue(previousScrollCode);
        ui->plainTextCode->verticalScrollBar()->setValue(previousScrollCode);
    } else if (lineNum < previousScrollCode + autoScrollDownLimit){
        previousScrollCode = lineNum - autoScrollDownLimit;
        ui->plainTextLines->verticalScrollBar()->setValue(previousScrollCode);
        ui->plainTextCode->verticalScrollBar()->setValue(previousScrollCode);
    }

}
void MainWindow::updateSelectionsLines(int line){

    linesSelectionsLines.clear();
    codeSelectionsLines.clear();
    memorySelectionsLines.clear();
    QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(line);
    QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(line);
    QTextCursor codeCursor(codeBlock);
    codeCursor.select(QTextCursor::LineUnderCursor);
    QTextCursor linesCursor(linesBlock);
    linesCursor.select(QTextCursor::LineUnderCursor);

    int lineM = std::floor(getAddressByLine(line+1) / 16)+1;
    int position = (getAddressByLine(line+1) % 16) * 3 + 4;
    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
    QTextCursor memoryCursor(memoryBlock);
    memoryCursor.setPosition(lineM*55 + position);
    memoryCursor.setPosition(lineM*55 + position+2,QTextCursor::KeepAnchor);

    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::green);

    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    lineSelection.cursor = linesCursor;
    linesSelectionsLines.append(lineSelection);

    lineSelection.cursor = codeCursor;
    codeSelectionsLines.append(lineSelection);

    lineSelection.cursor = memoryCursor;
    memorySelectionsLines.append(lineSelection);
    QList<QTextEdit::ExtraSelection> combinedLinesSelections;
    QList<QTextEdit::ExtraSelection> combinedCodeSelections;
    QList<QTextEdit::ExtraSelection> combinedMemorySelections;
    combinedLinesSelections.append(linesSelectionsRunTime);
    combinedLinesSelections.append(linesSelectionsLines);
    combinedCodeSelections.append(codeSelectionsRunTime);
    combinedCodeSelections.append(codeSelectionsLines);
    combinedMemorySelections.append(memorySelectionsRunTime);
    combinedMemorySelections.append(memorySelectionsLines);

    ui->plainTextLines->setExtraSelections(combinedLinesSelections);
    ui->plainTextCode->setExtraSelections(combinedCodeSelections);
    ui->plainTextMemory->setExtraSelections(combinedMemorySelections);

}

void MainWindow::resetEmulator(){
    if (running){
        running = false;
        executionTimer->stop();
    }
    aReg = 0;
    bReg = 0;
    xRegister = 0;
    PC = 0;
    SP = 0xF000;
    flags = 0xC0;
    lastInput = -1;
    std::memcpy(Memory, backupMemory, sizeof(backupMemory));
    updateMemoryTab();
    updateElement(regPC);
    updateElement(regSP);
    updateElement(regA);
    updateElement(regB);
    updateElement(regX);
    ui->lineEditHValue->setText(QString::number(flags & 0x30));
    ui->lineEditIValue->setText(QString::number(flags & 0x20));
    ui->lineEditNValue->setText(QString::number(flags & 0x10));
    ui->lineEditZValue->setText(QString::number(flags & 0x04));
    ui->lineEditVValue->setText(QString::number(flags & 0x02));
    ui->lineEditCValue->setText(QString::number(flags & 0x01));
    QList<QTextEdit::ExtraSelection> emptySelections;
    ui->plainTextLines->setExtraSelections(emptySelections);
    ui->plainTextCode->setExtraSelections(emptySelections);
    ui->plainTextMemory->setExtraSelections(emptySelections);
    linesSelectionsRunTime.clear();
    codeSelectionsRunTime.clear();
    memorySelectionsRunTime.clear();
    linesSelectionsLines.clear();
    codeSelectionsLines.clear();
    memorySelectionsLines.clear();
    QList<QTextEdit::ExtraSelection> memorySelections;
    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(1);
    QTextCursor memoryCursor(memoryBlock);
    QTextEdit::ExtraSelection lineSelection;
    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::yellow);
    lineSelection.format = lineFormat;
    memoryCursor.setPosition(59);
    memoryCursor.setPosition(61,QTextCursor::KeepAnchor);
    lineSelection.cursor = memoryCursor;
    memorySelections.append(lineSelection);
    ui->plainTextMemory->setExtraSelections(memorySelections);
    ui->plainTextDisplay->setPlainText("                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                       ,");
}

void MainWindow::handleVerticalScrollBarValueChanged(int value)
{
    ui->plainTextLines->verticalScrollBar()->setValue(value);
}
void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    emit resized(this->size());
}
void MainWindow::handleMainWindowSizeChanged(const QSize& newSize)
{
    int buttonYoffset = 30;
    int buttonY = newSize.height() - buttonYoffset;

    if (newSize.width() >= 1785) {
        displayActive = true;
        ui->plainTextDisplay->setGeometry(0, 0, 498, 350);
        ui->plainTextDisplay->setEnabled(true);
        ui->frameDisplay->setGeometry(910, 10, 498, 350);
        ui->frameDisplay->setEnabled(true);
        if (newSize.height() >= 800) {
            ui->tabWidget->setGeometry(910, 370, newSize.width() - 915, newSize.height() - 359 - buttonYoffset - 17);
        }
        else {
            ui->tabWidget->setGeometry(newSize.width() - 370, 300, 361, newSize.height() - 289 - buttonYoffset - 17);
        }
    }
    else {
        displayActive = true;
        ui->plainTextDisplay->setGeometry(0, 0, 0, 0);
        ui->plainTextDisplay->setEnabled(false);
        ui->frameDisplay->setGeometry(910, 10, 0, 0);
        ui->frameDisplay->setEnabled(false);

        ui->tabWidget->setGeometry(910, 300, newSize.width() - 917, newSize.height() - 289 - buttonYoffset - 17);
    }

    ui->buttonCompile->setGeometry(ui->buttonCompile->x(), buttonY, ui->buttonCompile->width(), ui->buttonCompile->height());
    ui->buttonFSDisplay->setGeometry(newSize.width() - 170, buttonY, ui->buttonFSDisplay->width(), ui->buttonFSDisplay->height());
    ui->comboBoxVersionSelector->setGeometry(ui->comboBoxVersionSelector->x(), buttonY, ui->comboBoxVersionSelector->width(), ui->comboBoxVersionSelector->height());
    ui->buttonLoad->setGeometry(ui->buttonLoad->x(), buttonY, ui->buttonLoad->width(), ui->buttonLoad->height());
    ui->buttonSave->setGeometry(ui->buttonSave->x(), buttonY, ui->buttonSave->width(), ui->buttonSave->height());
    ui->buttonReset->setGeometry(ui->buttonReset->x(), buttonY, ui->buttonReset->width(), ui->buttonReset->height());
    ui->buttonStep->setGeometry(ui->buttonStep->x(), buttonY, ui->buttonStep->width(), ui->buttonStep->height());
    ui->buttonRunStop->setGeometry(ui->buttonRunStop->x(), buttonY, ui->buttonRunStop->width(), ui->buttonRunStop->height());
    ui->comboBoxSpeedSelector->setGeometry(ui->comboBoxSpeedSelector->x(), buttonY, ui->comboBoxSpeedSelector->width(), ui->comboBoxSpeedSelector->height());
    ui->buttonSwitchWrite->setGeometry(ui->buttonSwitchWrite->x(), buttonY, ui->buttonSwitchWrite->width(), ui->buttonSwitchWrite->height());
    ui->labelWritingMode->setGeometry(ui->labelWritingMode->x(), buttonY, ui->labelWritingMode->width(), ui->labelWritingMode->height());

    ui->plainTextCode->setGeometry(ui->plainTextCode->x(), ui->plainTextCode->y(), ui->plainTextCode->width(), newSize.height() - buttonYoffset - 17);
    ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), ui->plainTextLines->width(), newSize.height() - buttonYoffset - 17);
    ui->plainTextMemory->setGeometry(ui->plainTextMemory->x(), ui->plainTextMemory->y(), ui->plainTextMemory->width(), newSize.height() - buttonYoffset - 17);

    ui->groupBox->setGeometry(newSize.width() - 370, ui->groupBox->y(), ui->groupBox->width(), 281);

    ui->plainTextConsole->setGeometry(5, 5, ui->tabWidget->width() - 15, ui->tabWidget->height() - 35);
    ui->plainTextInfo->setGeometry(5, 5, ui->tabWidget->width() - 15, ui->tabWidget->height() - 35);
    //ui->tableWidget->setGeometry(5, 5, ui->tabWidget->width() - 15, ui->tabWidget->height() - 35);
}

void MainWindow::on_plainTextCode_textChanged()
{

    if (ui->plainTextCode->toPlainText().count('\n') > 65535) {
        QString text = ui->plainTextCode->toPlainText();
        QStringList lines = text.split('\n', Qt::SkipEmptyParts);
        lines = lines.mid(0, 0x10000);
        text = lines.join('\n');
        ui->plainTextCode->setPlainText(text);
    }
    compiled = 0;
    clearInstructions();
    ui->plainTextCode->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    ui->plainTextLines->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    updateLinesBox();
    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
    ui->buttonCompile->setStyleSheet("");
    ui->buttonCompile->setStyleSheet(uncompiledButton);
}
void MainWindow::on_lineEditBin_textChanged(const QString &arg1)
{
    if(ui->lineEditBin->text() != "X"){
        bool ok;
        int number = arg1.toInt(&ok, 2);

        if (ok) {
            ui->lineEditDec->setText(QString::number(number));
            ui->lineEditOct->setText(QString::number(number, 8));
            ui->lineEditHex->setText(QString::number(number, 16));
        } else {
            ui->lineEditDec->setText("X");
            ui->lineEditOct->setText("X");
            ui->lineEditHex->setText("X");
        }
    }
}
void MainWindow::on_lineEditOct_textChanged(const QString &arg1)
{
    if(ui->lineEditOct->text() != "X"){
        bool ok;
        int number = arg1.toInt(&ok, 8);

        if (ok) {
            ui->lineEditDec->setText(QString::number(number));
            ui->lineEditBin->setText(QString::number(number, 2));
            ui->lineEditHex->setText(QString::number(number, 16));
        } else {
            ui->lineEditDec->setText("X");
            ui->lineEditBin->setText("X");
            ui->lineEditHex->setText("X");
        }
    }

}
void MainWindow::on_lineEditHex_textChanged(const QString &arg1)
{
    if(ui->lineEditHex->text() != "X"){
        bool ok;
        int number = arg1.toInt(&ok, 16);
        if (ok) {
            ui->lineEditDec->setText(QString::number(number));
            ui->lineEditBin->setText(QString::number(number, 2));
            ui->lineEditOct->setText(QString::number(number, 8));
        } else {
            ui->lineEditDec->setText("X");
            ui->lineEditBin->setText("X");
            ui->lineEditOct->setText("X");
        }
    }

}
void MainWindow::on_lineEditDec_textChanged(const QString &arg1)
{
    if(ui->lineEditDec->text() != "X"){
        bool ok;
        int number = arg1.toInt(&ok);
        if(ok){
            ui->lineEditBin->setText(QString::number(number, 2));
            ui->lineEditOct->setText(QString::number(number, 8));
            ui->lineEditHex->setText(QString::number(number, 16));

        }else{
            ui->lineEditBin->setText("X");
            ui->lineEditOct->setText("X");
            ui->lineEditHex->setText("X");
        }
    }

}

void MainWindow::on_plainTextLines_cursorPositionChanged()
{
    if(compiled){
        int lineNumber = ui->plainTextLines->textCursor().blockNumber();
        updateSelectionsLines(lineNumber);
    }
}
void MainWindow::handleLinesScroll(){
    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
}
void MainWindow::handleDisplayScrollVertical(){
    ui->plainTextDisplay->verticalScrollBar()->setValue(0);
}
void MainWindow::handleDisplayScrollHorizontal(){
    ui->plainTextDisplay->horizontalScrollBar()->setValue(0);
}
void MainWindow::handleMemoryScrollHorizontal(){
    ui->plainTextMemory->horizontalScrollBar()->setValue(0);
}
int currentAddressSelection = 0;
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->plainTextDisplay) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() <= Qt::Key_AsciiTilde) {
                char asciiChar = static_cast<char>(keyEvent->key());
                uint8_t asciiValue = static_cast<uint8_t>(asciiChar);
                Memory[0xFFF0] = asciiValue;
                updateMemoryCell(0xFFF0);
            }
            return true;
        } else if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

            if (mouseEvent->button() == Qt::LeftButton) {
                Memory[0xFFF1] = 1;
                updateMemoryCell(0xFFF1);
            } else if (mouseEvent->button() == Qt::RightButton) {
                Memory[0xFFF1] = 2;
                updateMemoryCell(0xFFF1);
            } else if (mouseEvent->button() == Qt::MiddleButton) {
                Memory[0xFFF1] = 3;
                updateMemoryCell(0xFFF1);
            }
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                Memory[0xFFF1] = 4;
                updateMemoryCell(0xFFF1);
            } else if (mouseEvent->button() == Qt::RightButton) {
                Memory[0xFFF1] = 5;
                updateMemoryCell(0xFFF1);
            } else if (mouseEvent->button() == Qt::MiddleButton) {
                Memory[0xFFF1] = 6;
                updateMemoryCell(0xFFF1);
            }
            return true;
        }else if(event->type() == QEvent::FocusIn){
            ui->plainTextDisplay->setStyleSheet("QPlainTextEdit:focus { border: 2px solid blue; }");
        } else if(event->type() == QEvent::FocusOut){
            ui->plainTextDisplay->setStyleSheet("");
        }
    }/*/
    if (obj == ui->plainTextLines){
        if(event->type() == QEvent::Wheel){
            qDebug() << "hi";
            return true;
        }else{

        }
    }/*/
    if(writeToMemory){
        if (obj == ui->plainTextMemory){
            if(event->type() == QEvent::KeyPress){
                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

                int keyValue = keyEvent->key();
                if ((keyValue >= Qt::Key_0 && keyValue <= Qt::Key_9) ||
                    (keyValue >= Qt::Key_A && keyValue <= Qt::Key_F)) {
                    char newByte = keyEvent->text().toUpper().toLatin1()[0];
                    uint8_t currentCellValue = Memory[currentAddressSelection];
                    Memory[currentAddressSelection] = (currentCellValue << 4) | QString(newByte).toInt(nullptr, 16);;
                    updateMemoryCell(currentAddressSelection);
                    QTextCharFormat lineFormat;
                    lineFormat.setBackground(Qt::yellow);
                    QTextEdit::ExtraSelection lineSelection;
                    QList<QTextEdit::ExtraSelection> curMemorySelections;
                    lineSelection.format = lineFormat;
                    int line = std::floor(currentAddressSelection / 16)+1;
                    int position = (currentAddressSelection % 16) * 3 + 4;
                    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
                    QTextCursor memoryCursor(memoryBlock);
                    memoryCursor.setPosition(line*55 + position);
                    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);
                    lineSelection.cursor = memoryCursor;
                    curMemorySelections.append(lineSelection);
                    ui->plainTextMemory->setExtraSelections(curMemorySelections);
                    std::memcpy(backupMemory, Memory, sizeof(Memory));
                    compiled = 0;
                    clearInstructions();
                    updateLinesBox();
                    linesSelectionsRunTime.clear();
                    codeSelectionsRunTime.clear();
                    memorySelectionsRunTime.clear();
                    linesSelectionsLines.clear();
                    codeSelectionsLines.clear();
                    memorySelectionsLines.clear();
                    ui->buttonCompile->setStyleSheet("");
                    ui->buttonCompile->setStyleSheet(uncompiledButton);
                }



                if (keyEvent->key() == Qt::Key_Up ){
                    if(currentAddressSelection - 16 >= 0){
                        currentAddressSelection-=16;
                    }
                    QTextCharFormat lineFormat;
                    lineFormat.setBackground(Qt::green);
                    QTextEdit::ExtraSelection lineSelection;
                    QList<QTextEdit::ExtraSelection> curMemorySelections;
                    lineSelection.format = lineFormat;
                    int line = std::floor(currentAddressSelection / 16)+1;
                    int position = (currentAddressSelection % 16) * 3 + 4;
                    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
                    QTextCursor memoryCursor(memoryBlock);
                    memoryCursor.setPosition(line*55 + position);
                    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);
                    lineSelection.cursor = memoryCursor;
                    curMemorySelections.append(lineSelection);
                    ui->plainTextMemory->setExtraSelections(curMemorySelections);
                }else if (keyEvent->key() == Qt::Key_Down){
                    if(currentAddressSelection + 16 <= 0xFFFF){
                        currentAddressSelection+=16;
                    }
                    QTextCharFormat lineFormat;
                    lineFormat.setBackground(Qt::green);
                    QTextEdit::ExtraSelection lineSelection;
                    QList<QTextEdit::ExtraSelection> curMemorySelections;
                    lineSelection.format = lineFormat;
                    int line = std::floor(currentAddressSelection / 16)+1;
                    int position = (currentAddressSelection % 16) * 3 + 4;
                    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
                    QTextCursor memoryCursor(memoryBlock);
                    memoryCursor.setPosition(line*55 + position);
                    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);
                    lineSelection.cursor = memoryCursor;
                    curMemorySelections.append(lineSelection);
                    ui->plainTextMemory->setExtraSelections(curMemorySelections);
                }else if(keyEvent->key() == Qt::Key_Left){
                    if(currentAddressSelection > 0){
                        currentAddressSelection--;
                    }
                    QTextCharFormat lineFormat;
                    lineFormat.setBackground(Qt::green);
                    QTextEdit::ExtraSelection lineSelection;
                    QList<QTextEdit::ExtraSelection> curMemorySelections;
                    lineSelection.format = lineFormat;
                    int line = std::floor(currentAddressSelection / 16)+1;
                    int position = (currentAddressSelection % 16) * 3 + 4;
                    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
                    QTextCursor memoryCursor(memoryBlock);
                    memoryCursor.setPosition(line*55 + position);
                    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);
                    lineSelection.cursor = memoryCursor;
                    curMemorySelections.append(lineSelection);
                    ui->plainTextMemory->setExtraSelections(curMemorySelections);
                }else if (keyEvent->key() == Qt::Key_Right) {
                    if(currentAddressSelection < 0xFFFF){
                        currentAddressSelection++;
                    }
                    QTextCharFormat lineFormat;
                    lineFormat.setBackground(Qt::green);
                    QTextEdit::ExtraSelection lineSelection;
                    QList<QTextEdit::ExtraSelection> curMemorySelections;
                    lineSelection.format = lineFormat;
                    int line = std::floor(currentAddressSelection / 16)+1;
                    int position = (currentAddressSelection % 16) * 3 + 4;
                    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
                    QTextCursor memoryCursor(memoryBlock);
                    memoryCursor.setPosition(line*55 + position);
                    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);
                    lineSelection.cursor = memoryCursor;
                    curMemorySelections.append(lineSelection);
                    ui->plainTextMemory->setExtraSelections(curMemorySelections);
                }
                return true;
            }else{

            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_comboBoxVersionSelector_currentIndexChanged(int index)
{
    currentVersionIndex = index;
    running = false;
    executionTimer->stop();
    compiled = 0;
    clearInstructions();
    ui->plainTextCode->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    ui->plainTextLines->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    updateLinesBox();
    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
    ui->buttonCompile->setStyleSheet("");
    ui->buttonCompile->setStyleSheet(uncompiledButton);
    resetEmulator();
}
bool MainWindow::on_buttonCompile_clicked()
{
    if(currentVersionIndex ==  0){
        return compileM6800();
    } else if(currentVersionIndex ==  1){
        return compileM6803();
    }
    return false;
}
void MainWindow::on_buttonLoad_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            ui->plainTextCode->setPlainText(in.readAll());
            file.close();
        } else {
            PrintConsole("Error loading script",0);
        }
    }
}
void MainWindow::on_buttonSave_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->plainTextCode->toPlainText();
            file.close();
        } else {
            PrintConsole("Error saving script",0);
        }
    }
}
void MainWindow::on_buttonReset_clicked()
{
    resetEmulator();
}
void MainWindow::on_buttonStep_clicked()
{
    bool ok = true;;
    if (!compiled && compileOnRun){ok = on_buttonCompile_clicked();}
    if(ok){
        executeInstruction();
    }
    if (running){
        running = false;
        executionTimer->stop();
    }
}
void MainWindow::on_buttonRunStop_clicked()
{
    bool ok = true;
    if (running){
        running = false;
        executionTimer->stop();
    }else {
        if (!compiled && compileOnRun){ok = on_buttonCompile_clicked();}
        if(ok){
            if(ui->checkBoxAutoReset->isChecked()){
                if (Memory[PC] == 0){
                    resetEmulator();
                }
            }
            running = true;
            executeInstruction();
            if(running == true){
                executionTimer->start(executionSpeed);
            }
        }
    }
}
void MainWindow::on_comboBoxSpeedSelector_activated(int index)
{
    executionSpeed = std::pow(2, index);
    executionSpeed = std::ceil(1000.0 / executionSpeed);
    if (running){
        running = false;
        executionTimer->stop();
    }
}

void MainWindow::on_checkBoxHexRegs_clicked(bool checked)
{
    hexReg = checked;
    updateElement(regPC);
    updateElement(regSP);
    updateElement(regA);
    updateElement(regB);
    updateElement(regX);
}
void MainWindow::on_checkBoxAdvancedInfo_clicked(bool checked)
{
    if (checked) {
        ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), 181, ui->plainTextLines->height());
        ui->plainTextCode->setGeometry(190, ui->plainTextCode->y(), 201, ui->plainTextCode->height());
        updateLinesBox();
    }
    else {
        ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), 101, ui->plainTextLines->height());
        ui->plainTextCode->setGeometry(110, ui->plainTextCode->y(), 281, ui->plainTextCode->height());
        updateLinesBox();
    }
}
void MainWindow::on_checkBoxCompileOnRun_clicked(bool checked)
{
    compileOnRun = checked;
}
void MainWindow::on_spinBoxLow_valueChanged(int arg1)
{
    autoScrollDownLimit = arg1;
}
void MainWindow::on_spinBoxUp_valueChanged(int arg1)
{
    autoScrollUpLimit = arg1;
}
void MainWindow::on_checkBoxWriteMemory_clicked(bool checked)
{
    if(checked){
        ui->buttonSwitchWrite->setVisible(true);
        ui->labelWritingMode->setVisible(true);
        ui->buttonSwitchWrite->setEnabled(true);
        ui->labelWritingMode->setEnabled(true);
    }else{
        ui->buttonSwitchWrite->setVisible(false);
        ui->labelWritingMode->setVisible(false);
        ui->buttonSwitchWrite->setEnabled(false);
        ui->labelWritingMode->setEnabled(false);
        writeToMemory = false;
        ui->plainTextCode->setReadOnly(false);
        ui->buttonCompile->setEnabled(true);
        ui->checkBoxCompileOnRun->setEnabled(true);
        compileOnRun = true;
        ui->labelWritingMode->setText("Code");
    }
}
void MainWindow::on_buttonSwitchWrite_clicked()
{
    if(writeToMemory){
        writeToMemory = false;
        ui->plainTextCode->setReadOnly(false);
        ui->buttonCompile->setEnabled(true);
        ui->checkBoxCompileOnRun->setEnabled(true);
        compileOnRun = true;
        ui->labelWritingMode->setText("Code");

    }else{
        writeToMemory = true;
        ui->plainTextCode->setReadOnly(true);
        ui->buttonCompile->setEnabled(false);
        ui->checkBoxCompileOnRun->setEnabled(false);
        compileOnRun = false;
        ui->labelWritingMode->setText("Memory");

    }
}
void MainWindow::on_comboBoxBreakWhen_currentIndexChanged(int index)
{
    ui->spinBoxBreakAt->setValue(0);
    ui->spinBoxBreakIs->setValue(0);
    if(index == 0){
        breakEnabled = false;
    }else{
        breakEnabled = true;
    }
    if(index == 13){
        ui->labelAt->setVisible(true);
        ui->spinBoxBreakAt->setVisible(true);
    }else{
        ui->labelAt->setVisible(false);
        ui->spinBoxBreakAt->setVisible(false);
    }
    switch(ui->comboBoxBreakWhen->currentIndex()){
    case 1:
        ui->spinBoxBreakIs->setMaximum(65535);
        break;
    case 2:
        ui->spinBoxBreakIs->setMaximum(0xFFFF);
        break;
    case 3:
        ui->spinBoxBreakIs->setMaximum(0xFFFF);
        break;
    case 4:
        ui->spinBoxBreakIs->setMaximum(0xFFFF);
        break;
    case 5:
        ui->spinBoxBreakIs->setMaximum(0xFF);
        break;
    case 6:
        ui->spinBoxBreakIs->setMaximum(0xFF);
        break;
    case 7:
        ui->spinBoxBreakIs->setMaximum(1);
        break;
    case 8:
        ui->spinBoxBreakIs->setMaximum(1);
        break;
    case 9:
        ui->spinBoxBreakIs->setMaximum(1);
        break;
    case 10:
        ui->spinBoxBreakIs->setMaximum(1);
        break;
    case 11:
        ui->spinBoxBreakIs->setMaximum(1);
        break;
    case 12:
        ui->spinBoxBreakIs->setMaximum(1);
        break;
    case 13:
        ui->spinBoxBreakIs->setMaximum(0xFF);
        break;


    }
}
void MainWindow::on_checkBoxSimpleMemory_clicked(bool checked)
{
    simpleMemory = checked;
    if(checked){
        ui->groupBoxSimpleMemory->setVisible(true);
        ui->groupBoxSimpleMemory->setEnabled(true);
        ui->plainTextMemory->setVisible(false);
        ui->plainTextMemory->setEnabled(false);
    }else{
        ui->plainTextMemory->setVisible(true);
        ui->plainTextMemory->setEnabled(true);
        ui->groupBoxSimpleMemory->setVisible(false);
        ui->groupBoxSimpleMemory->setEnabled(false);
    }
    updateMemoryTab();
}
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    currentSMScroll = arg1;
    updateMemoryTab();
}

void MainWindow::executeInstruction(){
    if(currentVersionIndex ==  0){
        executeInstructionM6800();
    } else if(currentVersionIndex ==  1){
        executeInstructionM6803();
    }
    if (breakEnabled){
        switch(ui->comboBoxBreakWhen->currentIndex()){
        case 1:
            if(getLineByAddress(PC) - 1 == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 2:
            if(PC == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 3:
            if(SP == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 4:
            if(xRegister == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 5:
            if(aReg == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 6:
            if(bReg == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 7:
            if(bit(flags,5) == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 8:
            if(bit(flags,4) == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 9:
            if(bit(flags,3) == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 10:
            if(bit(flags,2) == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 11:
            if(bit(flags,1) == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 12:
            if(bit(flags,0) == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        case 13:
            if(Memory[ui->spinBoxBreakAt->value()] == ui->spinBoxBreakIs->value()){
                running = false;
                executionTimer->stop();
            }
            break;
        }
    }
}

bool MainWindow::compileM6800(){
    resetEmulator();
    compiled = 0;
    ui->buttonCompile->setStyleSheet("");
    ui->buttonCompile->setStyleSheet(compiledButton);
    int i_ = 0;
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
        bool operandCallsLabel = false;
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
        line = line.toUpper();
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
                    label = line.sliced(0, charNum);
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
                    Err("Label cannot contain a space and must be followed by a tab");
                    goto end;
                }
                else {
                    Err("Unexpected character: '" % line[charNum] % "'");
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
        } else if (line[0].isDigit()){
            Err("Label cannot start with a number: '" % line[0] % "'");
            goto end;
        }
        else {
            Err("Line cannot start with: '" % line[0] % "'");
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
                        in = line.sliced(start);
                        goto operationIdentification;
                    }
                }
                else if (line[charNum] == ' ') {
                    in = (line.sliced(start, charNum - start));
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
                    Err("Cannot mix immediate and indexed");
                    goto end;
                }
                charNum++;
                if (line.sliced(charNum).isEmpty()) {
                    charNum--;
                    Err("Invalid operand");
                    goto end;
                }
                if (line[charNum].isLetter()) {
                    operandCallsLabel = true;
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
                else if (line[charNum] == '%' || line[charNum] == '-' || line[charNum].isDigit()) {
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
                        Err("Invlaid ASCII character");
                        goto end;
                    }
                }
                else {
                    Err("Unexpected character: '" % line[charNum] % "'");
                    goto end;
                }
            }
            else if (line[charNum].isLetter()) {
                operandCallsLabel = true;
                charNum++;
                for (; charNum < line.size(); ++charNum) {
                    if (line.size() - 2 == charNum) {
                        if (line[charNum] == ',') {
                            if (line[charNum + 1] == 'X') {
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
                            if (line[charNum + 1] == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
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
                            if (line[charNum + 1] == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
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
        charNum = 1000;
        if (allInstructionsM6800.indexOf(in) == -1 && specialInstructions.indexOf(in) == -1) {
            Err("Unknown instruction");
            goto end;
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
                            Err("Invalid number: " + op);
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
                            Err("Invalid number: " + op);
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
                            Err("Invalid number: " + op);
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
        else if (vseInstructionsM6800.indexOf(in) != -1) { //VSE
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
                    inCode = 0x0F;
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
                Err("Instruction won't take immediate data");
                goto end;
            }
            if (op.contains(",")) {
                Err("Instruction won't take indirect data");
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
                if(labelValMap.count(op) == 0){
                    callLabelRelMap[currentAddress + 1] = op;
                    opCode = 0;
                }else{
                    int location2 = labelValMap[op];
                    int value;
                    bool ok;
                    value = location2 - currentAddress;
                    if (value < 0)
                        if(value < 0){
                            value-=2;
                        }
                    if (value > 127 || value < -128) {
                        Err("Relative address out of range[-128,127]: " + QString::number(value));
                        currentLine = getLineByAddress(currentAddress);
                        goto end;
                    }
                    qint8 signedValue = static_cast<qint8>(value);
                    value = signedValue & 0xFF;
                    opCode = value;
                }
            }
            else if (op.startsWith('$')) {
                QString hexValue = op.sliced(1);
                if (opCode > 0xFD) {
                    Err("Relative address out of range[-128,127]: " + hexValue);
                    goto end;
                }
                opCode = hexValue.toInt(&ok, 16);
                if (!ok) {
                    Err("Invalid hexadecimal number: " + op);
                    goto end;
                }
            }
            else if (op.startsWith('%')) {
                QString binaryValue = op.sliced(1);
                opCode = binaryValue.toInt(&ok, 2);
                if (opCode > 0xFD) {
                    Err("Relative address out of range[-128,127]: " + binaryValue);
                    goto end;
                }
                if (!ok) {
                    Err("Invalid binary number: " + op);
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
                Err("Instruction does not take indirect data");
                goto end;
            }
            bool ok;
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
            Err("Really invalid instruction");
            goto end;
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
                PrintConsole("Value ("+QString::number(value)+") called by '" +label+"'at location: '" + QString::number(location)+"' is out of instructions range. Entered least significant byte",1);
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
            bool ok;
            value = location2 - location -1;
            if (value < 0)
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
        compiled = 0;
        labelValMap.clear();
        callLabelMap.clear();
        callLabelRazMap.clear();
        callLabelRelMap.clear();
        clearInstructions();
        QList<QTextEdit::ExtraSelection> linesSelections;
        QList<QTextEdit::ExtraSelection> CodeSelections;
        QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(currentLine);
        QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(currentLine);

        QTextCursor codeCursor(codeBlock);
        codeCursor.select(QTextCursor::LineUnderCursor);
        QTextCursor linesCursor(linesBlock);
        linesCursor.select(QTextCursor::LineUnderCursor);

        QTextCharFormat lineFormat;
        lineFormat.setBackground(Qt::darkYellow);
        QTextCharFormat charFormat;
        charFormat.setBackground(Qt::red);

        QTextEdit::ExtraSelection linesLineSelection;
        linesLineSelection.format = lineFormat;
        linesLineSelection.cursor = linesCursor;
        linesSelections.append(linesLineSelection);
        ui->plainTextLines->setExtraSelections(linesSelections);

        QTextEdit::ExtraSelection codeLineSelection;
        codeLineSelection.format = lineFormat;
        codeLineSelection.cursor = codeCursor;
        CodeSelections.append(codeLineSelection);

        QTextCursor charCursor(codeBlock);
        charCursor.setPosition(codeBlock.position() + charNum);
        charCursor.setPosition(codeBlock.position() + charNum + 1, QTextCursor::KeepAnchor);

        QTextEdit::ExtraSelection charSelection;
        charSelection.format = charFormat;
        charSelection.cursor = charCursor;

        CodeSelections.append(charSelection);

        ui->plainTextCode->setExtraSelections(CodeSelections);
        ui->buttonCompile->setStyleSheet("");
        ui->buttonCompile->setStyleSheet(uncompiledButton);
        return false;
    }else{
        return true;
    }
}
bool MainWindow::compileM6803(){
    resetEmulator();
    compiled = 0;
    ui->buttonCompile->setStyleSheet("");
    ui->buttonCompile->setStyleSheet(compiledButton);
    int i_ = 0;
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
        bool operandCallsLabel = false;
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
        line = line.toUpper();
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
                    label = line.sliced(0, charNum);
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
                    Err("Label cannot contain a space and must be followed by a tab");
                    goto end;
                }
                else {
                    Err("Unexpected character: '" % line[charNum] % "'");
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
            Err("Label cannot start with a number: '" % line[0] % "'");
            goto end;
        }
        else {
            Err("Line cannot start with: '" % line[0] % "'");
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
                        in = line.sliced(start);
                        goto operationIdentification;
                    }
                }
                else if (line[charNum] == ' ') {
                    in = (line.sliced(start, charNum - start));
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
                    Err("Cannot mix immediate and indexed");
                    goto end;
                }
                charNum++;
                if (line.sliced(charNum).isEmpty()) {
                    charNum--;
                    Err("Invalid operand");
                    goto end;
                }
                if (line[charNum].isLetter()) {
                    operandCallsLabel = true;
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
                else if (line[charNum] == '%' || line[charNum] == '-' || line[charNum].isDigit()) {
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
                        Err("Invlaid ASCII character");
                        goto end;
                    }
                }
                else {
                    Err("Unexpected character: '" % line[charNum] % "'");
                    goto end;
                }
            }
            else if (line[charNum].isLetter()) {
                operandCallsLabel = true;
                charNum++;
                for (; charNum < line.size(); ++charNum) {
                    if (line.size() - 2 == charNum) {
                        if (line[charNum] == ',') {
                            if (line[charNum + 1] == 'X') {
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
                            if (line[charNum + 1] == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
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
                            if (line[charNum + 1] == 'X') {
                                charNum++;
                                break;
                            }
                            else {
                                Err("Invalid indexing register");
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
        charNum = 1000;
        if (allInstructionsM6803.indexOf(in) == -1 && specialInstructions.indexOf(in) == -1) {
            Err("Unknown instruction");
            goto end;
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
                            Err("Invalid number: " + op);
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
                            Err("Invalid number: " + op);
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
                            Err("Invalid number: " + op);
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
        else if (vseInstructionsM6803.indexOf(in) != -1) { //VSE
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
                    inCode = 0x0F;
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
                Err("Instruction won't take indirect data");
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
                if(labelValMap.count(op) == 0){
                    callLabelRelMap[currentAddress + 1] = op;
                    opCode = 0;
                }else{
                    int location2 = labelValMap[op];
                    int value;
                    bool ok;
                    value = location2 - currentAddress;
                    if (value < 0)
                        if(value < 0){
                            value-=2;
                        }
                    if (value > 127 || value < -128) {
                        Err("Relative address out of range[-128,127]: " + QString::number(value));
                        currentLine = getLineByAddress(currentAddress);
                        goto end;
                    }
                    qint8 signedValue = static_cast<qint8>(value);
                    value = signedValue & 0xFF;
                    opCode = value;
                }
            }
            else if (op.startsWith('$')) {
                QString hexValue = op.sliced(1);
                if (opCode > 0xFD) {
                    Err("Relative address out of range[-128,127]: " + hexValue);
                    goto end;
                }
                opCode = hexValue.toInt(&ok, 16);
                if (!ok) {
                    Err("Invalid hexadecimal number: " + op);
                    goto end;
                }
            }
            else if (op.startsWith('%')) {
                QString binaryValue = op.sliced(1);
                opCode = binaryValue.toInt(&ok, 2);
                if (opCode > 0xFD) {
                    Err("Relative address out of range[-128,127]: " + binaryValue);
                    goto end;
                }
                if (!ok) {
                    Err("Invalid binary number: " + op);
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
            bool ok;
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
            Err("Really invalid instruction");
            goto end;
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
            bool ok;
            value = location2 - location -1;
            if (value < 0)
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
        compiled = 0;
        labelValMap.clear();
        callLabelMap.clear();
        callLabelRazMap.clear();
        callLabelRelMap.clear();
        clearInstructions();
        QList<QTextEdit::ExtraSelection> linesSelections;
        QList<QTextEdit::ExtraSelection> CodeSelections;
        QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(currentLine);
        QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(currentLine);

        QTextCursor codeCursor(codeBlock);
        codeCursor.select(QTextCursor::LineUnderCursor);
        QTextCursor linesCursor(linesBlock);
        linesCursor.select(QTextCursor::LineUnderCursor);

        QTextCharFormat lineFormat;
        lineFormat.setBackground(Qt::darkYellow);
        QTextCharFormat charFormat;
        charFormat.setBackground(Qt::red);

        QTextEdit::ExtraSelection linesLineSelection;
        linesLineSelection.format = lineFormat;
        linesLineSelection.cursor = linesCursor;
        linesSelections.append(linesLineSelection);
        ui->plainTextLines->setExtraSelections(linesSelections);

        QTextEdit::ExtraSelection codeLineSelection;
        codeLineSelection.format = lineFormat;
        codeLineSelection.cursor = codeCursor;
        CodeSelections.append(codeLineSelection);

        QTextCursor charCursor(codeBlock);
        charCursor.setPosition(codeBlock.position() + charNum);
        charCursor.setPosition(codeBlock.position() + charNum + 1, QTextCursor::KeepAnchor);

        QTextEdit::ExtraSelection charSelection;
        charSelection.format = charFormat;
        charSelection.cursor = charCursor;

        CodeSelections.append(charSelection);

        ui->plainTextCode->setExtraSelections(CodeSelections);
        ui->buttonCompile->setStyleSheet("");
        ui->buttonCompile->setStyleSheet(uncompiledButton);
        return false;
    }else{
        return true;
    }
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
            executionTimer->stop();
            running = false;
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
        updateFlags(Overflow,(bit(aReg, 7) & !bit(bReg, 7) & !bit(uInt8, 7)) || (!bit(aReg, 7)  & bit(bReg, 7) & bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) & bit(bReg, 7)) || (bit(bReg, 7) & bit(uInt8, 7)) || (bit(uInt8, 7) & !bit(aReg, 7))));
        aReg = uInt8;
        updateElement(regA);
        PC++;
        break;
    case 0x11:
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) & !bit(bReg, 7) & !bit(uInt8, 7)) || (!bit(aReg, 7) & bit(bReg, 7) & bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) & bit(bReg, 7)) || (bit(bReg, 7) & bit(uInt8, 7)) || (bit(uInt8, 7) & !bit(aReg, 7))));
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
        if (!uInt8 && uInt16 >= 0 && uInt16 <= 9 && !uInt82 && uInt162 >= 0 && uInt162 <= 9){
            aReg+=0x0;
            updateFlags(Carry,0);
        }else if (!uInt8 && uInt16 >= 0 && uInt16 <= 8 && !uInt82 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (!uInt8 && uInt16 >= 0 && uInt16 <= 9 && uInt82 && uInt162 >= 0 && uInt162 <= 3){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (!uInt8 && uInt16 >= 0xA && uInt16 <= 0xF && !uInt82 && uInt162 >= 0 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (!uInt8 && uInt16 >= 9 && uInt16 <= 0xF && !uInt82 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (!uInt8 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 && uInt162 >= 0 && uInt162 <= 3){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 >= 0 && uInt16 <= 2 && !uInt82 && uInt162 >= 0 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 >= 0 && uInt16 <= 2 && !uInt82 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 >= 0 && uInt16 <= 3 && uInt82 && uInt162 >= 0 && uInt162 <= 3){
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
        updateFlags(Carry, (bit(aReg, 7) & bit(bReg, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(bReg, 7) & !bit(uInt16, 7)));
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(bReg, 3)) || (bit(aReg, 3) & !bit(uInt16, 3)) || (bit(bReg, 3) & !bit(uInt16, 3)));
        updateFlags(Overflow, (bit(aReg, 7) & bit(bReg, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(bReg, 7) & bit(uInt16, 7)));
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
        if(lastInput == -1){
            lastInput = Memory[0xFFF0];
        }else if(lastInput != Memory[0xFFF0]){
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x81:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=2;
        break;
    case 0x82:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x91:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=2;
        break;
    case 0x92:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0xA1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=2;
        break;
    case 0xA2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=3;
        updateElement(regA);
        break;
    case 0xB1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=3;
        break;
    case 0xB2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xC1:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=2;
        break;
    case 0xC2:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xD1:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=2;
        break;
    case 0xD2:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xE1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=2;
        break;
    case 0xE2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=3;
        updateElement(regB);
        break;
    case 0xF1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=3;
        break;
    case 0xF2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
    updateSelectionsRunTime(PC);
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
            executionTimer->stop();
            running = false;
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
        updateFlags(Overflow,(bit(aReg, 7) & !bit(bReg, 7) & !bit(uInt8, 7)) || (!bit(aReg, 7)  & bit(bReg, 7) & bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) & bit(bReg, 7)) || (bit(bReg, 7) & bit(uInt8, 7)) || (bit(uInt8, 7) & !bit(aReg, 7))));
        aReg = uInt8;
        updateElement(regA);
        PC++;
        break;
    case 0x11:
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) & !bit(bReg, 7) & !bit(uInt8, 7)) || (!bit(aReg, 7) & bit(bReg, 7) & bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) & bit(bReg, 7)) || (bit(bReg, 7) & bit(uInt8, 7)) || (bit(uInt8, 7) & !bit(aReg, 7))));
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
        if (!uInt8 && uInt16 >= 0 && uInt16 <= 9 && !uInt82 && uInt162 >= 0 && uInt162 <= 9){
            aReg+=0x0;
            updateFlags(Carry,0);
        }else if (!uInt8 && uInt16 >= 0 && uInt16 <= 8 && !uInt82 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (!uInt8 && uInt16 >= 0 && uInt16 <= 9 && uInt82 && uInt162 >= 0 && uInt162 <= 3){
            aReg+=0x6;
            updateFlags(Carry,0);
        }else if (!uInt8 && uInt16 >= 0xA && uInt16 <= 0xF && !uInt82 && uInt162 >= 0 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (!uInt8 && uInt16 >= 9 && uInt16 <= 0xF && !uInt82 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (!uInt8 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 && uInt162 >= 0 && uInt162 <= 3){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 >= 0 && uInt16 <= 2 && !uInt82 && uInt162 >= 0 && uInt162 <= 9){
            aReg+=0x60;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 >= 0 && uInt16 <= 2 && !uInt82 && uInt162 >= 0xA && uInt162 <= 0xF){
            aReg+=0x66;
            updateFlags(Carry,1);
        }else if (uInt8 && uInt16 >= 0 && uInt16 <= 3 && uInt82 && uInt162 >= 0 && uInt162 <= 3){
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
        updateFlags(Carry, (bit(aReg, 7) & bit(bReg, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(bReg, 7) & !bit(uInt16, 7)));
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(bReg, 3)) || (bit(aReg, 3) & !bit(uInt16, 3)) || (bit(bReg, 3) & !bit(uInt16, 3)));
        updateFlags(Overflow, (bit(aReg, 7) & bit(bReg, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(bReg, 7) & bit(uInt16, 7)));
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
        if(lastInput == -1){
            lastInput = Memory[0xFFF0];
        }else if(lastInput != Memory[0xFFF0]){
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x81:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=2;
        break;
    case 0x82:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(adr,15) & !bit(uInt162,15)) || (!bit(aReg,7) & bit(adr,15) & bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) & bit(adr,15)) || (bit(adr,15) & bit(uInt162,15)) || (bit(uInt162,15) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0x91:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=2;
        break;
    case 0x92:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(adr,15) & !bit(uInt162,15)) || (!bit(aReg,7) & bit(adr,15) & bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) & bit(adr,15)) || (bit(adr,15) & bit(uInt162,15)) || (bit(uInt162,15) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=2;
        updateElement(regA);
        break;
    case 0xA1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=2;
        break;
    case 0xA2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(adr,15) & !bit(uInt162,15)) || (!bit(aReg,7) & bit(adr,15) & bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) & bit(adr,15)) || (bit(adr,15) & bit(uInt162,15)) || (bit(uInt162,15) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        aReg = uInt82;
        PC+=3;
        updateElement(regA);
        break;
    case 0xB1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
        PC+=3;
        break;
    case 0xB2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(aReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(aReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & !bit(adr,15) & !bit(uInt162,15)) || (!bit(aReg,7) & bit(adr,15) & bit(uInt162,15)));
        updateFlags(Carry,(!bit(aReg,7) & bit(adr,15)) || (bit(adr,15) & bit(uInt162,15)) || (bit(uInt162,15) & !bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(aReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(aReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(aReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(aReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(aReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(aReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit((*curIndReg), 15) & !bit(uInt16, 15) & !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xC1:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=2;
        break;
    case 0xC2:
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & bit(uInt16,15) & !bit(uInt162,12)) || (!bit(aReg,7) & !bit(uInt16,15) & bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) & bit(uInt16,15)) || (bit(uInt16,15) & !bit(uInt162,12)) || (!bit(uInt162,12) & bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xD1:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=2;
        break;
    case 0xD2:
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & bit(uInt16,15) & !bit(uInt162,12)) || (!bit(aReg,7) & !bit(uInt16,15) & bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) & bit(uInt16,15)) || (bit(uInt16,15) & !bit(uInt162,12)) || (!bit(uInt162,12) & bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=2;
        updateElement(regB);
        break;
    case 0xE1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=2;
        break;
    case 0xE2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & bit(uInt16,15) & !bit(uInt162,12)) || (!bit(aReg,7) & !bit(uInt16,15) & bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) & bit(uInt16,15)) || (bit(uInt16,15) & !bit(uInt162,12)) || (!bit(uInt162,12) & bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        bReg = uInt82;
        PC+=3;
        updateElement(regB);
        break;
    case 0xF1:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
        PC+=3;
        break;
    case 0xF2:
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8 - (flags & 0x1);
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) & !bit(uInt8,7) & !bit(uInt82,7)) || (!bit(bReg,7) & bit(uInt8,7) & bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) & bit(uInt8,7)) || (bit(uInt8,7) & bit(uInt82,7)) || (bit(uInt82,7) & !bit(bReg,7)));
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
        updateFlags(Overflow,(bit(aReg,7) & bit(uInt16,15) & !bit(uInt162,12)) || (!bit(aReg,7) & !bit(uInt16,15) & bit(uInt162,12)));
        updateFlags(Carry,(bit(aReg,7) & bit(uInt16,15)) || (bit(uInt16,15) & !bit(uInt162,12)) || (!bit(uInt162,12) & bit(aReg,7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
        updateFlags(HalfCarry, (bit(bReg, 3) & bit(uInt8, 3)) || (bit(uInt8, 3) & !bit(uInt16, 3)) || (!bit(uInt16, 3) & bit(bReg, 3)));
        updateFlags(Negative, bit(uInt16, 7));
        updateFlags(Overflow, (bit(bReg, 7) & bit(uInt8, 7) & !bit(uInt16, 7)) || (!bit(bReg, 7) & !bit(uInt8, 7) & bit(uInt16, 7)));
        updateFlags(Carry, (bit(bReg, 7) & bit(uInt8, 7)) || (!bit(uInt16, 7) & bit(bReg, 7)) || (bit(uInt8, 7) & !bit(uInt16, 7)));
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
    updateSelectionsRunTime(PC);
    return true;
}


bool displayFS;
void MainWindow::on_buttonFSDisplay_clicked()
{
    if(displayFS){
        displayFS = false;

    }else{
        displayFS = true;

    }
}
