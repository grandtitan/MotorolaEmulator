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
#include "InstructionList.h"
#include <QPointer>
#include <unordered_map>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    instructionList.clear();
    externalDisplay = new ExternalDisplay(this);
    plainTextDisplay = externalDisplay->findChild<QPlainTextEdit*>("plainTextDisplay");
    QWidget::setWindowTitle("Motorola M68XX Emulator-"+softwareVersion);
    updateMemoryTab();
    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
    for (int col = 0; col < ui->treeWidget->columnCount(); ++col) {
        if(col == 0){
            ui->treeWidget->setColumnWidth(col, 90);
        }else if (col == 1){
            ui->treeWidget->setColumnWidth(col, 200);
        }else if (col == 8){
            ui->treeWidget->setColumnWidth(col, 50);
        }else if (col == 9 || col == 10){
            ui->treeWidget->setColumnWidth(col, 60);
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
    for (int row = 0; row < ui->treeWidget->topLevelItemCount(); ++row) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(row);
        int colorR = 180;
        if (row == 1 || row == 6 ||row == 12 ||row == 31 ||row == 65 ||row == 71 ||row == 72 || row == 81 || row == 84 || row == 106) {
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col) {
                item->setForeground(col, QBrush(QColor(colorR, 0, 0)));
            }
        } else if (row == 62) {
                item->setForeground(4, QBrush(QColor(colorR, 0, 0)));
        }
        if (row == 9 || row == 10 || row == 11 ||row == 12 ||row == 16||row == 17) {
                item->child(0)->setForeground(0, QBrush(QColor(colorR, 0, 0)));
                item->child(0)->setForeground(1, QBrush(QColor(colorR, 0, 0)));
        }
    }
    ui->labelRunningIndicatior->setVisible(false);
    ui->labelRunningIndicatior->setText("Operation/second: " + QString::number(std::pow(2, ui->comboBoxSpeedSelector->currentIndex())));

    ui->labelRunningCycleNum->setVisible(false);
    ui->labelRunningCycleNum->setText("Instruction cycle: ");
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
    //ui->plainTextCode->installEventFilter(this);
    externalDisplay->installEventFilter(this);

    ui->buttonSwitchWrite->setVisible(false);
    ui->labelWritingMode->setVisible(false);
    ui->buttonSwitchWrite->setEnabled(false);
    ui->labelWritingMode->setEnabled(false);

    ui->labelAt->setVisible(false);
    ui->spinBoxBreakAt->setVisible(false);

    executionTimer = new QTimer(this);
    executionTimer->setTimerType(Qt::PreciseTimer);
    connect(executionTimer, &QTimer::timeout, this, &MainWindow::executeLoop);

}
std::map<QPointer<QLineEdit>, QString> pendingUpdateUMap;
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

void MainWindow::setCompileStatus(bool isCompiled){
    if(isCompiled){
        ui->buttonCompile->setStyleSheet(compiledButton);
        updateLinesBox();
        updateMemoryTab();
        compiled = 1;
    }else{
        compiled = 0;
        ui->buttonCompile->setStyleSheet(uncompiledButton);
        PrintConsole("", 2);
        instructionList.clear();
        updateLinesBox();
        clearSelection(0);
        ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
    }
}


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
        cursor.insertText(QString("%1").arg(Memory[address], 2, 16, QChar('0')));
        if(address >= 0xFB00 && address <= 0xFF37){
            int relativeAddress = address-0xFB00;
            int line = std::floor(relativeAddress / 54);
            int position = (relativeAddress % 54);
            if(ui->comboBoxDisplayStatus->currentIndex() == 1){
                QTextCursor cursord2(plainTextDisplay->document());
                cursord2.setPosition(line * 55 + position);
                cursord2.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                ushort charValue = Memory[address];
                if (charValue < 32 || charValue == 127) {
                    cursord2.insertText(" ");
                } else {
                    cursord2.insertText(QChar(static_cast<ushort>(charValue)));
                }
            } else{
                QTextCursor cursord(ui->plainTextDisplay->document());
                cursord.setPosition(line * 55 + position);
                cursord.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                ushort charValue = Memory[address];
                if (charValue < 32 || charValue == 127) {
                    cursord.insertText(" ");
                } else {
                    cursord.insertText(QChar(static_cast<ushort>(charValue)));
                }
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
        if (instructionList.isEmpty()) {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i < code.count("\n") + 1; i++) {
                text = text + convertToQString(i, 5) + "\n";
            }
            ui->plainTextLines->setPlainText(text);
        }
        else {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i < code.count("\n") + 1; i++) {
                const InstructionList::Instruction& instr = instructionList.getObjectByLine(i);
                if (instr.address == -1) {
                    text = text % convertToQString(i, 5) % ":----\n";
                }
                else {
                    text = text % convertToQString(i, 5)
                           % ":" % QString("%1").arg(instr.address, 4, 16, QChar('0'))
                           % ":" % QString("%1").arg(instr.byte1, 2, 16, QChar('0'))
                           % ":" % QString("%1").arg(instr.byte2, 2, 16, QChar('0'))
                           % ":" % QString("%1").arg(instr.byte3, 2, 16, QChar('0'))
                           % "\n";
                }
            }
            ui->plainTextLines->setPlainText(text);
        }
    }
    else {
        if (instructionList.isEmpty()) {
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
                if (instructionList.getObjectByLine(i).address == -1) {
                    text = text % convertToQString(i, 5) % ":----\n";
                }
                else {
                    text = text % convertToQString(i, 5) % ":" % QString("%1").arg(instructionList.getObjectByLine(i).address, 4, 16, QChar('0')) % "\n";
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
        pendingUpdateUMap.emplace(ui->lineEditHValue, QString::number(value));
        break;
    case InterruptMask:
        flags = (flags & ~(1 << 4)) | (value << 4);
        pendingUpdateUMap.emplace(ui->lineEditIValue, QString::number(value));
        break;
    case Negative:
        flags = (flags & ~(1 << 3)) | (value << 3);
        pendingUpdateUMap.emplace(ui->lineEditNValue, QString::number(value));
        break;
    case Zero:
        flags = (flags & ~(1 << 2)) | (value << 2);
        pendingUpdateUMap.emplace(ui->lineEditZValue, QString::number(value));
        break;
    case Overflow:
        flags = (flags & ~(1 << 1)) | (value << 1);
        pendingUpdateUMap.emplace(ui->lineEditVValue, QString::number(value));
        break;
    case Carry:
        flags = (flags & ~(1)) | (value);
        pendingUpdateUMap.emplace(ui->lineEditCValue, QString::number(value));
        break;
    default:
        throw;
    }
}
void MainWindow::updateElement(elementToUpdate element){
    switch (element) {
    case regPC:
        if(hexReg){
            pendingUpdateUMap.emplace(ui->lineEditPCValue, QString("%1").arg(PC, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            pendingUpdateUMap.emplace(ui->lineEditPCValue, QString::number(PC));
        }
        break;
    case regSP:
        if(hexReg){
            pendingUpdateUMap.emplace(ui->lineEditSPValue, QString("%1").arg(SP, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            pendingUpdateUMap.emplace(ui->lineEditSPValue, QString::number(SP));
        }
        break;
    case regA:
        if(hexReg){
            pendingUpdateUMap.emplace(ui->lineEditAValue, QString("%1").arg(aReg, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            pendingUpdateUMap.emplace(ui->lineEditAValue, QString::number(aReg));
        }
        break;
    case regB:
        if(hexReg){
            pendingUpdateUMap.emplace(ui->lineEditBValue, QString("%1").arg(bReg, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            pendingUpdateUMap.emplace(ui->lineEditBValue, QString::number(bReg));
        }
        break;
    case regX:
        if(hexReg){
            pendingUpdateUMap.emplace(ui->lineEditXValue, QString("%1").arg(xRegister, 4, 16, QLatin1Char('0')).toUpper());
        }else{
            pendingUpdateUMap.emplace(ui->lineEditXValue, QString::number(xRegister));
        }
        break;
    case allFlags:
        pendingUpdateUMap.emplace(ui->lineEditHValue, QString::number(bit(flags,5)));
        pendingUpdateUMap.emplace(ui->lineEditIValue, QString::number(bit(flags,4)));
        pendingUpdateUMap.emplace(ui->lineEditNValue, QString::number(bit(flags,3)));
        pendingUpdateUMap.emplace(ui->lineEditZValue, QString::number(bit(flags,2)));
        pendingUpdateUMap.emplace(ui->lineEditVValue, QString::number(bit(flags,1)));
        pendingUpdateUMap.emplace(ui->lineEditCValue, QString::number(bit(flags,0)));
        break;
    default:
        throw;
    }
}

void MainWindow::PrintConsole(const QString& text, int type){
    QString consoleText;
    if (type == -1) { // DEBUG
        consoleText = "DEBUG: " + ("Ln:" + QString::number(currentCompilerLine)) + " " + text;
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
    PrintConsole("Ln:" + QString::number(currentCompilerLine) + " " + text, 0);
}


void MainWindow::updateSelectionsLines(int line){
    lastLinesSelection = line;
    if(lastLinesSelection != -1){
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
    int address = instructionList.getObjectByLine(line).address;
    if(address >= 0){
        int lineM = std::floor(address / 16)+1;
        int position = (address % 16) * 3 + 4;
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
    QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(currentCompilerLine);
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
    if (currentCompilerLine > ui->plainTextCode->verticalScrollBar()->value() + autoScrollUpLimit){
        ui->plainTextLines->verticalScrollBar()->setValue(currentCompilerLine - autoScrollUpLimit);
        ui->plainTextCode->verticalScrollBar()->setValue(currentCompilerLine - autoScrollUpLimit);
    } else if (currentCompilerLine < ui->plainTextCode->verticalScrollBar()->value() + autoScrollDownLimit){
        ui->plainTextLines->verticalScrollBar()->setValue(currentCompilerLine - autoScrollDownLimit);
        ui->plainTextCode->verticalScrollBar()->setValue(currentCompilerLine - autoScrollDownLimit);
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
    int lineNum = instructionList.getObjectByAddress(address).lineNumber;
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

int skipUpdateNum;
int skipUpdateCount;

void MainWindow::resetEmulator(bool failedCompile){
    if (running){
        stopExecution();
    }

    skipUpdateCount = 0;
    waitCycles = 0;
    cycleNum = 1;
    pendingUpdateUMap.clear();
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
    updatePending();
    ui->plainTextDisplay->setPlainText("                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                       ,");
    plainTextDisplay->setPlainText("                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                       ,");
    if(!failedCompile){
        updateSelectionsRunTime(PC);
        if(lastLinesSelection != -1){
            updateSelectionsLines(lastLinesSelection);
        }
        if(writeToMemory){
            updateSelectionsMemoryEdit(currentCompilerAddressSelection);
        }
    }
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
        if(ui->comboBoxDisplayStatus->currentIndex() == 0){
            ui->plainTextDisplay->setEnabled(true);
            ui->plainTextDisplay->setVisible(true);
            ui->frameDisplay->setGeometry(newSize.width() - 875, 10, 498, 350);
            ui->frameDisplay->setEnabled(true);
            ui->frameDisplay->setVisible(true);
        }
        ui->plainTextCode->setGeometry(ui->checkBoxAdvancedInfo->isChecked() ? 190 : 110, ui->plainTextCode->y(), ui->checkBoxAdvancedInfo->isChecked() ? (newSize.width() - 1589) : (newSize.width() - 1509), newSize.height() - buttonYoffset - 17);
        ui->plainTextMemory->setGeometry(newSize.width() - 1390, ui->plainTextMemory->y(), ui->plainTextMemory->width(), newSize.height() - buttonYoffset - 17);
        if (newSize.height() >= 800) {
            ui->tabWidget->setGeometry(newSize.width() - 875, 370, 868, newSize.height() - 359 - buttonYoffset - 17);
        }
        else {
            ui->tabWidget->setGeometry(newSize.width() - 370, 300, 363, newSize.height() - 289 - buttonYoffset - 17);
        }
    }
    else {
        ui->plainTextDisplay->setEnabled(false);
        ui->plainTextDisplay->setVisible(false);
        ui->frameDisplay->setEnabled(false);
        ui->frameDisplay->setVisible(false);

        ui->tabWidget->setGeometry(910, 300, newSize.width() - 917, newSize.height() - 289 - buttonYoffset - 17);
        ui->plainTextCode->setGeometry(ui->checkBoxAdvancedInfo->isChecked() ? 190 : 110, ui->plainTextCode->y(), ui->checkBoxAdvancedInfo->isChecked() ? 201 : 281, newSize.height() - buttonYoffset - 17);
        ui->plainTextMemory->setGeometry(400, ui->plainTextMemory->y(), ui->plainTextMemory->width(), newSize.height() - buttonYoffset - 17);
    }

    ui->buttonCompile->setGeometry(ui->buttonCompile->x(), buttonY, ui->buttonCompile->width(), ui->buttonCompile->height());
    ui->comboBoxVersionSelector->setGeometry(ui->comboBoxVersionSelector->x(), buttonY, ui->comboBoxVersionSelector->width(), ui->comboBoxVersionSelector->height());
    ui->buttonLoad->setGeometry(ui->buttonLoad->x(), buttonY, ui->buttonLoad->width(), ui->buttonLoad->height());
    ui->buttonSave->setGeometry(ui->buttonSave->x(), buttonY, ui->buttonSave->width(), ui->buttonSave->height());
    ui->buttonReset->setGeometry(ui->buttonReset->x(), buttonY, ui->buttonReset->width(), ui->buttonReset->height());
    ui->buttonStep->setGeometry(ui->buttonStep->x(), buttonY, ui->buttonStep->width(), ui->buttonStep->height());
    ui->buttonRunStop->setGeometry(ui->buttonRunStop->x(), buttonY, ui->buttonRunStop->width(), ui->buttonRunStop->height());
    ui->comboBoxSpeedSelector->setGeometry(ui->comboBoxSpeedSelector->x(), buttonY, ui->comboBoxSpeedSelector->width(), ui->comboBoxSpeedSelector->height());
    ui->buttonSwitchWrite->setGeometry(ui->buttonSwitchWrite->x(), buttonY, ui->buttonSwitchWrite->width(), ui->buttonSwitchWrite->height());
    ui->labelWritingMode->setGeometry(ui->labelWritingMode->x(), buttonY, ui->labelWritingMode->width(), ui->labelWritingMode->height());

    ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), ui->plainTextLines->width(), newSize.height() - buttonYoffset - 33);
    ui->lineCodeLinesSeperator->setGeometry(ui->lineCodeLinesSeperator->x(), newSize.height() - buttonYoffset - 24, ui->lineCodeLinesSeperator->width(), 16);

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
    if(!writeToMemory){
        setCompileStatus(false);
    }
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
        if(ui->comboBoxDisplayStatus->currentIndex() == 0){
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() <= Qt::Key_AsciiTilde) {
                char asciiValue = static_cast<uint8_t>(keyEvent->key());
                Memory[0xFFF0] = asciiValue;
                lastInput = asciiValue;
                updateMemoryCell(0xFFF0);
            }
            return true;
        } else if (event->type() == QMouseEvent::MouseButtonPress || event->type() == QMouseEvent::MouseButtonDblClick) {
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
        } else if (event->type() == QMouseEvent::MouseButtonRelease) {
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
    }
    else if (obj == ui->plainTextLines){
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
                                updateSelectionsLines(lineNumber);
                            } else {
                                clearSelection(1);
                            }
                        }
                    } else if (mouseEvent->button() == Qt::RightButton) {
                        clearSelection(1);
                    }
                }
            }
        }else if (event->type() == QEvent::Wheel || event->type() == QEvent::Scroll || event->type() == QEvent::User || event->type() == QEvent::KeyPress){
            return true;
        }
    } else if (obj == externalDisplay){
        if (event->type() == QEvent::Show) {
            plainTextDisplay->setPlainText(ui->plainTextDisplay->toPlainText());
            ui->plainTextDisplay->setEnabled(false);
            ui->plainTextDisplay->setVisible(false);
            ui->frameDisplay->setEnabled(false);
            ui->frameDisplay->setVisible(false);
        } else if (event->type() == QEvent::Hide) {
            ui->plainTextDisplay->setPlainText(plainTextDisplay->toPlainText());
            ui->comboBoxDisplayStatus->setCurrentIndex(0);
            handleMainWindowSizeChanged(MainWindow::size());
        } else if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() <= Qt::Key_AsciiTilde) {
                char asciiValue = static_cast<uint8_t>(keyEvent->key());
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
        } else if(event->type() == QEvent::FocusIn){
            plainTextDisplay->setStyleSheet("QPlainTextEdit:focus { border: 2px solid blue; }");
        } else if(event->type() == QEvent::FocusOut){
            plainTextDisplay->setStyleSheet("");
        }
    }
    else if(writeToMemory){
        if (obj == ui->plainTextMemory){
            if(event->type() == QEvent::KeyPress){
                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

                int keyValue = keyEvent->key();
                if ((keyValue >= Qt::Key_0 && keyValue <= Qt::Key_9) ||
                    (keyValue >= Qt::Key_A && keyValue <= Qt::Key_F)) {
                    char newByte = keyEvent->text().toUpper().toLatin1()[0];
                    uint8_t currentCellValue = Memory[currentCompilerAddressSelection];
                    Memory[currentCompilerAddressSelection] = (currentCellValue << 4) | QString(newByte).toInt(nullptr, 16);;
                    updateMemoryCell(currentCompilerAddressSelection);
                    if (!running){
                        if(compiled){
                            setCompileStatus(false);
                        }
                        std::memcpy(backupMemory, Memory, sizeof(Memory));
                    }

                    updateSelectionsMemoryEdit(currentCompilerAddressSelection);
                }

                if (keyEvent->key() == Qt::Key_Up ){
                    if(currentCompilerAddressSelection - 16 >= 0){
                        currentCompilerAddressSelection-=16;
                    }
                    updateSelectionsMemoryEdit(currentCompilerAddressSelection);
                }else if (keyEvent->key() == Qt::Key_Down){
                    if(currentCompilerAddressSelection + 16 <= 0xFFFF){
                        currentCompilerAddressSelection+=16;
                    }
                    updateSelectionsMemoryEdit(currentCompilerAddressSelection);
                }else if(keyEvent->key() == Qt::Key_Left){
                    if(currentCompilerAddressSelection > 0){
                        currentCompilerAddressSelection--;
                    }
                    updateSelectionsMemoryEdit(currentCompilerAddressSelection);
                }else if (keyEvent->key() == Qt::Key_Right) {
                    if(currentCompilerAddressSelection < 0xFFFF){
                        currentCompilerAddressSelection++;
                    }
                    updateSelectionsMemoryEdit(currentCompilerAddressSelection);
                }
                return true;
            }else{

            }
        }
    }/*/ PAZI KER JE ATTACH FILTER COMMENT OUTANE
    else if (obj == ui->plainTextCode){
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if (keyEvent->modifiers() == Qt::ControlModifier)
            {
                if (keyEvent->key() == Qt::Key_Plus)
                {
                    qDebug() << obj->objectName();
                    changeFontSize(1);
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Minus)
                {
                    qDebug() << obj->objectName();
                    changeFontSize(-1);
                    return true;
                }
            }
        }else if (event->type() == QEvent::Wheel)
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->modifiers() == Qt::ControlModifier)
            {
                int delta = wheelEvent->angleDelta().y();
                int size = changeFontSize(delta > 0 ? 1 : -1);
                return true;
            }
        }
    }/*/
    return QMainWindow::eventFilter(obj, event);
}
int MainWindow::changeFontSize(int delta)
{
    QFont font = ui->plainTextCode->font();
    int newSize = font.pointSize() + delta;
    if (newSize > 5 && newSize < 50)
    {
        font.setPointSize(newSize);
        ui->plainTextCode->setFont(font);
    }
    return newSize;
}
void MainWindow::on_comboBoxVersionSelector_currentIndexChanged(int index)
{
    compilerVersionIndex = index;
    setCompileStatus(false);
    resetEmulator(true);
    if(writeToMemory){
        updateSelectionsMemoryEdit(lastMemoryAddressSelection);
    }
}
bool MainWindow::on_buttonCompile_clicked()
{
    ui->plainTextConsole->clear();
    if(!writeToMemory){
        bool ok = false;
        ok = compileMix(compilerVersionIndex);
        resetEmulator(!ok);
        return ok;
    }else{
        bool ok;
        QString text = QInputDialog::getText(this, "Input Dialog", "Enter the decimal address where the program beggins. Data before that will be written with .BYTE.", QLineEdit::Normal, QString(), &ok);
        if (ok) {
            bool iok;
            int number = text.toInt(&iok);
            if(iok && number >= 0 && number <= 0xFFFF){
                bool cok = reverseCompile(compilerVersionIndex, number);
                resetEmulator(!cok);
                return cok;
            }else{
                Err("Invalid address");
                return false;
            }
        } else {
            PrintConsole("Decompile canceled",1);
            return false;
        }
    }

}
void MainWindow::on_buttonLoad_clicked()
{
    if(!writeToMemory){
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
    }else{
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Binary Files (*.bin);;All Files (*)"));

        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly)) {
              QByteArray byteArray = file.readAll();
              if (byteArray.size() == sizeof(Memory)) {
                    stopExecution();
                    if(compiled){
                        setCompileStatus(false);
                    }
                    std::memcpy(Memory, byteArray.constData(), sizeof(Memory));
                    std::memcpy(backupMemory, Memory, sizeof(Memory));
                    updateMemoryTab();
                    resetEmulator(false);
              } else {
                    PrintConsole("Error: File size doesn't match Memory size", 0);
              }
              file.close();
            } else {
              PrintConsole("Error loading memory", 0);
            }
        }
    }
}
void MainWindow::on_buttonSave_clicked()
{
    if(!writeToMemory){
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
    }else{
        QByteArray byteArray(reinterpret_cast<char*>(Memory), sizeof(Memory));
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Binary Files (*.bin);;All Files (*)"));

        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(byteArray);
                file.close();
            } else {
                PrintConsole("Error saving memory", 0);
            }
        }
    }
}
void MainWindow::on_buttonReset_clicked()
{
    resetEmulator(false);
}
void MainWindow::on_buttonStep_clicked()
{
    if (running){
        stopExecution();
    }
    bool ok = true;;
    if (!compiled && compileOnRun){ok = on_buttonCompile_clicked();}
    if(ok){
        executeInstruction();
        updatePending();
        updateSelectionsRunTime(PC);
    }
}
void MainWindow::on_buttonRunStop_clicked()
{
    if (running){
        stopExecution();
    }else {
        bool ok = true;
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
    if (running){
        stopExecution();
    }
    if(index != 11){
        executionSpeed = std::pow(2, index);
        ui->labelRunningIndicatior->setText("Operation/second: "+ QString::number(executionSpeed));
        if(executionSpeed > 288){
            skipUpdateNum = std::floor(executionSpeed / 144);
        } else{
            skipUpdateNum = 0;
        }
        executionSpeed = std::ceil(1000.0 / executionSpeed);
        qDebug() << skipUpdateNum << " " << executionSpeed;
    } else{
        executionSpeed = 0;
        ui->labelRunningIndicatior->setText("Operation/second: full speed");
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
        ui->lineCodeLinesSeperator->setGeometry(190, ui->lineCodeLinesSeperator->y(), 1, 16);
        updateLinesBox();
    }
    else {
        ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), 101, ui->plainTextLines->height());
        ui->lineCodeLinesSeperator->setGeometry(110, ui->lineCodeLinesSeperator->y(), 1, 16);
        updateLinesBox();
    }
    updateSelectionsLines(lastLinesSelection);
    updateSelectionsRunTime(PC);
    handleMainWindowSizeChanged(MainWindow::size());

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
        //ui->buttonCompile->setEnabled(true);
        ui->checkBoxCompileOnRun->setEnabled(true);
        compileOnRun = true;
        ui->labelWritingMode->setText("Code");
        clearSelection(3);
        ui->buttonLoad->setText("Load Code");
        ui->buttonSave->setText("Save Code");
        ui->buttonCompile->setText("Assemble");
    }
}
void MainWindow::on_buttonSwitchWrite_clicked()
{
    if(writeToMemory){
        writeToMemory = false;
        ui->plainTextCode->setReadOnly(false);
        //ui->buttonCompile->setEnabled(true);
        ui->checkBoxCompileOnRun->setEnabled(true);
        compileOnRun = true;
        ui->labelWritingMode->setText("Code");
        clearSelection(3);
        ui->buttonLoad->setText("Load Code");
        ui->buttonSave->setText("Save Code");
        ui->buttonCompile->setText("Assemble");
    }else{
        writeToMemory = true;
        ui->plainTextCode->setReadOnly(true);
        //ui->buttonCompile->setEnabled(false);
        ui->checkBoxCompileOnRun->setEnabled(false);
        compileOnRun = false;
        ui->labelWritingMode->setText("Memory");
        updateSelectionsMemoryEdit(currentCompilerAddressSelection);
        ui->buttonLoad->setText("Load Memory");
        ui->buttonSave->setText("Save Memory");
        ui->buttonCompile->setText("Disassemble");

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
    updateMemoryTab();
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
        if(writeToMemory) updateSelectionsMemoryEdit(lastMemoryAddressSelection);
        updateSelectionsLines(lastLinesSelection);
        updateSelectionsRunTime(PC);
    }
}
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    currentSMScroll = arg1;
    updateMemoryTab();
}
void MainWindow::on_checkBoxAutoReset_2_clicked(bool checked)
{
    useCyclesPerSecond = checked;
    if(!useCyclesPerSecond){
        ui->labelRunningCycleNum->setVisible(false);

    }else if(running){
        ui->labelRunningCycleNum->setVisible(true);
    }
    waitCycles = 0;
    cycleNum = 0;
}
void MainWindow::on_comboBoxDisplayStatus_currentIndexChanged(int index)
{
    if(index == 1){
        externalDisplay->show();
    } else{
        externalDisplay->hide();
    }
}


void MainWindow::updatePending(){
    for (auto it = pendingUpdateUMap.begin(); it != pendingUpdateUMap.end(); ) {
        it->first->setText(it->second);
        it = pendingUpdateUMap.erase(it);
    }
}
void MainWindow::stopExecution(){
    running = false;
    waitCycles = 0;
    cycleNum = 1;
    executionTimer->stop();
    ui->labelRunningIndicatior->setVisible(false);
    ui->labelRunningCycleNum->setVisible(false);
    updatePending();
    updateSelectionsRunTime(PC);
    skipUpdateCount = 0;
}
void MainWindow::startExecution(){
    running = true;
    ui->labelRunningIndicatior->setVisible(true);
    if(useCyclesPerSecond){
        ui->labelRunningCycleNum->setVisible(true);
    }
    executeLoop();
    if(running == true){
        executionTimer->start(executionSpeed);
    }
}

void MainWindow::executeLoop(){
    if(useCyclesPerSecond){
        if(waitCycles > 0){
            cycleNum++;
            waitCycles--;
        }else{
            updatePending();
            updateSelectionsRunTime(PC);
            waitCycles = executeInstruction();
            cycleNum = 1;
            waitCycles--;
            if (breakEnabled){
                switch(ui->comboBoxBreakWhen->currentIndex()){
                case 1:
                    if(instructionList.getObjectByAddress(PC).lineNumber == ui->spinBoxBreakIs->value()){
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
        }
        ui->labelRunningCycleNum->setText("Instruction cycle: "+ QString::number(cycleNum));
    }else{
        executeInstruction();
        if(skipUpdateNum > 0){
            if(skipUpdateCount > 0){
                skipUpdateCount--;
                pendingUpdateUMap.clear();
            }else{
                skipUpdateCount = skipUpdateNum;
                updatePending();
                updateSelectionsRunTime(PC);
            }
        }else{
            updatePending();
            updateSelectionsRunTime(PC);
        }
        if (breakEnabled){
            switch(ui->comboBoxBreakWhen->currentIndex()){
            case 1:
                if(instructionList.getObjectByAddress(PC).lineNumber == ui->spinBoxBreakIs->value()){
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

    }
}
int oldX = 0;
int oldY = 0;
int MainWindow::executeInstruction(){
    int cycleCount = 1;
    uint8_t uInt8 = 0;
    uint8_t uInt82 = 0;
    int8_t sInt8 = 0;
    uint16_t uInt16 = 0;
    uint16_t uInt162 = 0;
    uint16_t adr = 0;
    uint16_t* curIndReg = &xRegister;
    if(ui->comboBoxDisplayStatus->currentIndex() == 0){
        if(ui->plainTextDisplay->hasFocus()){
            QPoint position = QCursor::pos();
            QPoint localMousePos = ui->plainTextDisplay->mapFromGlobal(position);
            localMousePos.setX(localMousePos.x() - 3);
            localMousePos.setY(localMousePos.y() - 5);
            QFontMetrics fontMetrics(ui->plainTextDisplay->font());
            int charWidth = fontMetrics.averageCharWidth();
            int charHeight = fontMetrics.height();
            int x = localMousePos.x() / charWidth;
            int y = localMousePos.y() / charHeight;
            if(x >= 0 && x <= 53 && y >= 0 && y <= 19){
                oldX = x;
                oldY = y;
            } else{
                x = oldX;
                y = oldY;
            }
            Memory[0xFFF2] = x;
            Memory[0xFFF3] = y;
            updateMemoryCell(0xFFF2);
            updateMemoryCell(0xFFF3);
        }
    }else{
        if (plainTextDisplay->hasFocus()) {
        QPoint position = QCursor::pos();
        QPoint localMousePos = plainTextDisplay->mapFromGlobal(position);
        localMousePos.setX(localMousePos.x() - 3);
        localMousePos.setY(localMousePos.y() - 5);
        QFontMetrics fontMetrics(plainTextDisplay->font());
        int charWidth = fontMetrics.averageCharWidth();
        int charHeight = fontMetrics.height();
        int x = localMousePos.x() / charWidth;
        int y = localMousePos.y() / charHeight;
        if(x >= 0 && x <= 53 && y >= 0 && y <= 19){
                oldX = x;
                oldY = y;
        } else{
                x = oldX;
                y = oldY;
        }
        Memory[0xFFF2] = x;
        Memory[0xFFF3] = y;
        updateMemoryCell(0xFFF2);
        updateMemoryCell(0xFFF3);
        }
    }

    //if (!indexRegister) {
        //curIndReg = &yRegister;
    //}
    switch(Memory[PC]){
    case 0x00:
        if (running){
            stopExecution();
        }else{
            PC++;
        }
        break;
    case 0x01:
        cycleCount = 2;
        PC++;
        //NOP
        break;
    case 0x04:
        if(compilerVersionIndex>=1){
            cycleCount = 3;
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

        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0x05:
        if(compilerVersionIndex>=1){
            cycleCount = 3;
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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x06:
        cycleCount = 2;
        updateFlags(HalfCarry, bit(aReg, 5));
        updateFlags(InterruptMask, bit(aReg, 4));
        updateFlags(Negative, bit(aReg, 3));
        updateFlags(Zero, bit(aReg, 2));
        updateFlags(Overflow, bit(aReg, 1));
        updateFlags(Carry, bit(aReg, 0));
        PC++;
        break;
    case 0x07:
        cycleCount = 2;
        aReg = flags;
        updateElement(regA);
        PC++;
        break;
    case 0x08:
        cycleCount = 3;
        (*curIndReg)++;
        updateFlags(Zero, (*curIndReg) == 0);
        updateElement(regX);
        PC++;
        break;
    case 0x09:
        cycleCount = 3;
        (*curIndReg)--;
        updateFlags(Zero, (*curIndReg) == 0);
        updateElement(regX);
        PC++;
        break;
    case 0x0A:
        cycleCount = 2;
        updateFlags(Overflow, 0);
        PC++;
        break;
    case 0x0B:
        cycleCount = 2;
        updateFlags(Overflow, 1);
        PC++;
        break;
    case 0x0C:
        cycleCount = 2;
        updateFlags(Carry, 0);
        PC++;
        break;
    case 0x0D:
        cycleCount = 2;
        updateFlags(Carry, 1);
        PC++;
        break;
    case 0x0E:
        cycleCount = 2;
        updateFlags(InterruptMask, 0);
        PC++;
        break;
    case 0x0F:
        cycleCount = 2;
        updateFlags(InterruptMask, 1);
        PC++;
        break;
    case 0x10:
        cycleCount = 2;
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
        cycleCount = 2;
        uInt8 = aReg - bReg;
        updateFlags(Negative, bit(uInt8, 7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,(bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7) && bit(bReg, 7) && bit(uInt8, 7)));
        updateFlags(Carry,((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
        PC++;
        break;
    case 0x16:
        cycleCount = 2;
        bReg = aReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC++;
        break;
    case 0x17:
        cycleCount = 2;
        aReg = bReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC++;
        break;
    case 0x19:
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 3;
        sInt8 = Memory[(PC + 1) % 0x10000];
        PC += sInt8 + 2;
        break;
    case 0x21:
        cycleCount = 3;
        if(compilerVersionIndex>=1){
            PC += 2;
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0x22:
        cycleCount = 3;
        if ((bit(flags, 2) || (flags & 0x01)) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x23:
        cycleCount = 3;
        if (bit(flags, 2) || (flags & 0x01))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x24:
        cycleCount = 3;
        if ((flags & 0x01) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x25:
        cycleCount = 3;
        if ((flags & 0x01)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x26:
        cycleCount = 3;
        if (bit(flags, 2) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x27:
        cycleCount = 3;
        if (bit(flags, 2)) {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x28:
        cycleCount = 3;
        if (bit(flags, 1) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x29:
        cycleCount = 3;
        if (bit(flags, 1)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2A:
        cycleCount = 3;
        if (bit(flags, 3) == 0){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2B:
        cycleCount = 3;
        if (bit(flags, 3)){
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2C:
        cycleCount = 3;
        if ((bit(flags, 3) ^ bit(flags, 1)) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2D:
        cycleCount = 3;
        if (bit(flags, 3) ^ bit(flags, 1))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2E:
        cycleCount = 3;
        if ((bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1))) == 0)
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x2F:
        cycleCount = 3;
        if (bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1)))
        {
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8;
        }
        PC+=2;
        break;
    case 0x30:
        cycleCount = 3;
        (*curIndReg) = SP + 1;
        updateElement(regX);
        PC++;
        break;
    case 0x31:
        cycleCount = 3;
        SP++;
        updateElement(regSP);
        PC++;
        break;
    case 0x32:
        cycleCount = 4;
        SP++;
        aReg = Memory[SP];
        updateElement(regSP);
        updateElement(regA);
        PC++;
        break;
    case 0x33:
        cycleCount = 4;
        SP++;
        bReg = Memory[SP];
        updateElement(regSP);
        updateElement(regB);
        PC++;
        break;
    case 0x34:
        cycleCount = 3;
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x35:
        cycleCount = 3;
        SP = (*curIndReg) - 1;
        updateElement(regSP);
        PC++;
        break;
    case 0x36:
        cycleCount = 3;
        Memory[SP] = aReg;
        updateMemoryCell(SP);
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x37:
        cycleCount = 3;
        Memory[SP] = bReg;
        updateMemoryCell(SP);
        SP--;
        updateElement(regSP);
        PC++;
        break;
    case 0x38:
        if(compilerVersionIndex>=1){
            cycleCount = 5;
            SP++;
            (*curIndReg) = (Memory[SP] << 8);
            SP++;
            (*curIndReg) += Memory[SP];
            updateElement(regSP);
            updateElement(regX);
            PC++;
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x39:
        cycleCount = 5;
        SP++;
        PC = Memory[SP] << 8;
        SP++;
        PC += Memory[SP];
        updateElement(regSP);
        break;
    case 0x3A:
        if(compilerVersionIndex>=1){
            cycleCount = 3;
            (*curIndReg) = (*curIndReg) + bReg;
            PC++;
            updateElement(regX);
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x3B:
        cycleCount = 10;
        SP++;
        flags = Memory[SP];

        SP++;
        bReg = Memory[SP];

        SP++;
        aReg = Memory[SP];

        SP++;
        (*curIndReg) = (Memory[SP] << 8) + Memory[(SP+ 1) % 0x10000];

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
        if(compilerVersionIndex>=1){
            cycleCount = 4;
            Memory[SP] = ((*curIndReg) & 0xFF);
            updateMemoryCell(SP);
            Memory[SP - 1] = (((*curIndReg) >> 8) & 0xFF);
            updateMemoryCell(SP-1);
            SP-=2;
            updateElement(regSP);
            PC++;
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x3D:
        if(compilerVersionIndex>=1){
            cycleCount = 10;
            uInt16 = static_cast<uint16_t>(aReg) * static_cast<uint16_t>(bReg);
            updateFlags(Carry, (uInt16 >> 8) != 0);
            aReg = (uInt16 >> 8);
            bReg = (uInt16 & 0xFF);
            updateElement(regA);
            updateElement(regB);
            PC++;
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x3E:
        cycleCount = 9;
        if(lastInput != -1){
            PC++;
            lastInput = -1;
        }
        break;
    case 0x3F:
        cycleCount = 12;
        PC++;
        Memory[SP] = PC & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = (PC >> 8) & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = (*curIndReg) & 0xFF;
        updateMemoryCell(SP);
        SP--;
        Memory[SP] = ((*curIndReg) >> 8) & 0xFF;
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
        cycleCount = 2;
        aReg = 0x0 - aReg;
        updateFlags(Negative,bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow, aReg == 0x80);
        updateFlags(Carry, aReg != 0);
        updateElement(regA);
        PC++;
        break;
    case 0x43:
        cycleCount = 2;
        aReg = 0xFF - aReg;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateElement(regA);
        PC++;
        break;
    case 0x44:
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
        updateFlags(Overflow, aReg == 0x80);
        aReg--;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x4C:
        cycleCount = 2;
        updateFlags(Overflow, aReg == 0x7F);
        aReg++;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateElement(regA);
        PC++;
        break;
    case 0x4D:
        cycleCount = 2;
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC++;
        break;
    case 0x4F:
        cycleCount = 2;
        aReg = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateElement(regA);
        PC++;
        break;
    case 0x50:
        cycleCount = 2;
        bReg = 0x0 - bReg;
        updateFlags(Negative,bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow, bReg == 0x80);
        updateFlags(Carry, bReg != 0);
        updateElement(regB);
        PC++;
        break;
    case 0x53:
        cycleCount = 2;
        bReg = 0xFF - bReg;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,1);
        updateElement(regB);
        PC++;
        break;
    case 0x54:
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
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
        cycleCount = 2;
        updateFlags(Overflow, bReg == 0x80);
        bReg--;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC++;
        break;
    case 0x5C:
        cycleCount = 2;
        updateFlags(Overflow, bReg == 0x7F);
        bReg++;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateElement(regB);
        PC++;
        break;
    case 0x5D:
        cycleCount = 2;
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC++;
        break;
    case 0x5F:
        cycleCount = 2;
        bReg = 0;
        updateFlags(Negative,0);
        updateFlags(Zero, 1);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        updateElement(regB);
        PC++;
        break;
    case 0x60:
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Overflow, Memory[adr] == 0x80);
        Memory[adr]--;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6C:
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Overflow, Memory[adr] == 0x7F);
        Memory[adr]++;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x6D:
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC+=2;
        break;
    case 0x6E:
        cycleCount = 3;
        PC = ((Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000);
        break;
    case 0x6F:
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
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
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Overflow, Memory[adr] == 0x80);
        Memory[adr]--;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7C:
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Overflow, Memory[adr] == 0x7F);
        Memory[adr]++;
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0x7D:
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(Memory[adr], 7));
        updateFlags(Zero, Memory[adr] == 0);
        updateFlags(Overflow,0);
        updateFlags(Carry,0);
        PC+=3;
        break;
    case 0x7E:
        cycleCount = 3;
        PC = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        break;
    case 0x7F:
        cycleCount = 6;
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
        cycleCount = 2;
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
        cycleCount = 2;
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0x82:
        cycleCount = 2;
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
        if(compilerVersionIndex>=1){
            cycleCount = 4;

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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0x84:
        cycleCount = 2;
        aReg = (aReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x85:
        cycleCount = 2;
        uInt8 = (aReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0x86:
        cycleCount = 2;
        aReg = Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x88:
        cycleCount = 2;
        aReg = aReg ^ Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x89:
        cycleCount = 2;
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
        cycleCount = 2;
        aReg = aReg | Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x8B:
        cycleCount = 2;
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
        cycleCount = 4;
        uInt16 = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=3;
        break;
    case 0x8D:
        cycleCount = 6;
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
        cycleCount = 3;
        SP = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regSP);
        break;
    case 0x90:
        cycleCount = 3;
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
        cycleCount = 3;
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0x92:
        cycleCount = 3;
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
        if(compilerVersionIndex>=1){
            cycleCount = 5;
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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x94:
        cycleCount = 3;
        aReg = (aReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x95:
        cycleCount = 3;
        uInt8 = (aReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0x96:
        cycleCount = 3;
        aReg = Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x97:
        cycleCount = 3;
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0x98:
        cycleCount = 3;
        aReg = aReg ^ Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x99:
        cycleCount = 3;
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
        cycleCount = 3;
        aReg = aReg | Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0x9B:
        cycleCount = 3;
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
        cycleCount = 5;
        adr = Memory[(PC+1) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=2;
        break;
    case 0x9D:
        cycleCount = 5;
        if(compilerVersionIndex>=1){
            adr = Memory[(PC+1) % 0x10000];
            PC+=2;
            Memory[SP] = (PC & 0xFF);
            updateMemoryCell(SP);
            Memory[SP - 1] = ((PC >> 8) & 0xFF);
            updateMemoryCell(SP-1);
            SP-=2;
            PC = adr;
            updateElement(regSP);
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0x9E:
        cycleCount = 4;
        adr = Memory[(PC+1) % 0x10000];
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regSP);
        break;
    case 0x9F:
        cycleCount = 4;
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
        cycleCount = 4;
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
        cycleCount = 4;
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=2;
        break;
    case 0xA2:
        cycleCount = 4;
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
        if(compilerVersionIndex>=1){
            cycleCount = 6;
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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0xA4:
        cycleCount = 4;
        aReg = (aReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA5:
        cycleCount = 4;
        uInt8 = (aReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xA6:
        cycleCount = 4;
        aReg = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA7:
        cycleCount = 4;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xA8:
        cycleCount = 4;
        aReg = aReg ^ Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xA9:
        cycleCount = 4;
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
        cycleCount = 4;
        aReg = aReg | Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=2;
        break;
    case 0xAB:
        cycleCount = 4;
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
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=2;
        break;
    case 0xAD:
        cycleCount = 6;
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
        cycleCount = 5;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regSP);
        break;
    case 0xAF:
        cycleCount = 5;
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
        cycleCount = 4;
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
        cycleCount = 4;
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = aReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(aReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(aReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(aReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(aReg,7)));
        PC+=3;
        break;
    case 0xB2:
        cycleCount = 4;
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
        if(compilerVersionIndex>=1){
            cycleCount = 6;
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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }

        break;
    case 0xB4:
        cycleCount = 4;
        aReg = (aReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB5:
        cycleCount = 4;
        uInt8 = (aReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=3;
        break;
    case 0xB6:
        cycleCount = 4;
        aReg = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB7:
        cycleCount = 4;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = aReg;
        updateFlags(Negative, bit(aReg,7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0xB8:
        cycleCount = 4;
        aReg = aReg ^ Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xB9:
        cycleCount = 4;
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
        cycleCount = 4;
        aReg = aReg | Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(aReg, 7));
        updateFlags(Zero, aReg == 0);
        updateFlags(Overflow,0);
        updateElement(regA);
        PC+=3;
        break;
    case 0xBB:
        cycleCount = 4;
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
        cycleCount = 6;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        uInt162 = (*curIndReg) - uInt16;
        updateFlags(Negative, bit(uInt162,15));
        updateFlags(Zero, uInt162 == 0);
        updateFlags(Overflow,(bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
        PC+=3;
        break;
    case 0xBD:
        cycleCount = 6;
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
        cycleCount = 5;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit(SP,15));
        updateFlags(Zero, SP == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regSP);
        break;
    case 0xBF:
        cycleCount = 5;
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
        cycleCount = 2;
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
        cycleCount = 2;
        uInt8 = Memory[(PC+1) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xC2:
        cycleCount = 2;
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
        if(compilerVersionIndex>=1){
            cycleCount = 4;

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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xC4:
        cycleCount = 2;
        bReg = (bReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC5:
        cycleCount = 2;
        uInt8 = (bReg & Memory[(PC+1) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xC6:
        cycleCount = 2;
        bReg = Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC8:
        cycleCount = 2;
        bReg = bReg ^ Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xC9:
        cycleCount = 2;
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
        cycleCount = 2;
        bReg = bReg | Memory[(PC+1) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xCB:
        cycleCount = 2;
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
        if(compilerVersionIndex>=1){
            cycleCount = 3;

            uInt16 = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
            updateFlags(Negative, bit(uInt16,15));
            updateFlags(Zero, uInt16 == 0);
            updateFlags(Overflow, 0);
            aReg = (uInt16 >> 8);
            bReg = (uInt16 & 0xFF);
            updateElement(regA);
            updateElement(regB);
            PC+=3;
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xCE:
        cycleCount = 3;
        (*curIndReg) = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regX);
        break;
    case 0xD0:
        cycleCount = 3;
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
        cycleCount = 3;
        uInt8 = Memory[Memory[(PC+1) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xD2:
        cycleCount = 3;
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
        if(compilerVersionIndex>=1){
            cycleCount = 5;

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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xD4:
        cycleCount = 3;
        bReg =( bReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD5:
        cycleCount = 3;
        uInt8 = (bReg & Memory[Memory[(PC+1) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xD6:
        cycleCount = 3;
        bReg = Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD7:
        cycleCount = 3;
        adr = Memory[(PC+1) % 0x10000];
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xD8:
        cycleCount = 3;
        bReg = bReg ^ Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xD9:
        cycleCount = 3;
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
        cycleCount = 3;
        bReg = bReg | Memory[Memory[(PC+1) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xDB:
        cycleCount = 3;
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
        if(compilerVersionIndex>=1){
            cycleCount = 4;

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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xDD:
        if(compilerVersionIndex>=1){
            cycleCount = 4;
            adr = Memory[(PC+1) % 0x10000];
            Memory[adr] = aReg;
            Memory[adr+1] = bReg;
            updateFlags(Negative, bit(aReg,7));
            updateFlags(Zero, bReg + aReg == 0);
            updateFlags(Overflow,0);
            updateMemoryCell(adr);
            updateMemoryCell(adr+1);
            PC+=2;

        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xDE:
        cycleCount = 4;
        adr = Memory[(PC+1) % 0x10000];
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regX);
        break;
    case 0xDF:
        cycleCount = 4;
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
        cycleCount = 4;
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
        cycleCount = 4;
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=2;
        break;
    case 0xE2:
        cycleCount = 4;
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
        if(compilerVersionIndex>=1){
            cycleCount = 6;
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

        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xE4:
        cycleCount = 4;
        bReg = (bReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE5:
        cycleCount = 4;
        uInt8 = (bReg & Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=2;
        break;
    case 0xE6:
        cycleCount = 4;
        bReg = Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE7:
        cycleCount = 4;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=2;
        break;
    case 0xE8:
        cycleCount = 4;
        bReg = bReg ^ Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xE9:
        cycleCount = 4;
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
        cycleCount = 4;
        bReg = bReg | Memory[(Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=2;
        break;
    case 0xEB:
        cycleCount = 4;
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
        if(compilerVersionIndex>=1){
            cycleCount = 5;

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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xED:
        if(compilerVersionIndex>=1){
            cycleCount = 5;
            adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = aReg;
            Memory[adr+1] = bReg;
            updateFlags(Negative, bit(aReg,7));
            updateFlags(Zero, bReg + aReg == 0);
            updateFlags(Overflow,0);
            updateMemoryCell(adr);
            updateMemoryCell(adr+1);
            PC+=2;

        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xEE:
        cycleCount = 5;
        adr = (Memory[(PC+1) % 0x10000] + *curIndReg) % 0x10000;
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=2;
        updateElement(regX);
        break;
    case 0xEF:
        cycleCount = 5;
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
        cycleCount = 4;
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
        cycleCount = 4;
        uInt8 = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        uInt82 = bReg - uInt8;
        updateFlags(Negative, bit(uInt82,7));
        updateFlags(Zero, uInt82 == 0);
        updateFlags(Overflow,(bit(bReg,7) && !bit(uInt8,7) && !bit(uInt82,7)) || (!bit(bReg,7) && bit(uInt8,7) && bit(uInt82,7)));
        updateFlags(Carry,(!bit(bReg,7) && bit(uInt8,7)) || (bit(uInt8,7) && bit(uInt82,7)) || (bit(uInt82,7) && !bit(bReg,7)));
        PC+=3;
        break;
    case 0xF2:
        cycleCount = 4;
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
        if(compilerVersionIndex>=1){
            cycleCount = 6;

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
        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xF4:
        cycleCount = 4;
        bReg = (bReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF5:
        cycleCount = 4;
        uInt8 = (bReg & Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]]);
        updateFlags(Negative, bit(uInt8,7));
        updateFlags(Zero, uInt8 == 0);
        updateFlags(Overflow,0);
        PC+=3;
        break;
    case 0xF6:
        cycleCount = 4;
        bReg = Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF7:
        cycleCount = 4;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        Memory[adr] = bReg;
        updateFlags(Negative, bit(bReg,7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateMemoryCell(adr);
        PC+=3;
        break;
    case 0xF8:
        cycleCount = 4;
        bReg = bReg ^ Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xF9:
        cycleCount = 4;
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
        cycleCount = 4;
        bReg = bReg | Memory[(Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000]];
        updateFlags(Negative, bit(bReg, 7));
        updateFlags(Zero, bReg == 0);
        updateFlags(Overflow,0);
        updateElement(regB);
        PC+=3;
        break;
    case 0xFB:
        cycleCount = 4;
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
        if(compilerVersionIndex>=1){
            cycleCount = 5;
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

        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xFD:
        if(compilerVersionIndex>=1){
            cycleCount = 5;
            adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
            Memory[adr] = aReg;
            Memory[adr+1] = bReg;
            updateFlags(Negative, bit(aReg,7));
            updateFlags(Zero, bReg + aReg == 0);
            updateFlags(Overflow,0);
            updateMemoryCell(adr);
            updateMemoryCell(adr+1);
            PC+=3;

        } else{
            PrintConsole("Unkown instruction:" + QString::number(Memory[PC]), 1);
            PC++;
        }
        break;
    case 0xFE:
        cycleCount = 5;
        adr = (Memory[(PC+1) % 0x10000] << 8) + Memory[(PC+2) % 0x10000];
        (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
        updateFlags(Negative, bit((*curIndReg),15));
        updateFlags(Zero, (*curIndReg) == 0);
        updateFlags(Overflow, 0);
        PC+=3;
        updateElement(regX);
        break;
    case 0xFF:
        cycleCount = 5;
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


    int lineNum = instructionList.getObjectByAddress(PC).lineNumber;
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
    return cycleCount;
}


