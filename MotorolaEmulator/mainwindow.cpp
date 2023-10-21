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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    clearInstructions();
    QWidget::setWindowTitle("Motorola M68XX Emulator-"+softwareVersion);
    updateMemoryTab();

    for (int col = 0; col < ui->treeWidget->columnCount(); ++col) {
        if(col == 0){
            ui->treeWidget->setColumnWidth(col, 90);
        }else if (col == 1){
            ui->treeWidget->setColumnWidth(col, 200);
        }else{
            ui->treeWidget->setColumnWidth(col, 30);
        }
    }
    for (int row = 0; row < ui->treeWidget->topLevelItemCount(); ++row) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(row);

        if (row % 2 == 0) {
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col) {
                item->setBackground(col, QBrush(QColor(230, 230, 230)));
            }
        } else {
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col) {
                item->setBackground(col, QBrush(QColor(240, 240, 240)));
            }
        }
    }
    ui->labelRunningIndicatior->setVisible(false);
    ui->labelRunningIndicatior->setText("Operation/second: " + QString::number(std::pow(2, ui->comboBoxSpeedSelector->currentIndex())));

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
    ui->plainTextLines->setTextInteractionFlags(Qt::NoTextInteraction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

inline bool bit(int variable, int bitNum){
    return (variable & (1 << bitNum)) != 0;
}
QString convertToQString(int number, int width){
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(width) << number;
    std::string str = stream.str();
    return QString::fromStdString(str);
}

void MainWindow::stopExecution(){
        running = false;
        executionTimer->stop();
        ui->labelRunningIndicatior->setVisible(false);
}
void MainWindow::startExecution(){
        running = true;
        ui->labelRunningIndicatior->setVisible(true);
        executeInstruction();
        if(running == true){
            executionTimer->start(executionSpeed);
        }
}

QList<QTextEdit::ExtraSelection> linesSelectionsRunTime;
QList<QTextEdit::ExtraSelection> codeSelectionsRunTime;
QList<QTextEdit::ExtraSelection> memorySelectionsRunTime;
QList<QTextEdit::ExtraSelection> linesSelectionsLines;
QList<QTextEdit::ExtraSelection> codeSelectionsLines;
QList<QTextEdit::ExtraSelection> memorySelectionsLines;
QList<QTextEdit::ExtraSelection> memorySelectionsMemoryEdit;
int previousScrollCode = 0;
int previousScrollMemory = 0;
int autoScrollUpLimit = 20;
int autoScrollDownLimit = 5;
int lastLinesSelection = -1;
int lastLinesAddress = -1;
int lastMemoryAddressSelection = -1;
int currentAddressSelection = 0;
void MainWindow::updateMemoryTab(){
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
        if(address == lastLinesAddress){
            updateSelectionsLines(lastLinesSelection);
        }
        if(address == lastMemoryAddressSelection){
            updateSelectionsMemoryEdit(address);
        }
    }
}
void MainWindow::updateLinesBox(){
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
    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
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

void MainWindow::PrintConsole(const QString& text, int type){
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
void MainWindow::Err(const QString& text){
    PrintConsole("Ln:" + QString::number(currentLine) + " " + text, 0);
}


void MainWindow::updateSelectionsLines(int line){
    lastLinesSelection = line;
    linesSelectionsLines.clear();
    codeSelectionsLines.clear();
    memorySelectionsLines.clear();
    QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(line);
    QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(line);
    QTextCursor codeCursor(codeBlock);
    codeCursor.select(QTextCursor::LineUnderCursor);
    QTextCursor linesCursor(linesBlock);
    linesCursor.select(QTextCursor::LineUnderCursor);
    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::green);
    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    int address = getAddressByLine(line+1);
    if(address >= 0){
        int lineM = std::floor(address / 16)+1;
        int position = (getAddressByLine(line+1) % 16) * 3 + 4;
        QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
        QTextCursor memoryCursor(memoryBlock);
        memoryCursor.setPosition(lineM*55 + position);
        memoryCursor.setPosition(lineM*55 + position+2,QTextCursor::KeepAnchor);
        lineSelection.cursor = memoryCursor;
        memorySelectionsLines.append(lineSelection);
        lastLinesAddress = address;
    }else{
        lastLinesAddress = -1;
    }


    lineSelection.cursor = linesCursor;
    linesSelectionsLines.append(lineSelection);

    lineSelection.cursor = codeCursor;
    codeSelectionsLines.append(lineSelection);

    QList<QTextEdit::ExtraSelection> combinedLinesSelections;
    QList<QTextEdit::ExtraSelection> combinedCodeSelections;
    QList<QTextEdit::ExtraSelection> combinedMemorySelections;
    combinedLinesSelections.append(linesSelectionsRunTime);
    combinedLinesSelections.append(linesSelectionsLines);
    combinedCodeSelections.append(codeSelectionsRunTime);
    combinedCodeSelections.append(codeSelectionsLines);
    combinedMemorySelections.append(memorySelectionsRunTime);
    combinedMemorySelections.append(memorySelectionsLines);
    combinedMemorySelections.append(memorySelectionsMemoryEdit);

    ui->plainTextLines->setExtraSelections(combinedLinesSelections);
    ui->plainTextCode->setExtraSelections(combinedCodeSelections);
    ui->plainTextMemory->setExtraSelections(combinedMemorySelections);

}
void MainWindow::updateSelectionsMemoryEdit(int address){
    memorySelectionsMemoryEdit.clear();
    clearSelection(3);
    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::lightGray);
    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    int line = std::floor(address / 16)+1;
    int position = (address % 16) * 3 + 4;
    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
    QTextCursor memoryCursor(memoryBlock);
    memoryCursor.setPosition(line*55 + position);
    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);
    lineSelection.cursor = memoryCursor;
    memorySelectionsMemoryEdit.append(lineSelection);
    QList<QTextEdit::ExtraSelection> combinedMemorySelections;
    combinedMemorySelections.append(memorySelectionsRunTime);
    combinedMemorySelections.append(memorySelectionsLines);
    combinedMemorySelections.append(memorySelectionsMemoryEdit);
    ui->plainTextMemory->setExtraSelections(combinedMemorySelections);
    lastMemoryAddressSelection = address;
}
void MainWindow::updateSelectionCompileError(int charNum){
    breakCompile();
    labelValMap.clear();
    callLabelMap.clear();
    callLabelRazMap.clear();
    callLabelRelMap.clear();
    QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(currentLine);
    QTextCursor codeCursor(codeBlock);
    codeCursor.select(QTextCursor::LineUnderCursor);
    QTextCursor charCursor(codeBlock);
    charCursor.setPosition(codeBlock.position() + charNum);
    charCursor.setPosition(codeBlock.position() + charNum + 1, QTextCursor::KeepAnchor);

    QTextCharFormat lineFormat;
    QList<QTextEdit::ExtraSelection> combinedCodeSelections;
    QTextEdit::ExtraSelection lineSelection;

    lineFormat.setBackground(Qt::darkYellow);
    lineSelection.format = lineFormat;
    lineSelection.cursor = codeCursor;
    combinedCodeSelections.append(lineSelection);


    lineFormat.setBackground(Qt::darkGreen);
    lineSelection.format = lineFormat;
    lineSelection.cursor = charCursor;
    combinedCodeSelections.append(lineSelection);

    ui->plainTextCode->setExtraSelections(combinedCodeSelections);
    if (currentLine > ui->plainTextCode->verticalScrollBar()->value() + autoScrollUpLimit){
        ui->plainTextLines->verticalScrollBar()->setValue(currentLine - autoScrollUpLimit);
        ui->plainTextCode->verticalScrollBar()->setValue(currentLine - autoScrollUpLimit);
    } else if (currentLine < ui->plainTextCode->verticalScrollBar()->value() + autoScrollDownLimit){
        ui->plainTextLines->verticalScrollBar()->setValue(currentLine - autoScrollDownLimit);
        ui->plainTextCode->verticalScrollBar()->setValue(currentLine - autoScrollDownLimit);
    }
}
void MainWindow::updateSelectionsRunTime(int address){
    linesSelectionsRunTime.clear();
    codeSelectionsRunTime.clear();
    memorySelectionsRunTime.clear();

    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::yellow);
    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    int lineNum = getLineByAddress(address) - 1;
    if(lineNum >= 0){
        QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(lineNum);
        QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(lineNum);
        QTextCursor codeCursor(codeBlock);
        codeCursor.select(QTextCursor::LineUnderCursor);
        QTextCursor linesCursor(linesBlock);
        linesCursor.select(QTextCursor::LineUnderCursor);
        lineSelection.cursor = linesCursor;
        linesSelectionsRunTime.append(lineSelection);

        lineSelection.cursor = codeCursor;
        codeSelectionsRunTime.append(lineSelection);

    }

    int line = std::floor(address / 16)+1;
    int position = (address % 16) * 3 + 4;
    QTextBlock memoryBlock = ui->plainTextMemory->document()->findBlockByLineNumber(line);
    QTextCursor memoryCursor(memoryBlock);
    memoryCursor.setPosition(line*55 + position);
    memoryCursor.setPosition(line*55 + position+2,QTextCursor::KeepAnchor);



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
    combinedMemorySelections.append(memorySelectionsMemoryEdit);

    ui->plainTextLines->setExtraSelections(combinedLinesSelections);
    ui->plainTextCode->setExtraSelections(combinedCodeSelections);
    ui->plainTextMemory->setExtraSelections(combinedMemorySelections);


}
void MainWindow::clearSelection(int clearWhat) {
    // 0=clear all 1= clear lines election  2= clear runTime 3 = clear memoryedit
    if (clearWhat == 0) {
        linesSelectionsLines.clear();
        codeSelectionsLines.clear();
        memorySelectionsLines.clear();
        linesSelectionsRunTime.clear();
        codeSelectionsRunTime.clear();
        memorySelectionsRunTime.clear();
        memorySelectionsMemoryEdit.clear();
        lastLinesSelection = -1;
        lastLinesAddress = -1;
        lastMemoryAddressSelection = -1;
        QList<QTextEdit::ExtraSelection> combinedLinesSelections;
        QList<QTextEdit::ExtraSelection> combinedCodeSelections;
        QList<QTextEdit::ExtraSelection> combinedMemorySelections;

        ui->plainTextLines->setExtraSelections(combinedLinesSelections);
        ui->plainTextCode->setExtraSelections(combinedCodeSelections);
        ui->plainTextMemory->setExtraSelections(combinedMemorySelections);
    }
    else if (clearWhat == 1) {
        linesSelectionsLines.clear();
        codeSelectionsLines.clear();
        memorySelectionsLines.clear();
        lastLinesSelection = -1;
        lastLinesAddress = -1;
        QList<QTextEdit::ExtraSelection> combinedLinesSelections;
        QList<QTextEdit::ExtraSelection> combinedCodeSelections;
        QList<QTextEdit::ExtraSelection> combinedMemorySelections;
        combinedLinesSelections.append(linesSelectionsRunTime);
        combinedCodeSelections.append(codeSelectionsRunTime);
        combinedMemorySelections.append(memorySelectionsRunTime);
        combinedMemorySelections.append(memorySelectionsMemoryEdit);

        ui->plainTextLines->setExtraSelections(combinedLinesSelections);
        ui->plainTextCode->setExtraSelections(combinedCodeSelections);
        ui->plainTextMemory->setExtraSelections(combinedMemorySelections);
    }
    else if (clearWhat == 2) {
        linesSelectionsRunTime.clear();
        codeSelectionsRunTime.clear();
        memorySelectionsRunTime.clear();
        QList<QTextEdit::ExtraSelection> combinedLinesSelections;
        QList<QTextEdit::ExtraSelection> combinedCodeSelections;
        QList<QTextEdit::ExtraSelection> combinedMemorySelections;
        combinedLinesSelections.append(linesSelectionsLines);
        combinedCodeSelections.append(codeSelectionsLines);
        combinedMemorySelections.append(memorySelectionsLines);
        combinedMemorySelections.append(memorySelectionsMemoryEdit);

        ui->plainTextLines->setExtraSelections(combinedLinesSelections);
        ui->plainTextCode->setExtraSelections(combinedCodeSelections);
        ui->plainTextMemory->setExtraSelections(combinedMemorySelections);
    }
    else if (clearWhat == 3) {
        memorySelectionsMemoryEdit.clear();
        lastMemoryAddressSelection = -1;
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

}



void MainWindow::resetEmulator(bool failedCompile){
    if (running){
        stopExecution();
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
    ui->plainTextDisplay->setPlainText("                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                       ,");
    if(!failedCompile){
        updateSelectionsRunTime(PC);
        if(lastLinesSelection != -1){
            updateSelectionsLines(lastLinesSelection);
        }
        if(writeToMemory){
            updateSelectionsMemoryEdit(currentAddressSelection);
        }
    }else{

	}
	
}
void MainWindow::breakCompile(){
    compiled = 0;
    clearInstructions();
    updateLinesBox();
    clearSelection(0);
    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
    ui->buttonCompile->setStyleSheet("");
    ui->buttonCompile->setStyleSheet(uncompiledButton);
}
void MainWindow::handleVerticalScrollBarValueChanged(int value){
    ui->plainTextLines->verticalScrollBar()->setValue(value);
}
void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    emit resized(this->size());
}
void MainWindow::handleMainWindowSizeChanged(const QSize& newSize){
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
    ui->treeWidget->setGeometry(5, 5, ui->tabWidget->width() - 15, ui->tabWidget->height() - 35);
}

void MainWindow::on_plainTextCode_textChanged(){

    if (ui->plainTextCode->toPlainText().count('\n') > 65535) {
        QString text = ui->plainTextCode->toPlainText();
        QStringList lines = text.split('\n', Qt::SkipEmptyParts);
        lines = lines.mid(0, 0x10000);
        text = lines.join('\n');
        ui->plainTextCode->setPlainText(text);
    }
    breakCompile();
}
void MainWindow::on_lineEditBin_textChanged(const QString &arg1){
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
void MainWindow::on_lineEditOct_textChanged(const QString &arg1){
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
void MainWindow::on_lineEditHex_textChanged(const QString &arg1){
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
void MainWindow::on_lineEditDec_textChanged(const QString &arg1){
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

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->plainTextDisplay) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() <= Qt::Key_AsciiTilde) {
                char asciiChar = static_cast<char>(keyEvent->key());
                uint8_t asciiValue = static_cast<uint8_t>(asciiChar);
                Memory[0xFFF0] = asciiValue;
                lastInput = asciiValue;
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
    }
    if (obj == ui->plainTextLines){
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent) {
                if (compiled) {
                    if (mouseEvent->button() == Qt::LeftButton) {
                        QPoint mousePos = mouseEvent->pos();
                        QTextCursor cursor = ui->plainTextLines->cursorForPosition(mousePos);
                        if (!cursor.atEnd()) {
                            int lineNumber = cursor.blockNumber();
                            int totalLines = ui->plainTextLines->document()->blockCount();
                            if (lineNumber >= 0 && lineNumber < totalLines) {
                                qDebug() << "Clicked on line number:" << lineNumber;
                                updateSelectionsLines(lineNumber);
                            } else {
                                clearSelection(1);
                                qDebug() << "Clicked out of bounds";
                            }
                        }
                    } else if (mouseEvent->button() == Qt::RightButton) {
                        clearSelection(1);
                        qDebug() << "Right-clicked";
                    }
                }
                return true;
            }
        }
    }
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
                    if (!running){
                        std::memcpy(backupMemory, Memory, sizeof(Memory));
                    }
                    if(compiled){
                        breakCompile();
                    }
                    updateSelectionsMemoryEdit(currentAddressSelection);
                }

                if (keyEvent->key() == Qt::Key_Up ){
                    if(currentAddressSelection - 16 >= 0){
                        currentAddressSelection-=16;
                    }
                    updateSelectionsMemoryEdit(currentAddressSelection);
                }else if (keyEvent->key() == Qt::Key_Down){
                    if(currentAddressSelection + 16 <= 0xFFFF){
                        currentAddressSelection+=16;
                    }
                    updateSelectionsMemoryEdit(currentAddressSelection);
                }else if(keyEvent->key() == Qt::Key_Left){
                    if(currentAddressSelection > 0){
                        currentAddressSelection--;
                    }
                    updateSelectionsMemoryEdit(currentAddressSelection);
                }else if (keyEvent->key() == Qt::Key_Right) {
                    if(currentAddressSelection < 0xFFFF){
                        currentAddressSelection++;
                    }
                    updateSelectionsMemoryEdit(currentAddressSelection);
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
    breakCompile();
    resetEmulator(true);
    clearSelection(0);
}
bool MainWindow::on_buttonCompile_clicked()
{
    bool ok = false;
    if(currentVersionIndex ==  0){
        ok = compileMix(0);
    } else if(currentVersionIndex ==  1){
        ok = compileMix(1);
    }
    resetEmulator(!ok);
    return ok;
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
    resetEmulator(false);
}
void MainWindow::on_buttonStep_clicked()
{
    bool ok = true;;
    if (!compiled && compileOnRun){ok = on_buttonCompile_clicked();}
    if(ok){
        executeInstruction();
    }
    if (running){
        stopExecution();
    }
}
void MainWindow::on_buttonRunStop_clicked()
{
    bool ok = true;
    if (running){
        stopExecution();
    }else {
        if (!compiled && compileOnRun){ok = on_buttonCompile_clicked();}
        if(ok){
            if(ui->checkBoxAutoReset->isChecked()){
                if (Memory[PC] == 0){
                    resetEmulator(false);
                }
            }
            startExecution();
        }
    }
}
void MainWindow::on_comboBoxSpeedSelector_activated(int index)
{
    executionSpeed = std::pow(2, index);
    executionSpeed = std::ceil(1000.0 / executionSpeed);
    if (running){
        stopExecution();
    }
    ui->labelRunningIndicatior->setText("Operation/second: "+ QString::number(std::pow(2, index)));
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
        clearSelection(3);
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
        clearSelection(3);
    }else{
        writeToMemory = true;
        ui->plainTextCode->setReadOnly(true);
        ui->buttonCompile->setEnabled(false);
        ui->checkBoxCompileOnRun->setEnabled(false);
        compileOnRun = false;
        ui->labelWritingMode->setText("Memory");
        updateSelectionsMemoryEdit(currentAddressSelection);

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
void MainWindow::on_spinBox_valueChanged(int arg1)//G
{
    currentSMScroll = arg1;
    updateMemoryTab();
}

void MainWindow::executeInstruction(){//G
    if(currentVersionIndex ==  0){
        executeInstructionM6800();
    } else if(currentVersionIndex ==  1){
        executeInstructionM6803();
    }
    if (breakEnabled){
        switch(ui->comboBoxBreakWhen->currentIndex()){
        case 1:
            if(getLineByAddress(PC) - 1 == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 2:
            if(PC == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 3:
            if(SP == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 4:
            if(xRegister == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 5:
            if(aReg == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 6:
            if(bReg == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 7:
            if(bit(flags,5) == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 8:
            if(bit(flags,4) == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 9:
            if(bit(flags,3) == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 10:
            if(bit(flags,2) == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 11:
            if(bit(flags,1) == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 12:
            if(bit(flags,0) == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        case 13:
            if(Memory[ui->spinBoxBreakAt->value()] == ui->spinBoxBreakIs->value()){
                stopExecution();
            }
            break;
        }
    }
    updateSelectionsRunTime(PC);
    int lineNum = getLineByAddress(PC) - 1;
    if(lineNum >= 0){
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
