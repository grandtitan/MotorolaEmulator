#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringBuilder>
#include <QScrollBar>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <QTextBlock>
#include <QString>
#include <QTimer>
#include <cmath>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QTableWidget>
#include <QInputDialog>
#include "InstructionList.h"
#include <QPointer>
#include "instructioninfodialog.h"
#include <QtConcurrent/QtConcurrentRun>
#include <chrono>
#include <QThreadPool>
const QColor memoryCellDefaultColor =  QColor(230,230,255);
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    instructionList.clear();
    externalDisplay = new ExternalDisplay(this);
    plainTextDisplay = externalDisplay->findChild<QPlainTextEdit*> ("plainTextDisplay");
    connect(externalDisplay, &QDialog::finished, [=]() {
        ui->comboBoxDisplayStatus->setCurrentIndex(0);
    });
    QWidget::setWindowTitle("Motorola M68XX Emulator-" + softwareVersion);
    const int memWidth = 28;
    const int memHeight = 20;
    const int fontSize = 9;
    const QString font = "Lucida Console";
    ui->tableWidgetMemory->horizontalHeader()->setMinimumSectionSize(memWidth);
    ui->tableWidgetMemory->verticalHeader()->setMinimumSectionSize(memHeight);
    ui->tableWidgetMemory->horizontalHeader()->setMaximumSectionSize(memWidth);
    ui->tableWidgetMemory->verticalHeader()->setMaximumSectionSize(memHeight);
    ui->tableWidgetMemory->horizontalHeader()->setDefaultSectionSize(memWidth);
    ui->tableWidgetMemory->verticalHeader()->setDefaultSectionSize(memHeight);
    for (int row = 0; row <= 0xFFF; ++row) {
        QString address = QString("%1").arg(row * 16, 4, 16, QChar('0')).toUpper();
        ui->tableWidgetMemory->insertRow(row);

        QTableWidgetItem *headerItem = new QTableWidgetItem(address);
        headerItem->setTextAlignment(Qt::AlignCenter);
        headerItem->setBackground(QBrush(QColor(210, 210, 255)));
        QFont headerFont(font, fontSize, QFont::Bold);
        headerItem->setFont(headerFont);
        ui->tableWidgetMemory->setVerticalHeaderItem(row, headerItem);

        for (int col = 0; col < ui->tableWidgetMemory->columnCount(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem("00");
            item->setBackground(QBrush(memoryCellDefaultColor));
            item->setTextAlignment(Qt::AlignCenter);
            QFont cellFont(font, fontSize, QFont::Bold);
            item->setFont(cellFont);
            ui->tableWidgetMemory->setItem(row, col, item);
        }
    }
    for (int col = 0; col < ui->tableWidgetMemory->columnCount(); ++col) {
        QTableWidgetItem *columnHeaderItem = ui->tableWidgetMemory->horizontalHeaderItem(col);
        columnHeaderItem->setBackground(QBrush(QColor(210, 210, 255)));
        columnHeaderItem->setTextAlignment(Qt::AlignCenter);
        QFont columnHeaderFont(font, fontSize, QFont::Bold);
        columnHeaderItem->setFont(columnHeaderFont);
    }
    ui->tableWidgetMemory->setTextElideMode(Qt::ElideNone);
    updateMemoryTab();
    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
    for (int col = 0; col < ui->treeWidget->columnCount(); ++col)
    {
        if (col == 0)
        {
            ui->treeWidget->setColumnWidth(col, 90);
        }
        else if (col == 1)
        {
            ui->treeWidget->setColumnWidth(col, 200);
        }
        else if (col == 8)
        {
            ui->treeWidget->setColumnWidth(col, 50);
        }
        else if (col == 9 || col == 10)
        {
            ui->treeWidget->setColumnWidth(col, 60);
        }
        else
        {
            ui->treeWidget->setColumnWidth(col, 30);
        }
    }

    for (int row = 0; row < ui->treeWidget->topLevelItemCount(); ++row)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(row);

        if (row % 2 == 0)
        {
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col)
            {
                item->setBackground(col, QBrush(QColor(230, 230, 230)));
            }
        }
        else
        {
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col)
            {
                item->setBackground(col, QBrush(QColor(240, 240, 240)));
            }
        }
    }

    for (int row = 0; row < ui->treeWidget->topLevelItemCount(); ++row)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(row);
        if (row == 1 || row == 6 || row == 12 || row == 31 || row == 65 || row == 71 || row == 72 || row == 81 || row == 84 || row == 106)
        {
            for (int col = 0; col < ui->treeWidget->columnCount(); ++col)
            {
                item->setForeground(col, QBrush(Qt::red));
            }
        }
        else if (row == 62)
        {
            item->setForeground(4, QBrush(Qt::red));
        }

        if (row == 9 || row == 10 || row == 11 || row == 12 || row == 16 || row == 17)
        {
            item->child(0)->setForeground(0, QBrush(Qt::red));
            item->child(0)->setForeground(1, QBrush(Qt::red));
        }
    }
    for (int row = 0; row < ui->treeWidget->topLevelItemCount(); ++row)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(row);
        QString textInColumn0 = item->text(0);
        if (textInColumn0.startsWith("ZZZ."))
        {
            item->setText(0, textInColumn0.mid(3));
        }
    }

    ui->labelRunningIndicatior->setVisible(false);
    ui->labelRunningIndicatior->setText("Operation/second: " + QString::number(std::pow(2, ui->comboBoxSpeedSelector->currentIndex())));

    ui->labelRunningCycleNum->setVisible(false);
    ui->labelRunningCycleNum->setText("Instruction cycle: ");
    connect(ui->plainTextCode->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleCodeVerticalScrollBarValueChanged);
    connect(ui->plainTextLines->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleLinesScroll);

    connect(ui->plainTextDisplay->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleDisplayScrollVertical);
    connect(ui->plainTextDisplay->horizontalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::handleDisplayScrollHorizontal);

    connect(this, &MainWindow::resized, this, &MainWindow::handleMainWindowSizeChanged);

    ui->groupBoxSimpleMemory->setVisible(false);
    ui->groupBoxSimpleMemory->setEnabled(false);

    ui->plainTextDisplay->installEventFilter(this);
    ui->plainTextLines->installEventFilter(this);
    plainTextDisplay->installEventFilter(this);

    ui->buttonSwitchWrite->setVisible(false);
    ui->labelWritingMode->setVisible(false);
    ui->buttonSwitchWrite->setEnabled(false);
    ui->labelWritingMode->setEnabled(false);

    ui->labelAt->setVisible(false);
    ui->spinBoxBreakAt->setVisible(false);

    uiUpdateTimer = new QTimer(this);
    connect(uiUpdateTimer, &QTimer::timeout, this, &MainWindow::updateIfReady);

    ui->plainTextCode->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plainTextCode, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(const QPoint &)));

    ui->plainTextCode->moveCursor(QTextCursor::End);

    QFontMetrics metrics(ui->plainTextCode->font());
    ui->plainTextCode->setTabStopDistance(metrics.horizontalAdvance(' ') *ui->spinBoxTabWidth->value());

    on_comboBoxSpeedSelector_activated(ui->comboBoxSpeedSelector->currentIndex());

    for (int row = 0; row < ui->tableWidgetMemory->rowCount(); row++) {
        for (int col = 0; col < ui->tableWidgetMemory->columnCount(); col++) {
            QTableWidgetItem* item = ui->tableWidgetMemory->item(row, col);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
        }
    }
    on_comboBoxDisplayStatus_currentIndexChanged(0);

    for (int i = 0; i < 20; ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem(QString("%1").arg(currentSMScroll + i, 4, 16, QChar('0')).toUpper());
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() &~Qt::ItemIsEditable);
        ui->tableWidgetSM->setItem(i, 0, item);
        item = new QTableWidgetItem(QString("%1").arg(static_cast<quint8> (Memory[currentSMScroll + i]), 2, 16, QChar('0').toUpper()));
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() &~Qt::ItemIsEditable);
        ui->tableWidgetSM->setItem(i, 1, item);
    }
}
MainWindow::~MainWindow()
{
    uiUpdateTimer->stop();
    running = false;
    futureWatcher.cancel();
    futureWatcher.waitForFinished();
    QCoreApplication::processEvents();
    delete ui;
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QMenu *menu = ui->plainTextCode->createStandardContextMenu();
    menu->addSeparator();

    QVariant cursorPosVariant(pos);
    QAction *action = menu->addAction(tr("Mnemonic info"));
    action->setData(cursorPosVariant);

    connect(action, SIGNAL(triggered()), this, SLOT(showMnemonicInfo()));

    menu->exec(ui->plainTextCode->mapToGlobal(pos));

    menu->deleteLater();
}
void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    int version = 6800;
    if (item->foreground(0) == Qt::red)
    {
        version = 6803;
    }

    InstructionInfoDialog dialog(*item, version, this);
    dialog.exec();
}
void MainWindow::showMnemonicInfo()
{
    QAction *action = qobject_cast<QAction*> (sender());
    if (action)
    {
        QVariant cursorPosVariant = action->data();
        QPoint cursorPos = cursorPosVariant.toPoint();
        QString plainText = ui->plainTextCode->toPlainText();
        int index = ui->plainTextCode->cursorForPosition(cursorPos).position();
        int rightIndex = index;
        while (rightIndex < plainText.length() && !plainText[rightIndex].isSpace())
        {
            rightIndex++;
        }

        int leftIndex = index;
        while (leftIndex >= 0 && !plainText[leftIndex].isSpace())
        {
            leftIndex--;
        }

        QString selectedWord = (plainText.mid(leftIndex + 1, rightIndex - leftIndex - 1)).toUpper();

        int version = -1;	//-1 unkown 6800 6803
        if (specialInstructions.contains(selectedWord))
        {
            version = -1;
        }
        else if (allInstructionsM6800.contains(selectedWord))
        {
            version = 6800;
        }
        else if (allInstructionsM6803.contains(selectedWord))
        {
            version = 6803;
        }
        else
        {
            return;
        }

        showInstructionInfoWindow(selectedWord, version);
    }
}
void MainWindow::showInstructionInfoWindow(QString instruction, int version)
{
    QTreeWidgetItem item_;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        if (item && item->text(0) == instruction)
        {
            item_ = *item;
            break;
        }
    }

    InstructionInfoDialog dialog(item_, version, this);
    dialog.exec();
}

inline QString getDisplayText(const uint8_t* memory){
    QString text;
    for (int i = 0; i < 20*54; ++i) {
        int val = memory[i+0xFB00];
        if (val <= 32 || val >= 127) {
            text.append(" ");
        } else {
            text.append(QChar(val));
        }
        if ((i + 1) % 54 == 0) text.append("\n");
    }
    return text;
}
inline bool bit(int variable, int bitNum)
{
    return (variable &(1 << bitNum)) != 0;
}
QString convertToQString(int number, int width)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(width) << number;
    std::string str = stream.str();
    return QString::fromStdString(str);
}

void MainWindow::setCompileStatus(bool isCompiled)
{
    if (isCompiled)
    {
        ui->buttonCompile->setStyleSheet(compiledButton);
        updateLinesBox();
        updateMemoryTab();
        compiled = 1;
    }
    else
    {
        compiled = 0;
        ui->buttonCompile->setStyleSheet(uncompiledButton);
        PrintConsole("", 2);
        instructionList.clear();
        clearSelectionLines();
        updateLinesBox();
        updateSelectionsRunTime(PC);
        ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
    }
}

void MainWindow::updateLinesBox()
{
    if (ui->checkBoxAdvancedInfo->isChecked())
    {
        if (instructionList.isEmpty())
        {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i<code.count("\n") + 1; i++)
            {
                text = text + convertToQString(i, 5) + "\n";
            }

            ui->plainTextLines->setPlainText(text);
        }
        else
        {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i<code.count("\n") + 1; i++)
            {
                const InstructionList::Instruction &instr = instructionList.getObjectByLine(i);
                if (instr.address == -1)
                {
                    text = text % convertToQString(i, 5) % ":----\n";
                }
                else
                {
                    text = text % convertToQString(i, 5) %
                        ":" % QString("%1").arg(instr.address, 4, 16, QChar('0')) %
                        ":" % QString("%1").arg(instr.byte1, 2, 16, QChar('0')) %
                        ":" % QString("%1").arg(instr.byte2, 2, 16, QChar('0')) %
                        ":" % QString("%1").arg(instr.byte3, 2, 16, QChar('0')) %
                        "\n";
                }
            }

            ui->plainTextLines->setPlainText(text);
        }
    }
    else
    {
        if (instructionList.isEmpty())
        {
            QString code = ui->plainTextCode->toPlainText();
            QString text;
            for (int i = 0; i<code.count("\n") + 1; i++)
            {
                text = text % convertToQString(i, 5) % "\n";
            }

            ui->plainTextLines->setPlainText(text);
        }
        else
        {
            QString code = ui->plainTextCode->toPlainText();
            QString text;

            for (int i = 0; i<code.count("\n") + 1; i++)
            {
                if (instructionList.getObjectByLine(i).address == -1)
                {
                    text = text % convertToQString(i, 5) % ":----\n";
                }
                else
                {
                    text = text % convertToQString(i, 5) % ":" % QString("%1").arg(instructionList.getObjectByLine(i).address, 4, 16, QChar('0')) % "\n";
                }
            }

            ui->plainTextLines->setPlainText(text);
        }
    }

    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
}
void MainWindow::updateMemoryTab()
{
    if (simpleMemory)
    {
        for (int i = 0; i < 20; ++i)
        {
            ui->tableWidgetSM->item(i, 0)->setText(QString("%1").arg(currentSMScroll + i, 4, 16, QChar('0')).toUpper());
            ui->tableWidgetSM->item(i, 1)->setText(QString("%1").arg(static_cast<quint8> (Memory[currentSMScroll + i]), 2, 16, QChar('0').toUpper()));
            updateSelectionsRunTime(PC);
        }
    }
    else
    {
        for (int row = 0; row < ui->tableWidgetMemory->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidgetMemory->columnCount(); ++col) {
                int address = row * 16 + col;
                int value = Memory[address];

                if (hexReg) {
                    ui->tableWidgetMemory->item(row, col)->setText(QString("%1").arg(value, 2, 16, QChar('0')).toUpper());
                } else {
                    ui->tableWidgetMemory->item(row, col)->setText(QString("%1").arg(value));
                }
            }
        }

    }
}
void MainWindow::updateFlags(FlagToUpdate flag, bool value)
{
    switch (flag)
    {
        case HalfCarry:
            flags = (flags &~(1 << 5)) | (value << 5);
            break;
        case InterruptMask:
            flags = (flags &~(1 << 4)) | (value << 4);
            break;
        case Negative:
            flags = (flags &~(1 << 3)) | (value << 3);
            break;
        case Zero:
            flags = (flags &~(1 << 2)) | (value << 2);
            break;
        case Overflow:
            flags = (flags &~(1 << 1)) | (value << 1);
            break;
        case Carry:
            flags = (flags &~(1)) | (value);
            break;
        default:
            throw;
    }
}
void MainWindow::PrintConsole(const QString &text, int type)
{
    QString consoleText;
    if (type == -1)
    {
        consoleText = "DEBUG: " + ("Ln:" + QString::number(currentCompilerLine)) + " " + text;
    }
    else if (type == 0)
    {
        consoleText = "ERROR: " + text;
    }
    else if (type == 1)
    {
        consoleText = "WARN: " + text;
    }
    else
    {
        consoleText = text;
    }

    ui->plainTextConsole->appendPlainText(consoleText);
}
void MainWindow::Err(const QString &text)
{
    PrintConsole("Ln:" + QString::number(currentCompilerLine) + " " + text, 0);
}

void MainWindow::updateSelectionsLines(int line)
{
    if (line != -1)
    {
        int address = instructionList.getObjectByLine(line).address;

        if(!lineSelectionLines.contains(line)){
            lineSelectionLines.append(line);
            if (address >= 0)
            {
                lineSelectionAddresses.append(address);
            }

        }else{
            if(address >= 0){
                lineSelectionAddresses.removeOne(address);
                ui->tableWidgetMemory->item(address / 16, address % 16)->setBackground(QBrush(memoryCellDefaultColor));
                if(address == previousRunTimeSelectionAddress){
                    ui->tableWidgetMemory->item(previousRunTimeSelectionAddress / 16, previousRunTimeSelectionAddress % 16)->setBackground(QBrush(Qt::yellow));
                }
            }
            lineSelectionLines.removeOne(line);
        }
    }
    linesExtraSelectionsLines.clear();
    codeExtraSelectionsLines.clear();
    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::green);
    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    foreach (int line, lineSelectionLines) {
        QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(line);
        QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(line);
        QTextCursor codeCursor(codeBlock);
        codeCursor.select(QTextCursor::LineUnderCursor);
        QTextCursor linesCursor(linesBlock);
        linesCursor.select(QTextCursor::LineUnderCursor);
        lineSelection.cursor = linesCursor;
        linesExtraSelectionsLines.append(lineSelection);
        lineSelection.cursor = codeCursor;
        codeExtraSelectionsLines.append(lineSelection);
    }
    foreach(int address, lineSelectionAddresses)
    {
        ui->tableWidgetMemory->item(address / 16, address % 16)->setBackground(QBrush(Qt::green));
    }
   ui->plainTextLines->setExtraSelections(linesExtraSelectionsRunTime + linesExtraSelectionsLines);
   ui->plainTextCode->setExtraSelections(codeExtraSelectionsRunTime + codeExtraSelectionsLines);
}
void MainWindow::clearSelectionLines(){
    linesExtraSelectionsLines.clear();
    codeExtraSelectionsLines.clear();
    ui->tableWidgetMemory->item(previousRunTimeSelectionAddress / 16, previousRunTimeSelectionAddress % 16)->setBackground(QBrush(memoryCellDefaultColor));
    for (int i = 0; i < lineSelectionAddresses.length(); ++i) {
        ui->tableWidgetMemory->item(lineSelectionAddresses[i] / 16, lineSelectionAddresses[i] % 16)->setBackground(QBrush(memoryCellDefaultColor));
    }
    lineSelectionLines.clear();
    lineSelectionAddresses.clear();
    ui->plainTextLines->setExtraSelections(linesExtraSelectionsRunTime + linesExtraSelectionsLines);
    ui->plainTextCode->setExtraSelections(codeExtraSelectionsRunTime + codeExtraSelectionsLines);
}
void MainWindow::updateSelectionsRunTime(int address)
{
    linesExtraSelectionsRunTime.clear();
    codeExtraSelectionsRunTime.clear();

    QTextCharFormat lineFormat;
    lineFormat.setBackground(Qt::yellow);
    QTextEdit::ExtraSelection lineSelection;
    lineSelection.format = lineFormat;
    int lineNum = instructionList.getObjectByAddress(address).lineNumber;
    if (lineNum >= 0)
    {
        QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(lineNum);
        QTextBlock linesBlock = ui->plainTextLines->document()->findBlockByLineNumber(lineNum);
        QTextCursor codeCursor(codeBlock);
        codeCursor.select(QTextCursor::LineUnderCursor);
        QTextCursor linesCursor(linesBlock);
        linesCursor.select(QTextCursor::LineUnderCursor);
        lineSelection.cursor = linesCursor;
        linesExtraSelectionsRunTime.append(lineSelection);

        lineSelection.cursor = codeCursor;
        codeExtraSelectionsRunTime.append(lineSelection);
    }
    if(!simpleMemory){
        ui->tableWidgetMemory->item(address / 16, address % 16)->setBackground(QBrush(Qt::yellow));
        if(lineSelectionAddresses.contains(previousRunTimeSelectionAddress)){
            ui->tableWidgetMemory->item(previousRunTimeSelectionAddress / 16, previousRunTimeSelectionAddress % 16)->setBackground(QBrush(Qt::green));
        }else if (address != previousRunTimeSelectionAddress){
            ui->tableWidgetMemory->item(previousRunTimeSelectionAddress / 16, previousRunTimeSelectionAddress % 16)->setBackground(QBrush(memoryCellDefaultColor));
        }
        previousRunTimeSelectionAddress = address;
    } else{
        for (int i = 0; i < 20; ++i) {
            ui->tableWidgetSM->item(i, 1)->setBackground(QBrush(QColor(204,204,204)));
        }
        int row = address - currentSMScroll;
        if(row < 20 && row >= 0){
            ui->tableWidgetSM->item(row, 1)->setBackground(QBrush(Qt::yellow));
        }
    }
    ui->plainTextLines->setExtraSelections(linesExtraSelectionsRunTime + linesExtraSelectionsLines);
    ui->plainTextCode->setExtraSelections(codeExtraSelectionsRunTime + codeExtraSelectionsLines);
}
void MainWindow::updateSelectionCompileError(int charNum)
{
    QTextBlock codeBlock = ui->plainTextCode->document()->findBlockByLineNumber(currentCompilerLine);
    QTextCursor codeCursor(codeBlock);
    codeCursor.select(QTextCursor::LineUnderCursor);
    QTextCursor charCursor(codeBlock);
    charCursor.setPosition(codeBlock.position() + charNum);
    charCursor.setPosition(codeBlock.position() + charNum + 1, QTextCursor::KeepAnchor);

    QTextCharFormat lineFormat;
    QTextEdit::ExtraSelection lineSelection;

    lineFormat.setBackground(Qt::darkYellow);
    lineSelection.format = lineFormat;
    lineSelection.cursor = codeCursor;
    codeExtraSelectionsLines.append(lineSelection);

    lineFormat.setBackground(Qt::darkGreen);
    lineSelection.format = lineFormat;
    lineSelection.cursor = charCursor;
    codeExtraSelectionsLines.append(lineSelection);

    ui->plainTextLines->setExtraSelections(linesExtraSelectionsRunTime + linesExtraSelectionsLines);
    ui->plainTextCode->setExtraSelections(codeExtraSelectionsRunTime + codeExtraSelectionsLines);

    if (currentCompilerLine > ui->plainTextCode->verticalScrollBar()->value() + autoScrollUpLimit)
    {
        ui->plainTextLines->verticalScrollBar()->setValue(currentCompilerLine - autoScrollUpLimit);
        ui->plainTextCode->verticalScrollBar()->setValue(currentCompilerLine - autoScrollUpLimit);
    }
    else if (currentCompilerLine < ui->plainTextCode->verticalScrollBar()->value() + autoScrollDownLimit)
    {
        ui->plainTextLines->verticalScrollBar()->setValue(currentCompilerLine - autoScrollDownLimit);
        ui->plainTextCode->verticalScrollBar()->setValue(currentCompilerLine - autoScrollDownLimit);
    }
}

void MainWindow::handleCodeVerticalScrollBarValueChanged(int value)
{
    ui->plainTextLines->verticalScrollBar()->setValue(value);
}
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    emit resized(this->size());
}
void MainWindow::handleMainWindowSizeChanged(const QSize &newSize)
{
    int buttonYoffset = 30;
    int buttonY = newSize.height() - buttonYoffset;
    if (newSize.width() >= 1785)
    {
        ui->frameDisplay->setGeometry(newSize.width() - 875, 10, 498, 350);
        if (ui->comboBoxDisplayStatus->count() == 2) {
            ui->comboBoxDisplayStatus->insertItem(1, "Main Window");
        }


        ui->plainTextCode->setGeometry(ui->checkBoxAdvancedInfo->isChecked() ? 190 : 110, ui->plainTextCode->y(), ui->checkBoxAdvancedInfo->isChecked() ? (newSize.width() - 1589) : (newSize.width() - 1509), newSize.height() - buttonYoffset - 17);
        ui->tableWidgetMemory->setGeometry(newSize.width() - 1390, ui->tableWidgetMemory->y(), ui->tableWidgetMemory->width(), newSize.height() - buttonYoffset - 17);
        ui->groupBoxSimpleMemory->setGeometry(newSize.width() - 1390, ui->groupBoxSimpleMemory->y(), ui->groupBoxSimpleMemory->width(), ui->groupBoxSimpleMemory->height());
        if (newSize.height() >= 800)
        {
            ui->tabWidget->setGeometry(newSize.width() - 875, 370, 868, newSize.height() - 359 - buttonYoffset - 17);
        }
        else
        {
            ui->tabWidget->setGeometry(newSize.width() - 370, 300, 363, newSize.height() - 289 - buttonYoffset - 17);
        }
    }
    else
    {
        if(ui->comboBoxDisplayStatus->currentIndex() == 1){
            ui->comboBoxDisplayStatus->setCurrentIndex(0);
        }
        if(ui->comboBoxDisplayStatus->count() == 3){
            ui->comboBoxDisplayStatus->model()->removeRow(1);
        }
        ui->tabWidget->setGeometry(910, 300, newSize.width() - 917, newSize.height() - 289 - buttonYoffset - 17);
        ui->plainTextCode->setGeometry(ui->checkBoxAdvancedInfo->isChecked() ? 190 : 110, ui->plainTextCode->y(), ui->checkBoxAdvancedInfo->isChecked() ? 201 : 281, newSize.height() - buttonYoffset - 17);
        ui->tableWidgetMemory->setGeometry(400, ui->tableWidgetMemory->y(), ui->tableWidgetMemory->width(), newSize.height() - buttonYoffset - 17);
        ui->groupBoxSimpleMemory->setGeometry(400, ui->groupBoxSimpleMemory->y(), ui->groupBoxSimpleMemory->width(), ui->groupBoxSimpleMemory->height());
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
void MainWindow::on_plainTextCode_textChanged()
{
    if (ui->plainTextCode->toPlainText().count('\n') > 65535)
    {
        QString text = ui->plainTextCode->toPlainText();
        QStringList lines = text.split('\n', Qt::SkipEmptyParts);
        lines = lines.mid(0, 0x10000);
        text = lines.join('\n');
        ui->plainTextCode->setPlainText(text);
    }
    updateLinesBox();
    if (!writeToMemory)
    {
        if(compiled) setCompileStatus(false);
    }
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->plainTextLines)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
            if (mouseEvent)
            {
                if (compiled)
                {
                    if (mouseEvent->button() == Qt::LeftButton)
                    {
                        QPoint mousePos = mouseEvent->pos();
                        QTextCursor cursor = ui->plainTextLines->cursorForPosition(mousePos);
                        if (!cursor.atEnd())
                        {
                            int lineNumber = cursor.blockNumber();
                            int totalLines = ui->plainTextLines->document()->blockCount();
                            if (lineNumber >= 0 && lineNumber < totalLines)
                            {
                                updateSelectionsLines(lineNumber);
                            }
                        }
                    }
                    else if (mouseEvent->button() == Qt::RightButton)
                    {
                        clearSelectionLines();
                    }
                }
            }
        }
        else if (event->type() == QEvent::Wheel || event->type() == QEvent::Scroll || event->type() == QEvent::User || event->type() == QEvent::KeyPress)
        {
            return true;
        }
    }else if(displayStatusIndex == 1){
        if (obj == ui->plainTextDisplay)
        {
            if (event->type() == QEvent::KeyPress)
            {
                QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
                if (keyEvent->key() <= Qt::Key_AsciiTilde)
                {
                    char asciiValue = static_cast<uint8_t> (keyEvent->key());
                    Memory[0xFFF0] = asciiValue;
                    lastInput = asciiValue;
                }

                return true;
            }
            else if (event->type() == QMouseEvent::MouseButtonPress || event->type() == QMouseEvent::MouseButtonDblClick)
            {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    Memory[0xFFF1] = 1;
                }
                else if (mouseEvent->button() == Qt::RightButton)
                {
                    Memory[0xFFF1] = 2;
                }
                else if (mouseEvent->button() == Qt::MiddleButton)
                {
                    Memory[0xFFF1] = 3;
                }

                return true;
            }
            else if (event->type() == QMouseEvent::MouseButtonRelease)
            {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    Memory[0xFFF1] = 4;
                }
                else if (mouseEvent->button() == Qt::RightButton)
                {
                    Memory[0xFFF1] = 5;
                }
                else if (mouseEvent->button() == Qt::MiddleButton)
                {
                    Memory[0xFFF1] = 6;
                }
                return true;
            }
        }
    } else if(displayStatusIndex == 2){
        if (obj == plainTextDisplay)
        {
            if (event->type() == QEvent::KeyPress)
            {
                QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
                if (keyEvent->key() <= Qt::Key_AsciiTilde)
                {
                    char asciiValue = static_cast<uint8_t> (keyEvent->key());
                    Memory[0xFFF0] = asciiValue;
                    lastInput = asciiValue;
                }

                return true;
            }
            else if (event->type() == QMouseEvent::MouseButtonPress || event->type() == QMouseEvent::MouseButtonDblClick)
            {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    Memory[0xFFF1] = 1;
                }
                else if (mouseEvent->button() == Qt::RightButton)
                {
                    Memory[0xFFF1] = 2;
                }
                else if (mouseEvent->button() == Qt::MiddleButton)
                {
                    Memory[0xFFF1] = 3;
                }

                return true;
            }
            else if (event->type() == QMouseEvent::MouseButtonRelease)
            {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    Memory[0xFFF1] = 4;
                }
                else if (mouseEvent->button() == Qt::RightButton)
                {
                    Memory[0xFFF1] = 5;
                }
                else if (mouseEvent->button() == Qt::MiddleButton)
                {
                    Memory[0xFFF1] = 6;
                }
                return true;
            }
        }
    }


    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::handleLinesScroll()
{
    ui->plainTextLines->verticalScrollBar()->setValue(ui->plainTextCode->verticalScrollBar()->value());
}
void MainWindow::handleDisplayScrollVertical()
{
    ui->plainTextDisplay->verticalScrollBar()->setValue(0);
}
void MainWindow::handleDisplayScrollHorizontal()
{
    ui->plainTextDisplay->horizontalScrollBar()->setValue(0);
}

void MainWindow::resetEmulator(bool failedCompile)
{
    if (running)
    {
        stopExecution();
    }
    instructionCycleCount = 0;
    currentCycleNum = 1;
    aReg = 0;
    bReg = 0;
    xRegister = 0;
    SP = 0x00FF;
    flags = 0xD0;
    lastInput = -1;
    globalUpdateInfo.whatToUpdate = 0;
    std::memcpy(Memory, backupMemory, sizeof(backupMemory));
    PC = (Memory[interruptLocations - 1] << 8) + Memory[interruptLocations];
    updateUi();
    ui->plainTextDisplay->setPlainText("                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                       ,");
    plainTextDisplay->setPlainText("                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                      \n                                                       ,");
    if (!failedCompile)
    {
        updateSelectionsLines();
    }
}

void MainWindow::updateIfReady(){
    if(globalUpdateInfo.whatToUpdate != 0){
        updateCurUi();
        globalUpdateInfo.whatToUpdate = 0;
    }
}
void MainWindow::updateUi(){
        ui->lineEditHValue->setText(QString::number(bit(flags, 5)));
        ui->lineEditIValue->setText(QString::number(bit(flags, 4)));
        ui->lineEditNValue->setText(QString::number(bit(flags, 3)));
        ui->lineEditZValue->setText(QString::number(bit(flags, 2)));
        ui->lineEditVValue->setText(QString::number(bit(flags, 1)));
        ui->lineEditCValue->setText(QString::number(bit(flags, 0)));
        if (hexReg)
        {
            ui->lineEditPCValue->setText(QString("%1").arg(PC, 4, 16, QLatin1Char('0')).toUpper());
            ui->lineEditSPValue->setText(QString("%1").arg(SP, 4, 16, QLatin1Char('0')).toUpper());
            ui->lineEditAValue->setText(QString("%1").arg(aReg, 2, 16, QLatin1Char('0')).toUpper());
            ui->lineEditBValue->setText(QString("%1").arg(bReg, 2, 16, QLatin1Char('0')).toUpper());
            ui->lineEditXValue->setText(QString("%1").arg(xRegister, 4, 16, QLatin1Char('0')).toUpper());
        }
        else
        {
            ui->lineEditPCValue->setText(QString::number(PC));
            ui->lineEditSPValue->setText(QString::number(SP));
            ui->lineEditAValue->setText(QString::number(aReg));
            ui->lineEditBValue->setText(QString::number(bReg));
            ui->lineEditXValue->setText(QString::number(xRegister));
        }
        if (useCyclesPerSecond) { ui->labelRunningCycleNum->setText("Instruction cycle: " + QString::number(currentCycleNum));}
        int lineNum = instructionList.getObjectByAddress(PC).lineNumber;
        if (lineNum >= 0){
            if (lineNum > previousScrollCode + autoScrollUpLimit)
            {
                previousScrollCode = lineNum - autoScrollUpLimit;
                ui->plainTextLines->verticalScrollBar()->setValue(previousScrollCode);
                ui->plainTextCode->verticalScrollBar()->setValue(previousScrollCode);
            }
            else if (lineNum < previousScrollCode + autoScrollDownLimit)
            {
                previousScrollCode = lineNum - autoScrollDownLimit;
                ui->plainTextLines->verticalScrollBar()->setValue(previousScrollCode);
                ui->plainTextCode->verticalScrollBar()->setValue(previousScrollCode);
            }
        }
        if(displayStatusIndex == 1){
            ui->plainTextDisplay->setPlainText(getDisplayText(Memory));
        }else if (displayStatusIndex == 2){
            plainTextDisplay->setPlainText(getDisplayText(Memory));
        }
        updateMemoryTab();
        updateSelectionsRunTime(PC);

}
void MainWindow::updateCurUi(){

    if(globalUpdateInfo.whatToUpdate == 1){
        ui->lineEditHValue->setText(QString::number(bit(globalUpdateInfo.curFlags, 5)));
        ui->lineEditIValue->setText(QString::number(bit(globalUpdateInfo.curFlags, 4)));
        ui->lineEditNValue->setText(QString::number(bit(globalUpdateInfo.curFlags, 3)));
        ui->lineEditZValue->setText(QString::number(bit(globalUpdateInfo.curFlags, 2)));
        ui->lineEditVValue->setText(QString::number(bit(globalUpdateInfo.curFlags, 1)));
        ui->lineEditCValue->setText(QString::number(bit(globalUpdateInfo.curFlags, 0)));
        if (hexReg){
            ui->lineEditPCValue->setText(QString("%1").arg(globalUpdateInfo.curPC, 4, 16, QLatin1Char('0')).toUpper());
            ui->lineEditSPValue->setText(QString("%1").arg(globalUpdateInfo.curSP, 4, 16, QLatin1Char('0')).toUpper());
            ui->lineEditAValue->setText(QString("%1").arg(globalUpdateInfo.curA, 2, 16, QLatin1Char('0')).toUpper());
            ui->lineEditBValue->setText(QString("%1").arg(globalUpdateInfo.curB, 2, 16, QLatin1Char('0')).toUpper());
            ui->lineEditXValue->setText(QString("%1").arg(globalUpdateInfo.curX, 4, 16, QLatin1Char('0')).toUpper());
        }
        else{
            ui->lineEditPCValue->setText(QString::number(globalUpdateInfo.curPC));
            ui->lineEditSPValue->setText(QString::number(globalUpdateInfo.curSP));
            ui->lineEditAValue->setText(QString::number(globalUpdateInfo.curA));
            ui->lineEditBValue->setText(QString::number(globalUpdateInfo.curB));
            ui->lineEditXValue->setText(QString::number(globalUpdateInfo.curX));
        }
        if (useCyclesPerSecond) { ui->labelRunningCycleNum->setText("Instruction cycle: " + QString::number(globalUpdateInfo.curCycle));}
        int lineNum = instructionList.getObjectByAddress(globalUpdateInfo.curPC).lineNumber;
        if (lineNum >= 0){
            if (lineNum > previousScrollCode + autoScrollUpLimit){
                previousScrollCode = lineNum - autoScrollUpLimit;
                ui->plainTextLines->verticalScrollBar()->setValue(previousScrollCode);
                ui->plainTextCode->verticalScrollBar()->setValue(previousScrollCode);
            }
            else if (lineNum < previousScrollCode + autoScrollDownLimit){
                previousScrollCode = lineNum - autoScrollDownLimit;
                ui->plainTextLines->verticalScrollBar()->setValue(previousScrollCode);
                ui->plainTextCode->verticalScrollBar()->setValue(previousScrollCode);
            }
        }
        if(!simpleMemory){
            int firstVisibleRow = ui->tableWidgetMemory->rowAt(0);
            int lastVisibleRow = std::ceil(ui->tableWidgetMemory->rowAt(ui->tableWidgetMemory->viewport()->height() - 1));
            for (int row = firstVisibleRow; row <= lastVisibleRow; ++row) {
                for (int col = 0; col < ui->tableWidgetMemory->columnCount(); ++col) {
                    if (hexReg) {
                        ui->tableWidgetMemory->item(row, col)->setText(QString("%1").arg(globalUpdateInfo.curMemory[row*16+col], 2, 16, QChar('0')).toUpper());
                    } else {
                        ui->tableWidgetMemory->item(row, col)->setText(QString("%1").arg(globalUpdateInfo.curMemory[row*16+col]));
                    }
                }
            }

        } else{
            for (int i = 0; i < 20; ++i)
            {
                ui->tableWidgetSM->item(i, 0)->setText(QString("%1").arg(currentSMScroll + i, 4, 16, QChar('0')).toUpper());
                ui->tableWidgetSM->item(i, 1)->setText(QString("%1").arg(static_cast<quint8> (Memory[currentSMScroll + i]), 2, 16, QChar('0').toUpper()));
                updateSelectionsRunTime(PC);
            }
        }
        if(displayStatusIndex == 1){
            ui->plainTextDisplay->setPlainText(getDisplayText(globalUpdateInfo.curMemory));
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
                    oldCursorX = x;
                    oldCursorY = y;
                } else{
                    x = oldCursorX;
                    y = oldCursorY;
                }
                Memory[0xFFF2] = x;
                Memory[0xFFF3] = y;
            }
        }else if (displayStatusIndex == 2){
            plainTextDisplay->setPlainText(getDisplayText(globalUpdateInfo.curMemory));
            if(plainTextDisplay->hasFocus()){
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
                    oldCursorX = x;
                    oldCursorY = y;
                } else{
                    x = oldCursorX;
                    y = oldCursorY;
                }
                Memory[0xFFF2] = x;
                Memory[0xFFF3] = y;
            }
        }
        updateSelectionsRunTime(globalUpdateInfo.curPC);
    } else if(globalUpdateInfo.whatToUpdate == 2){
        ui->labelRunningCycleNum->setText("Instruction cycle: " + QString::number(globalUpdateInfo.curCycle));
    }
    globalUpdateInfo.whatToUpdate = 0;
}

//0-noupdate 1-fulluiupdate 2-currentcycle
void MainWindow::setUiUpdateData(int whatToUpdate, const uint8_t* curMemory, int curCycle, uint8_t curFlags , uint16_t curPC, uint16_t curSP, uint8_t curA, uint8_t curB, uint16_t curX){
    memcpy(globalUpdateInfo.curMemory, curMemory, 0x10000);
    globalUpdateInfo.curCycle = curCycle;
    globalUpdateInfo.curFlags = curFlags;
    globalUpdateInfo.curPC = curPC;
    globalUpdateInfo.curSP = curSP;
    globalUpdateInfo.curA = curA;
    globalUpdateInfo.curB = curB;
    globalUpdateInfo.curX = curX;
    globalUpdateInfo.whatToUpdate = whatToUpdate;
}
void MainWindow::setUiUpdateData(int whatToUpdate, int curCycle){
    globalUpdateInfo.curCycle = curCycle;
    globalUpdateInfo.whatToUpdate = whatToUpdate;
}


void MainWindow::startExecution() {

    running = true;
    uiUpdateTimer->start(std::ceil(1000/uiUpdateSpeed));
    ui->labelRunningIndicatior->setVisible(true);
    if (useCyclesPerSecond) {
        ui->labelRunningCycleNum->setVisible(true);
    }
    currentCycleNum = 1;
    instructionCycleCount = cycleCountArray[Memory[PC]];
    futureWatcher.setFuture(QtConcurrent::run([this]() {
        auto last = std::chrono::system_clock::now() + std::chrono::nanoseconds(executionSpeed * stepSkipCount);
        while (running) {
           auto cur = std::chrono::system_clock::now();
           if (cur >= last) {
                last = cur + std::chrono::nanoseconds(executionSpeed * stepSkipCount);
                for (int i = 0; i < stepSkipCount; i++) {
                    if (!running){
                        break;
                    }
                    if (useCyclesPerSecond) {
                        if(currentCycleNum < instructionCycleCount){
                            currentCycleNum++;
                            if(i+1 == stepSkipCount){
                                QMetaObject::invokeMethod(this, "setUiUpdateData", Qt::QueuedConnection,Q_ARG(int, 2),Q_ARG(int, currentCycleNum));
                            }
                        }else{
                            executeInstruction();
                            switch (pendingInterrupt) {
                            case 0:
                                break;
                            case 1:
                                updateFlags(InterruptMask, 1);
                                PC = (Memory[interruptLocations - 1] << 8) + Memory[interruptLocations];
                                pendingInterrupt = 0;
                                break;
                            case 2:
                                Memory[SP] = PC & 0xFF;
                                SP--;
                                Memory[SP] = (PC >> 8) & 0xFF;
                                SP--;
                                Memory[SP] = xRegister & 0xFF;
                                SP--;
                                Memory[SP] = (xRegister >> 8) & 0xFF;
                                SP--;
                                Memory[SP] = aReg;
                                SP--;
                                Memory[SP] = bReg;
                                SP--;
                                Memory[SP] = flags;
                                SP--;
                                updateFlags(InterruptMask, 1);
                                PC = (Memory[(interruptLocations - 3)] << 8) + Memory[(interruptLocations - 2)];
                                pendingInterrupt = 0;
                                break;
                            case 3:
                                if (!bit(flags, 4)) {
                                    Memory[SP] = PC & 0xFF;
                                    SP--;
                                    Memory[SP] = (PC >> 8) & 0xFF;
                                    SP--;
                                    Memory[SP] = xRegister & 0xFF;
                                    SP--;
                                    Memory[SP] = (xRegister >> 8) & 0xFF;
                                    SP--;
                                    Memory[SP] = aReg;
                                    SP--;
                                    Memory[SP] = bReg;
                                    SP--;
                                    Memory[SP] = flags;
                                    SP--;
                                    updateFlags(InterruptMask, 1);
                                    PC = (Memory[(interruptLocations - 7)] << 8) + Memory[(interruptLocations - 6)];
                                }
                                pendingInterrupt = 0;
                                break;
                            }
                            switch (breakWhenIndex) {
                            case 0:
                                break;
                            case 1:
                                if (instructionList.getObjectByAddress(PC).lineNumber == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 2:
                                if (PC == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 3:
                                if (SP == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 4:
                                if (xRegister == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 5:
                                if (aReg == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 6:
                                if (bReg == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 7:
                                if (bit(flags, 5) == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 8:
                                if (bit(flags, 4) == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 9:
                                if (bit(flags, 3) == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 10:
                                if (bit(flags, 2) == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 11:
                                if (bit(flags, 1) == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 12:
                                if (bit(flags, 0) == breakIsValue) {
                                    running = false;
                                }
                                break;
                            case 13:
                                if (Memory[breakAtValue] == breakIsValue) {
                                    running = false;
                                }
                                break;
                            }
                            instructionCycleCount = cycleCountArray[Memory[PC]];
                            currentCycleNum = 1;
                            if(i+1 == stepSkipCount){
                                QMetaObject::invokeMethod(this, "setUiUpdateData", Qt::QueuedConnection,Q_ARG(int, 1),Q_ARG(const uint8_t*, Memory),Q_ARG(int, currentCycleNum),Q_ARG(uint8_t, flags),Q_ARG(uint16_t, PC),Q_ARG(uint16_t, SP),Q_ARG(uint8_t, aReg),Q_ARG(uint8_t, bReg),Q_ARG(uint16_t, xRegister));
                            }
                        }
                    } else {
                        switch (pendingInterrupt) {
                        case 0:
                            executeInstruction();
                            break;
                        case 1:
                            updateFlags(InterruptMask, 1);
                            PC = (Memory[interruptLocations - 1] << 8) + Memory[interruptLocations];
                            pendingInterrupt = 0;
                            break;
                        case 2:
                            Memory[SP] = PC & 0xFF;
                            SP--;
                            Memory[SP] = (PC >> 8) & 0xFF;
                            SP--;
                            Memory[SP] = xRegister & 0xFF;
                            SP--;
                            Memory[SP] = (xRegister >> 8) & 0xFF;
                            SP--;
                            Memory[SP] = aReg;
                            SP--;
                            Memory[SP] = bReg;
                            SP--;
                            Memory[SP] = flags;
                            SP--;
                            updateFlags(InterruptMask, 1);
                            PC = (Memory[(interruptLocations - 3)] << 8) + Memory[(interruptLocations - 2)];
                            pendingInterrupt = 0;
                            break;
                        case 3:
                            if (!bit(flags, 4)) {
                                Memory[SP] = PC & 0xFF;
                                SP--;
                                Memory[SP] = (PC >> 8) & 0xFF;
                                SP--;
                                Memory[SP] = xRegister & 0xFF;
                                SP--;
                                Memory[SP] = (xRegister >> 8) & 0xFF;
                                SP--;
                                Memory[SP] = aReg;
                                SP--;
                                Memory[SP] = bReg;
                                SP--;
                                Memory[SP] = flags;
                                SP--;
                                updateFlags(InterruptMask, 1);
                                PC = (Memory[(interruptLocations - 7)] << 8) + Memory[(interruptLocations - 6)];
                            } else {
                                executeInstruction();
                            }
                            pendingInterrupt = 0;
                            break;
                        }
                        switch (breakWhenIndex) {
                        case 0:
                            break;
                        case 1:
                            if (instructionList.getObjectByAddress(PC).lineNumber == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 2:
                            if (PC == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 3:
                            if (SP == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 4:
                            if (xRegister == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 5:
                            if (aReg == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 6:
                            if (bReg == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 7:
                            if (bit(flags, 5) == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 8:
                            if (bit(flags, 4) == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 9:
                            if (bit(flags, 3) == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 10:
                            if (bit(flags, 2) == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 11:
                            if (bit(flags, 1) == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 12:
                            if (bit(flags, 0) == breakIsValue) {
                                running = false;
                            }
                            break;
                        case 13:
                            if (Memory[breakAtValue] == breakIsValue) {
                                running = false;
                            }
                            break;
                        }
                        if(i+1 == stepSkipCount){
                            QMetaObject::invokeMethod(this, "setUiUpdateData", Qt::QueuedConnection,Q_ARG(int, 1),Q_ARG(const uint8_t*, Memory),Q_ARG(int, currentCycleNum),Q_ARG(uint8_t, flags),Q_ARG(uint16_t, PC),Q_ARG(uint16_t, SP),Q_ARG(uint8_t, aReg),Q_ARG(uint8_t, bReg),Q_ARG(uint16_t, xRegister));
                        }
                    }
                }

            }

        }
        QMetaObject::invokeMethod(this, "stopUiUpdateTimer", Qt::QueuedConnection);

    }));
}
void MainWindow::stopUiUpdateTimer(){
    uiUpdateTimer->stop();
    stopExecution();
}
void MainWindow::stopExecution()
{
    running = false;
    futureWatcher.waitForFinished();
    ui->labelRunningIndicatior->setVisible(false);
    ui->labelRunningCycleNum->setVisible(false);
    updateUi();
}
void MainWindow::executeInstruction()
{
    uint8_t uInt8 = 0;
    uint8_t uInt82 = 0;
    int8_t sInt8 = 0;
    uint16_t uInt16 = 0;
    uint16_t uInt162 = 0;
    uint16_t adr = 0;
    uint16_t *curIndReg = &xRegister;

    //if (!indexRegister) {
    //curIndReg = &yRegister;
    //}

    switch (Memory[PC])
    {
        case 0x00:
        if(running){
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
            if (compilerVersionIndex >= 1)
            {

                uInt8 = bReg &0x01;
                uInt16 = (aReg << 8) + bReg;
                uInt16 = (uInt16 >> 1);
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);
                updateFlags(Negative, 0);
                updateFlags(Zero, uInt16 == 0);
                updateFlags(Overflow, uInt8);
                updateFlags(Carry, uInt8);
                PC++;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x05:
            if (compilerVersionIndex >= 1)
            {
                uInt8 = (bit(aReg, 7));
                uInt16 = (aReg << 8) + bReg;
                uInt16 = uInt16 << 1;
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);
                updateFlags(Negative, bit(aReg, 7));
                updateFlags(Zero, uInt16 == 0);
                updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
                updateFlags(Carry, uInt8);
                PC++;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

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
            PC++;
            break;
        case 0x08:
            (*curIndReg) ++;
            updateFlags(Zero, (*curIndReg) == 0);
            PC++;
            break;
        case 0x09:
            (*curIndReg) --;
            updateFlags(Zero, (*curIndReg) == 0);
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
            updateFlags(Overflow, (bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7) && bit(bReg, 7) && bit(uInt8, 7)));
            updateFlags(Carry, ((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
            aReg = uInt8;
            PC++;
            break;
        case 0x11:
            uInt8 = aReg - bReg;
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(bReg, 7) && !bit(uInt8, 7)) || (!bit(aReg, 7) && bit(bReg, 7) && bit(uInt8, 7)));
            updateFlags(Carry, ((!bit(aReg, 7) && bit(bReg, 7)) || (bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && !bit(aReg, 7))));
            PC++;
            break;
        case 0x16:
            bReg = aReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);
            PC++;
            break;
        case 0x17:
            aReg = bReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);
            PC++;
            break;
        case 0x19:
            uInt8 = flags &0x01;
            uInt82 = bit(flags, 5);
            uInt16 = aReg >> 4;
            uInt162 = aReg &0xF;
            if (uInt8 != 1 && uInt16 <= 9 && uInt82 != 1 && uInt162 <= 9)
            {
                aReg += 0x0;
                updateFlags(Carry, 0);
            }
            else if (uInt8 != 1 && uInt16 <= 8 && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF)
            {
                aReg += 0x6;
                updateFlags(Carry, 0);
            }
            else if (uInt8 != 1 && uInt16 <= 9 && uInt82 && uInt162 <= 3)
            {
                aReg += 0x6;
                updateFlags(Carry, 0);
            }
            else if (uInt8 != 1 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 != 1 && uInt162 <= 9)
            {
                aReg += 0x60;
                updateFlags(Carry, 1);
            }
            else if (uInt8 != 1 && uInt16 >= 9 && uInt16 <= 0xF && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF)
            {
                aReg += 0x66;
                updateFlags(Carry, 1);
            }
            else if (uInt8 != 1 && uInt16 >= 0xA && uInt16 <= 0xF && uInt82 && uInt162 <= 3)
            {
                aReg += 0x66;
                updateFlags(Carry, 1);
            }
            else if (uInt8 && uInt16 <= 2 && uInt82 != 1 && uInt162 <= 9)
            {
                aReg += 0x60;
                updateFlags(Carry, 1);
            }
            else if (uInt8 && uInt16 <= 2 && uInt82 != 1 && uInt162 >= 0xA && uInt162 <= 0xF)
            {
                aReg += 0x66;
                updateFlags(Carry, 1);
            }
            else if (uInt8 && uInt16 <= 3 && uInt82 && uInt162 <= 3)
            {
                aReg += 0x66;
                updateFlags(Carry, 1);
            }

            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            PC++;
            break;
        case 0x1B:
            uInt16 = aReg + bReg;
            updateFlags(Carry, (bit(aReg, 7) && bit(bReg, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(bReg, 7) && !bit(uInt16, 7)));
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(bReg, 3)) || (bit(aReg, 3) && !bit(uInt16, 3)) || (bit(bReg, 3) && !bit(uInt16, 3)));
            updateFlags(Overflow, (bit(aReg, 7) && bit(bReg, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(bReg, 7) && bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            PC++;
            break;
        case 0x20:
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += sInt8 + 2;
            break;
        case 0x21:
            if (compilerVersionIndex >= 1)
            {
                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x22:
            if ((bit(flags, 2) || (flags & 0x01)) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }

            PC += 2;
            break;
        case 0x23:
            if (bit(flags, 2) || (flags & 0x01))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x24:
            if ((flags & 0x01) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x25:
            if ((flags & 0x01))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x26:
            if (bit(flags, 2) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x27:
            if (bit(flags, 2))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x28:
            if (bit(flags, 1) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x29:
            if (bit(flags, 1))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x2A:
            if (bit(flags, 3) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x2B:
            if (bit(flags, 3))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x2C:
            if ((bit(flags, 3) ^ bit(flags, 1)) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x2D:
            if (bit(flags, 3) ^ bit(flags, 1))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x2E:
            if ((bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1))) == 0)
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x2F:
            if (bit(flags, 2) || (bit(flags, 3) ^ bit(flags, 1)))
            {
                sInt8 = Memory[(PC + 1) % 0x10000];
                PC += sInt8;
            }
            PC += 2;
            break;
        case 0x30:
            (*curIndReg) = SP + 1;
            PC++;
            break;
        case 0x31:
            SP++;
            PC++;
            break;
        case 0x32:
            SP++;
            aReg = Memory[SP];
            PC++;
            break;
        case 0x33:
            SP++;
            bReg = Memory[SP];
            PC++;
            break;
        case 0x34:
            SP--;
            PC++;
            break;
        case 0x35:
            SP = (*curIndReg) - 1;
            PC++;
            break;
        case 0x36:
            Memory[SP] = aReg;
            SP--;
            PC++;
            break;
        case 0x37:
            Memory[SP] = bReg;

            SP--;
            PC++;
            break;
        case 0x38:
            if (compilerVersionIndex >= 1)
            {
                SP++;
                (*curIndReg) = (Memory[SP] << 8);
                SP++;
                (*curIndReg) += Memory[SP];
                PC++;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x39:
            SP++;
            PC = Memory[SP] << 8;
            SP++;
            PC += Memory[SP];
            break;
        case 0x3A:
            if (compilerVersionIndex >= 1)
            {
                (*curIndReg) = (*curIndReg) + bReg;
                PC++;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x3B:
            SP++;
            flags = Memory[SP];

            SP++;
            bReg = Memory[SP];

            SP++;
            aReg = Memory[SP];

            SP++;
            (*curIndReg) = (Memory[SP] << 8) + Memory[(SP + 1) % 0x10000];

            SP += 2;
            PC = (Memory[SP] << 8) + Memory[(SP + 1) % 0x10000];
            SP++;
            break;
        case 0x3C:
            if (compilerVersionIndex >= 1)
            {
                Memory[SP] = ((*curIndReg) &0xFF);

                Memory[SP - 1] = (((*curIndReg) >> 8) &0xFF);

                SP -= 2;
                PC++;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x3D:
            if (compilerVersionIndex >= 1)
            {
                uInt16 = static_cast<uint16_t> (aReg) *static_cast<uint16_t> (bReg);
                updateFlags(Carry, (uInt16 >> 8) != 0);
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);
                PC++;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x3E:
            if(WAIStatus == 0){
                if(WAIJumpsToInterrupt){
                    PC++;
                    Memory[SP] = PC & 0xFF;
                    SP--;
                    Memory[SP] = (PC >> 8) & 0xFF;
                    SP--;
                    Memory[SP] = xRegister & 0xFF;
                    SP--;
                    Memory[SP] = (xRegister >> 8) & 0xFF;
                    SP--;
                    Memory[SP] = aReg;
                    SP--;
                    Memory[SP] = bReg;
                    SP--;
                    Memory[SP] = flags;
                    SP--;
                    updateFlags(InterruptMask, 1);
                    PC = (Memory[(interruptLocations - 7)] << 8) + Memory[(interruptLocations - 6)];
                }
                WAIStatus++;
            }
            if(WAIJumpsToInterrupt){
                if(bit(flags,4)){
                    TU NE DELA
                }
            }else{
                if (lastInput != -1){
                    PC++;
                    lastInput = -1;
                }
            }

            break;
        case 0x3F:
            PC++;
            Memory[SP] = PC &0xFF;
            SP--;
            Memory[SP] = (PC >> 8) &0xFF;

            SP--;
            Memory[SP] = (*curIndReg) &0xFF;

            SP--;
            Memory[SP] = ((*curIndReg) >> 8) &0xFF;

            SP--;
            Memory[SP] = aReg;

            SP--;
            Memory[SP] = bReg;

            SP--;
            Memory[SP] = flags;

            SP--;
            updateFlags(InterruptMask, 1);
            PC = (Memory[(interruptLocations - 5)] << 8) + Memory[(interruptLocations - 4)];
            break;
        case 0x40:
            aReg = 0x0 - aReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, aReg == 0x80);
            updateFlags(Carry, aReg != 0);
            PC++;
            break;
        case 0x43:
            aReg = 0xFF - aReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 1);
            PC++;
            break;
        case 0x44:
            uInt8 = (aReg & 0x1);
            aReg = (aReg >> 1);
            updateFlags(Negative, 0);
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, uInt8);
            updateFlags(Carry, uInt8);
            PC++;
            break;
        case 0x46:
            uInt8 = (aReg & 0x01);
            aReg = aReg >> 1;
            aReg += (flags & 0x01) << 7;
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
            PC++;
            break;
        case 0x47:
            uInt8 = aReg &0x01;
            updateFlags(Carry, uInt8);
            aReg = (aReg >> 1) + (aReg & 0x80);

            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
            PC++;
            break;
        case 0x48:
            uInt8 = bit(aReg, 7);
            updateFlags(Carry, uInt8);
            aReg = aReg << 1;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
            PC++;
            break;
        case 0x49:
            uInt8 = bit(aReg, 7);
            aReg = (aReg << 1) + (flags & 0x01);
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(aReg, 7));
            PC++;
            break;
        case 0x4A:
            updateFlags(Overflow, aReg == 0x80);
            aReg--;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            PC++;
            break;
        case 0x4C:
            updateFlags(Overflow, aReg == 0x7F);
            aReg++;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            PC++;
            break;
        case 0x4D:
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);
            PC++;
            break;
        case 0x4F:
            aReg = 0;
            updateFlags(Negative, 0);
            updateFlags(Zero, 1);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);
            PC++;
            break;
        case 0x50:
            bReg = 0x0 - bReg;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, bReg == 0x80);
            updateFlags(Carry, bReg != 0);
            PC++;
            break;
        case 0x53:
            bReg = 0xFF - bReg;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 1);
            PC++;
            break;
        case 0x54:
            uInt8 = (bReg & 0x1);
            bReg = (bReg >> 1);
            updateFlags(Negative, 0);
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, uInt8);
            updateFlags(Carry, uInt8);
            PC++;
            break;
        case 0x56:
            uInt8 = (bReg & 0x01);
            bReg = bReg >> 1;
            bReg += (flags & 0x01) << 7;
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(bReg, 7));
            PC++;
            break;
        case 0x57:
            uInt8 = bReg &0x01;
            updateFlags(Carry, uInt8);
            bReg = (bReg >> 1) + (bReg & 0x80);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(bReg, 7));

            PC++;
            break;
        case 0x58:
            uInt8 = bit(bReg, 7);
            updateFlags(Carry, uInt8);
            bReg = bReg << 1;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(bReg, 7));

            PC++;
            break;
        case 0x59:
            uInt8 = bit(bReg, 7);
            bReg = (bReg << 1) + (flags & 0x01);
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, uInt8 ^ bit(bReg, 7));

            PC++;
            break;
        case 0x5A:
            updateFlags(Overflow, bReg == 0x80);
            bReg--;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);

            PC++;
            break;
        case 0x5C:
            updateFlags(Overflow, bReg == 0x7F);
            bReg++;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);

            PC++;
            break;
        case 0x5D:
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);
            PC++;
            break;
        case 0x5F:
            bReg = 0;
            updateFlags(Negative, 0);
            updateFlags(Zero, 1);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);

            PC++;
            break;
        case 0x60:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = 0x0 - Memory[adr];
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, Memory[adr] == 0x80);
            updateFlags(Carry, Memory[adr] != 0);

            PC += 2;
            break;
        case 0x63:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = 0xFF - Memory[adr];
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 1);

            PC += 2;
            break;
        case 0x64:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            uInt8 = (Memory[adr] &0x1);
            Memory[adr] = (Memory[adr] >> 1);
            updateFlags(Negative, 0);
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8);
            updateFlags(Carry, uInt8);

            PC += 2;
            break;
        case 0x66:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            uInt8 = (Memory[adr] &0x01);
            Memory[adr] = Memory[adr] >> 1;
            Memory[adr] += (flags & 0x01) << 7;
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 2;
            break;
        case 0x67:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            uInt8 = (Memory[adr] &0x01);
            updateFlags(Carry, uInt8);
            Memory[adr] = (Memory[adr] >> 1) + (Memory[adr] &0x80);
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 2;
            break;
        case 0x68:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            uInt8 = bit(Memory[adr], 7);
            updateFlags(Carry, uInt8);
            Memory[adr] = Memory[adr] << 1;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 2;
            break;
        case 0x69:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            uInt8 = bit(Memory[adr], 7);
            Memory[adr] = (Memory[adr] << 1) + (flags & 0x01);
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 2;
            break;
        case 0x6A:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            updateFlags(Overflow, Memory[adr] == 0x80);
            Memory[adr]--;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);

            PC += 2;
            break;
        case 0x6C:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            updateFlags(Overflow, Memory[adr] == 0x7F);
            Memory[adr]++;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);

            PC += 2;
            break;
        case 0x6D:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);
            PC += 2;
            break;
        case 0x6E:
            PC = ((Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000);
            break;
        case 0x6F:
            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = 0;
            updateFlags(Negative, 0);
            updateFlags(Zero, 1);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);

            PC += 2;
            break;
        case 0x70:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = 0x0 - Memory[adr];
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, Memory[adr] == 0x80);
            updateFlags(Carry, Memory[adr] != 0);

            PC += 2;
            break;
        case 0x73:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = 0xFF - Memory[adr];
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 1);

            PC += 3;
            break;
        case 0x74:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt8 = (Memory[adr] &0x1);
            Memory[adr] = (Memory[adr] >> 1);
            updateFlags(Negative, 0);
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8);
            updateFlags(Carry, uInt8);

            PC += 3;
            break;
        case 0x76:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt8 = (Memory[adr] &0x01);
            Memory[adr] = Memory[adr] >> 1;
            Memory[adr] += (flags & 0x01) << 7;
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 3;
            break;
        case 0x77:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt8 = (Memory[adr] &0x01);
            updateFlags(Carry, uInt8);
            Memory[adr] = (Memory[adr] >> 1) + (Memory[adr] &0x80);
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 3;
            break;
        case 0x78:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt8 = bit(Memory[adr], 7);
            updateFlags(Carry, uInt8);
            Memory[adr] = Memory[adr] << 1;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 3;
            break;
        case 0x79:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt8 = bit(Memory[adr], 7);
            Memory[adr] = (Memory[adr] << 1) + (flags & 0x01);
            updateFlags(Carry, uInt8);
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, uInt8 ^ bit(Memory[adr], 7));

            PC += 3;
            break;
        case 0x7A:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            updateFlags(Overflow, Memory[adr] == 0x80);
            Memory[adr]--;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);

            PC += 3;
            break;
        case 0x7C:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            updateFlags(Overflow, Memory[adr] == 0x7F);
            Memory[adr]++;
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);

            PC += 3;
            break;
        case 0x7D:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            updateFlags(Negative, bit(Memory[adr], 7));
            updateFlags(Zero, Memory[adr] == 0);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);
            PC += 3;
            break;
        case 0x7E:
            PC = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            break;
        case 0x7F:
            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = 0;
            updateFlags(Negative, 0);
            updateFlags(Zero, 1);
            updateFlags(Overflow, 0);
            updateFlags(Carry, 0);

            PC += 3;
            break;
        case 0x80:
            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 2;

            break;
        case 0x81:
            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            PC += 2;
            break;
        case 0x82:
            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt82 = aReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 2;

            break;
        case 0x83:
            if (compilerVersionIndex >= 1)
            {
                adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                uInt16 = (aReg << 8) + bReg;
                uInt162 = uInt16 - adr;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && !bit(adr, 15) && !bit(uInt162, 15)) || (!bit(aReg, 7) && bit(adr, 15) && bit(uInt162, 15)));
                updateFlags(Carry, (!bit(aReg, 7) && bit(adr, 15)) || (bit(adr, 15) && bit(uInt162, 15)) || (bit(uInt162, 15) && !bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x84:
            aReg = (aReg &Memory[(PC + 1) % 0x10000]);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x85:
            uInt8 = (aReg &Memory[(PC + 1) % 0x10000]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 2;
            break;
        case 0x86:
            aReg = Memory[(PC + 1) % 0x10000];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x88:
            aReg = aReg ^ Memory[(PC + 1) % 0x10000];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x89:
            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt16 = aReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 2;
            break;
        case 0x8A:
            aReg = aReg | Memory[(PC + 1) % 0x10000];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x8B:
            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt16 = aReg + uInt8;
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 2;
            break;
        case 0x8C:
            uInt16 = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt162 = (*curIndReg) - uInt16;
            updateFlags(Negative, bit(uInt162, 15));
            updateFlags(Zero, uInt162 == 0);
            updateFlags(Overflow, (bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
            PC += 3;
            break;
        case 0x8D:
            sInt8 = Memory[(PC + 1) % 0x10000];
            PC += 2;
            Memory[SP] = (PC & 0xFF);

            Memory[SP - 1] = ((PC >> 8) &0xFF);

            SP -= 2;
            PC += sInt8;

            break;
        case 0x8E:
            SP = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);
            PC += 3;

            break;
        case 0x90:
            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 2;

            break;
        case 0x91:
            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            PC += 2;
            break;
        case 0x92:
            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt82 = aReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 2;

            break;
        case 0x93:
            if (compilerVersionIndex >= 1)
            {

                uInt8 = Memory[(PC + 1) % 0x10000];
                adr = (Memory[uInt8] << 8) + Memory[uInt8 + 1];
                uInt16 = (aReg << 8) + bReg;
                uInt162 = uInt16 - adr;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && !bit(adr, 15) && !bit(uInt162, 15)) || (!bit(aReg, 7) && bit(adr, 15) && bit(uInt162, 15)));
                updateFlags(Carry, (!bit(aReg, 7) && bit(adr, 15)) || (bit(adr, 15) && bit(uInt162, 15)) || (bit(uInt162, 15) && !bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x94:

            aReg = (aReg &Memory[Memory[(PC + 1) % 0x10000]]);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x95:

            uInt8 = (aReg &Memory[Memory[(PC + 1) % 0x10000]]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 2;
            break;
        case 0x96:

            aReg = Memory[Memory[(PC + 1) % 0x10000]];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x97:

            adr = Memory[(PC + 1) % 0x10000];
            Memory[adr] = aReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x98:

            aReg = aReg ^ Memory[Memory[(PC + 1) % 0x10000]];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x99:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt16 = aReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 2;
            break;
        case 0x9A:

            aReg = aReg | Memory[Memory[(PC + 1) % 0x10000]];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0x9B:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt16 = aReg + uInt8;
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 2;
            break;
        case 0x9C:

            adr = Memory[(PC + 1) % 0x10000];
            uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            uInt162 = (*curIndReg) - uInt16;
            updateFlags(Negative, bit(uInt162, 15));
            updateFlags(Zero, uInt162 == 0);
            updateFlags(Overflow, (bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
            PC += 2;
            break;
        case 0x9D:

            if (compilerVersionIndex >= 1)
            {
                adr = Memory[(PC + 1) % 0x10000];
                PC += 2;
                Memory[SP] = (PC & 0xFF);

                Memory[SP - 1] = ((PC >> 8) &0xFF);

                SP -= 2;
                PC = adr;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0x9E:

            adr = Memory[(PC + 1) % 0x10000];
            SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);
            PC += 2;

            break;
        case 0x9F:

            adr = Memory[(PC + 1) % 0x10000];
            Memory[adr] = SP >> 8;
            Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);


            PC += 2;
            break;
        case 0xA0:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 2;

            break;
        case 0xA1:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            PC += 2;
            break;
        case 0xA2:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt82 = aReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 2;

            break;
        case 0xA3:
            if (compilerVersionIndex >= 1)
            {

                uInt8 = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
                adr = (Memory[uInt8] << 8) + Memory[uInt8 + 1];
                uInt16 = (aReg << 8) + bReg;
                uInt162 = uInt16 - adr;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && !bit(adr, 15) && !bit(uInt162, 15)) || (!bit(aReg, 7) && bit(adr, 15) && bit(uInt162, 15)));
                updateFlags(Carry, (!bit(aReg, 7) && bit(adr, 15)) || (bit(adr, 15) && bit(uInt162, 15)) || (bit(uInt162, 15) && !bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xA4:

            aReg = (aReg &Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000]);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xA5:

            uInt8 = (aReg &Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 2;
            break;
        case 0xA6:

            aReg = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xA7:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = aReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xA8:

            aReg = aReg ^ Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xA9:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt16 = aReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 2;
            break;
        case 0xAA:

            aReg = aReg | Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xAB:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt16 = aReg + uInt8;
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 2;
            break;
        case 0xAC:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            uInt162 = (*curIndReg) - uInt16;
            updateFlags(Negative, bit(uInt162, 15));
            updateFlags(Zero, uInt162 == 0);
            updateFlags(Overflow, (bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
            PC += 2;
            break;
        case 0xAD:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            PC += 2;
            Memory[SP] = (PC & 0xFF);

            Memory[SP - 1] = ((PC >> 8) &0xFF);

            SP -= 2;
            PC = adr;

            break;
        case 0xAE:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);
            PC += 2;

            break;
        case 0xAF:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = SP >> 8;
            Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);


            PC += 2;
            break;
        case 0xB0:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 3;

            break;
        case 0xB1:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt82 = aReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            PC += 3;
            break;
        case 0xB2:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt82 = aReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(aReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(aReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(aReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(aReg, 7)));
            aReg = uInt82;
            PC += 3;

            break;
        case 0xB3:
            if (compilerVersionIndex >= 1)
            {

                adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                adr = (Memory[adr] << 8) + Memory[adr + 1];
                uInt16 = (aReg << 8) + bReg;
                uInt162 = uInt16 - adr;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && !bit(adr, 15) && !bit(uInt162, 15)) || (!bit(aReg, 7) && bit(adr, 15) && bit(uInt162, 15)));
                updateFlags(Carry, (!bit(aReg, 7) && bit(adr, 15)) || (bit(adr, 15) && bit(uInt162, 15)) || (bit(uInt162, 15) && !bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xB4:

            aReg = (aReg &Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]]);
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xB5:

            uInt8 = (aReg &Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 3;
            break;
        case 0xB6:

            aReg = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xB7:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = aReg;
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xB8:

            aReg = aReg ^ Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xB9:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt16 = aReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 3;
            break;
        case 0xBA:

            aReg = aReg | Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            updateFlags(Negative, bit(aReg, 7));
            updateFlags(Zero, aReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xBB:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt16 = aReg + uInt8;
            updateFlags(HalfCarry, (bit(aReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(aReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(aReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(aReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(aReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(aReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            aReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, aReg == 0);

            PC += 3;
            break;
        case 0xBC:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            uInt162 = (*curIndReg) - uInt16;
            updateFlags(Negative, bit(uInt162, 15));
            updateFlags(Zero, uInt162 == 0);
            updateFlags(Overflow, (bit((*curIndReg), 15) && !bit(uInt16, 15) && !bit(uInt162, 15)) || (!bit((*curIndReg), 15) && bit(uInt16, 15) && bit(uInt162, 15)));
            PC += 3;
            break;
        case 0xBD:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            PC += 3;
            Memory[SP] = (PC & 0xFF);

            Memory[SP - 1] = ((PC >> 8) &0xFF);

            SP -= 2;
            PC = adr;

            break;
        case 0xBE:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            SP = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);
            PC += 3;

            break;
        case 0xBF:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = SP >> 8;
            Memory[(adr + 1) % 0x10000] = (SP & 0xFF);
            updateFlags(Negative, bit(SP, 15));
            updateFlags(Zero, SP == 0);
            updateFlags(Overflow, 0);


            PC += 3;
            break;
        case 0xC0:

            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 2;

            break;
        case 0xC1:

            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            PC += 2;
            break;
        case 0xC2:

            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt82 = bReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 2;

            break;
        case 0xC3:
            if (compilerVersionIndex >= 1)
            {


                uInt16 = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                uInt162 = (aReg << 8) + bReg;
                uInt162 = uInt162 + uInt16;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(aReg, 7) && !bit(uInt16, 15) && bit(uInt162, 12)));
                updateFlags(Carry, (bit(aReg, 7) && bit(uInt16, 15)) || (bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(uInt162, 12) && bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xC4:

            bReg = (bReg &Memory[(PC + 1) % 0x10000]);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xC5:

            uInt8 = (bReg &Memory[(PC + 1) % 0x10000]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 2;
            break;
        case 0xC6:

            bReg = Memory[(PC + 1) % 0x10000];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xC8:

            bReg = bReg ^ Memory[(PC + 1) % 0x10000];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xC9:

            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt16 = bReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 2;
            break;
        case 0xCA:

            bReg = bReg | Memory[(PC + 1) % 0x10000];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xCB:

            uInt8 = Memory[(PC + 1) % 0x10000];
            uInt16 = bReg + uInt8;
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 2;
            break;
        case 0xCC:
            if (compilerVersionIndex >= 1)
            {


                uInt16 = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                updateFlags(Negative, bit(uInt16, 15));
                updateFlags(Zero, uInt16 == 0);
                updateFlags(Overflow, 0);
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);

                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xCE:

            (*curIndReg) = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);
            PC += 3;

            break;
        case 0xD0:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 2;

            break;
        case 0xD1:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            PC += 2;
            break;
        case 0xD2:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt82 = bReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 2;

            break;
        case 0xD3:
            if (compilerVersionIndex >= 1)
            {


                adr = Memory[(PC + 1) % 0x10000];
                uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
                uInt162 = (aReg << 8) + bReg;
                uInt162 = uInt162 + uInt16;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(aReg, 7) && !bit(uInt16, 15) && bit(uInt162, 12)));
                updateFlags(Carry, (bit(aReg, 7) && bit(uInt16, 15)) || (bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(uInt162, 12) && bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xD4:

            bReg = (bReg &Memory[Memory[(PC + 1) % 0x10000]]);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xD5:

            uInt8 = (bReg &Memory[Memory[(PC + 1) % 0x10000]]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 2;
            break;
        case 0xD6:

            bReg = Memory[Memory[(PC + 1) % 0x10000]];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xD7:

            adr = Memory[(PC + 1) % 0x10000];
            Memory[adr] = bReg;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xD8:

            bReg = bReg ^ Memory[Memory[(PC + 1) % 0x10000]];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xD9:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt16 = bReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 2;
            break;
        case 0xDA:

            bReg = bReg | Memory[Memory[(PC + 1) % 0x10000]];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xDB:

            uInt8 = Memory[Memory[(PC + 1) % 0x10000]];
            uInt16 = bReg + uInt8;
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 2;
            break;
        case 0xDC:
            if (compilerVersionIndex >= 1)
            {


                adr = Memory[(PC + 1) % 0x10000];
                uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
                updateFlags(Negative, bit(uInt16, 15));
                updateFlags(Zero, uInt16 == 0);
                updateFlags(Overflow, 0);
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);

                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xDD:
            if (compilerVersionIndex >= 1)
            {

                adr = Memory[(PC + 1) % 0x10000];
                Memory[adr] = aReg;
                Memory[adr + 1] = bReg;
                updateFlags(Negative, bit(aReg, 7));
                updateFlags(Zero, bReg + aReg == 0);
                updateFlags(Overflow, 0);


                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xDE:

            adr = Memory[(PC + 1) % 0x10000];
            (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);
            PC += 2;

            break;
        case 0xDF:

            adr = Memory[(PC + 1) % 0x10000];
            Memory[adr] = (*curIndReg) >> 8;
            Memory[(adr + 1) % 0x10000] = ((*curIndReg) &0xFF);
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);


            PC += 2;
            break;
        case 0xE0:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 2;

            break;
        case 0xE1:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            PC += 2;
            break;
        case 0xE2:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt82 = bReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 2;

            break;
        case 0xE3:
            if (compilerVersionIndex >= 1)
            {

                adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
                uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
                uInt162 = (aReg << 8) + bReg;
                uInt162 = uInt162 + uInt16;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(aReg, 7) && !bit(uInt16, 15) && bit(uInt162, 12)));
                updateFlags(Carry, (bit(aReg, 7) && bit(uInt16, 15)) || (bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(uInt162, 12) && bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xE4:

            bReg = (bReg &Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000]);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xE5:

            uInt8 = (bReg &Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 2;
            break;
        case 0xE6:

            bReg = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xE7:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = bReg;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xE8:

            bReg = bReg ^ Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xE9:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt16 = bReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 2;
            break;
        case 0xEA:

            bReg = bReg | Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 2;
            break;
        case 0xEB:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000];
            uInt16 = bReg + uInt8;
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 2;
            break;
        case 0xEC:
            if (compilerVersionIndex >= 1)
            {


                adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
                uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
                updateFlags(Negative, bit(uInt16, 15));
                updateFlags(Zero, uInt16 == 0);
                updateFlags(Overflow, 0);
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);

                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xED:
            if (compilerVersionIndex >= 1)
            {

                adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
                Memory[adr] = aReg;
                Memory[adr + 1] = bReg;
                updateFlags(Negative, bit(aReg, 7));
                updateFlags(Zero, bReg + aReg == 0);
                updateFlags(Overflow, 0);


                PC += 2;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xEE:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);
            PC += 2;

            break;
        case 0xEF:

            adr = (Memory[(PC + 1) % 0x10000] + *curIndReg) % 0x10000;
            Memory[adr] = (*curIndReg) >> 8;
            Memory[(adr + 1) % 0x10000] = ((*curIndReg) &0xFF);
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);


            PC += 2;
            break;
        case 0xF0:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 3;

            break;
        case 0xF1:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt82 = bReg - uInt8;
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            PC += 3;
            break;
        case 0xF2:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt82 = bReg - uInt8 - (flags & 0x1);
            updateFlags(Negative, bit(uInt82, 7));
            updateFlags(Zero, uInt82 == 0);
            updateFlags(Overflow, (bit(bReg, 7) && !bit(uInt8, 7) && !bit(uInt82, 7)) || (!bit(bReg, 7) && bit(uInt8, 7) && bit(uInt82, 7)));
            updateFlags(Carry, (!bit(bReg, 7) && bit(uInt8, 7)) || (bit(uInt8, 7) && bit(uInt82, 7)) || (bit(uInt82, 7) && !bit(bReg, 7)));
            bReg = uInt82;
            PC += 3;

            break;
        case 0xF3:
            if (compilerVersionIndex >= 1)
            {


                adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
                uInt162 = (aReg << 8) + bReg;
                uInt162 = uInt162 + uInt16;
                updateFlags(Negative, bit(uInt162, 15));
                updateFlags(Zero, uInt162 == 0);
                updateFlags(Overflow, (bit(aReg, 7) && bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(aReg, 7) && !bit(uInt16, 15) && bit(uInt162, 12)));
                updateFlags(Carry, (bit(aReg, 7) && bit(uInt16, 15)) || (bit(uInt16, 15) && !bit(uInt162, 12)) || (!bit(uInt162, 12) && bit(aReg, 7)));
                aReg = (uInt162 >> 8);
                bReg = (uInt162 & 0xFF);

                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xF4:

            bReg = (bReg &Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]]);
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xF5:

            uInt8 = (bReg &Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]]);
            updateFlags(Negative, bit(uInt8, 7));
            updateFlags(Zero, uInt8 == 0);
            updateFlags(Overflow, 0);
            PC += 3;
            break;
        case 0xF6:

            bReg = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xF7:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = bReg;
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xF8:

            bReg = bReg ^ Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xF9:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt16 = bReg + uInt8 + (flags & 0x01);
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 3;
            break;
        case 0xFA:

            bReg = bReg | Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            updateFlags(Negative, bit(bReg, 7));
            updateFlags(Zero, bReg == 0);
            updateFlags(Overflow, 0);

            PC += 3;
            break;
        case 0xFB:

            uInt8 = Memory[(Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000]];
            uInt16 = bReg + uInt8;
            updateFlags(HalfCarry, (bit(bReg, 3) && bit(uInt8, 3)) || (bit(uInt8, 3) && !bit(uInt16, 3)) || (!bit(uInt16, 3) && bit(bReg, 3)));
            updateFlags(Negative, bit(uInt16, 7));
            updateFlags(Overflow, (bit(bReg, 7) && bit(uInt8, 7) && !bit(uInt16, 7)) || (!bit(bReg, 7) && !bit(uInt8, 7) && bit(uInt16, 7)));
            updateFlags(Carry, (bit(bReg, 7) && bit(uInt8, 7)) || (!bit(uInt16, 7) && bit(bReg, 7)) || (bit(uInt8, 7) && !bit(uInt16, 7)));
            bReg = static_cast<uint8_t> (uInt16);
            updateFlags(Zero, bReg == 0);

            PC += 3;
            break;
        case 0xFC:
            if (compilerVersionIndex >= 1)
            {

                adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                uInt16 = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
                updateFlags(Negative, bit(uInt16, 15));
                updateFlags(Zero, uInt16 == 0);
                updateFlags(Overflow, 0);
                aReg = (uInt16 >> 8);
                bReg = (uInt16 & 0xFF);

                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xFD:
            if (compilerVersionIndex >= 1)
            {

                adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
                Memory[adr] = aReg;
                Memory[adr + 1] = bReg;
                updateFlags(Negative, bit(aReg, 7));
                updateFlags(Zero, bReg + aReg == 0);
                updateFlags(Overflow, 0);


                PC += 3;
            }
            else
            {
                if(incrementPCOnMissingInstruction){
                    PC++;
                } else if(running){
                    running = false;
                }
            }

            break;
        case 0xFE:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            (*curIndReg) = (Memory[adr] << 8) + Memory[(adr + 1) % 0x10000];
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);
            PC += 3;

            break;
        case 0xFF:

            adr = (Memory[(PC + 1) % 0x10000] << 8) + Memory[(PC + 2) % 0x10000];
            Memory[adr] = (*curIndReg) >> 8;
            Memory[(adr + 1) % 0x10000] = ((*curIndReg) &0xFF);
            updateFlags(Negative, bit((*curIndReg), 15));
            updateFlags(Zero, (*curIndReg) == 0);
            updateFlags(Overflow, 0);


            PC += 3;
            break;
        default:
            if(incrementPCOnMissingInstruction){
                PC++;
            } else if(running){
                running = false;
            }
            break;
    }

    PC = PC % 0x10000;
}

bool MainWindow::on_buttonCompile_clicked()
{
    ui->plainTextConsole->clear();
    if (!writeToMemory)
    {
        bool ok = false;
        ok = compileMix(compilerVersionIndex);
        resetEmulator(!ok);
        return ok;
    }
    else
    {
        bool ok;
        QString text = QInputDialog::getText(this, "Input Dialog", "Enter the decimal address where the program beggins. Data before that will be written with .BYTE.", QLineEdit::Normal, QString(), &ok);
        if (ok)
        {
            bool iok;
            int number = text.toInt(&iok);
            if (iok && number >= 0 && number <= 0xFFFF)
            {
                bool cok = reverseCompile(compilerVersionIndex, number);
                resetEmulator(!cok);
                return cok;
            }
            else
            {
                Err("Invalid address");
                return false;
            }
        }
        else
        {
            PrintConsole("Decompile canceled", 1);
            return false;
        }
    }
}
void MainWindow::on_comboBoxVersionSelector_currentIndexChanged(int index)
{
    compilerVersionIndex = index;
    setCompileStatus(false);
    resetEmulator(true);

}
void MainWindow::on_buttonLoad_clicked()
{
    if (!writeToMemory)
    {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));

        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in (&file);
                ui->plainTextCode->setPlainText(in .readAll());
                file.close();
            }
            else
            {
                PrintConsole("Error loading script", 0);
            }
        }
    }
    else
    {
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Binary Files (*.bin);;All Files (*)"));

        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray byteArray = file.readAll();
                if (byteArray.size() == sizeof(Memory))
                {
                    stopExecution();
                    if (compiled)
                    {
                        setCompileStatus(false);
                    }

                    std::memcpy(Memory, byteArray.constData(), sizeof(Memory));
                    std::memcpy(backupMemory, Memory, sizeof(Memory));
                    updateMemoryTab();
                    resetEmulator(false);
                }
                else
                {
                    PrintConsole("Error: File size doesn't match Memory size", 0);
                }

                file.close();
            }
            else
            {
                PrintConsole("Error loading memory", 0);
            }
        }
    }
}
void MainWindow::on_buttonSave_clicked()
{
    if (!writeToMemory)
    {
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                out << ui->plainTextCode->toPlainText();
                file.close();
            }
            else
            {
                PrintConsole("Error saving script", 0);
            }
        }
    }
    else
    {
        QByteArray byteArray(reinterpret_cast<char*> (Memory), sizeof(Memory));
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Binary Files (*.bin);;All Files (*)"));

        if (!filePath.isEmpty())
        {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly))
            {
                file.write(byteArray);
                file.close();
            }
            else
            {
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
    if (running)
    {
        stopExecution();
    }

    bool ok = true;
    if (!compiled && compileOnRun)
    {
        ok = on_buttonCompile_clicked();
    }

    if (ok)
    {
        switch (pendingInterrupt)
        {
            case 0:
                executeInstruction();
                break;
            case 1:
                updateFlags(InterruptMask, 1);
                PC = (Memory[interruptLocations - 1] << 8) + Memory[interruptLocations];
                pendingInterrupt = 0;
                break;
            case 2:
                Memory[SP] = PC &0xFF;

                SP--;
                Memory[SP] = (PC >> 8) &0xFF;

                SP--;
                Memory[SP] = xRegister &0xFF;

                SP--;
                Memory[SP] = (xRegister >> 8) &0xFF;

                SP--;
                Memory[SP] = aReg;

                SP--;
                Memory[SP] = bReg;

                SP--;
                Memory[SP] = flags;

                SP--;
                updateFlags(InterruptMask, 1);
                PC = (Memory[(interruptLocations - 3)] << 8) + Memory[(interruptLocations - 2)];
                pendingInterrupt = 0;
                break;
            case 3:
                if (!bit(flags, 4))
                {
                    Memory[SP] = PC &0xFF;

                    SP--;
                    Memory[SP] = (PC >> 8) &0xFF;

                    SP--;
                    Memory[SP] = xRegister &0xFF;

                    SP--;
                    Memory[SP] = (xRegister >> 8) &0xFF;

                    SP--;
                    Memory[SP] = aReg;

                    SP--;
                    Memory[SP] = bReg;


                    SP--;
                    Memory[SP] = flags;

                    SP--;
                    updateFlags(InterruptMask, 1);
                    PC = (Memory[(interruptLocations - 7)] << 8) + Memory[(interruptLocations - 6)];
                }
                else
                {
                    executeInstruction();
                }

                pendingInterrupt = 0;
                break;
        }

        updateUi();
    }
}
void MainWindow::on_buttonRunStop_clicked()
{
    if (running)
    {
        stopExecution();
    }
    else
    {
        bool ok = true;
        if (!compiled && compileOnRun)
        {
            ok = on_buttonCompile_clicked();
        }

        if (ok)
        {
            if (ui->checkBoxAutoReset->isChecked())
            {
                if (Memory[PC] == 0)
                {
                    resetEmulator(false);
                }
            }
            startExecution();
        }
    }
}
void MainWindow::on_comboBoxSpeedSelector_activated(int index)
{
    if (running) {
        stopExecution();
    }
    executionSpeed = std::pow(2, index);
    ui -> labelRunningIndicatior -> setText("Operation/second: " + QString::number(executionSpeed));
    stepSkipCount = std::ceil(executionSpeed / uiUpdateSpeed);
    executionSpeed = std::ceil(1000000000.0 / executionSpeed);

}
void MainWindow::on_checkBoxHexRegs_clicked(bool checked)
{
    hexReg = checked;
    updateUi();
}
void MainWindow::on_checkBoxAdvancedInfo_clicked(bool checked)
{
    if (checked)
    {
        ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), 181, ui->plainTextLines->height());
        ui->lineCodeLinesSeperator->setGeometry(190, ui->lineCodeLinesSeperator->y(), 1, 16);
        updateLinesBox();
    }
    else
    {
        ui->plainTextLines->setGeometry(ui->plainTextLines->x(), ui->plainTextLines->y(), 101, ui->plainTextLines->height());
        ui->lineCodeLinesSeperator->setGeometry(110, ui->lineCodeLinesSeperator->y(), 1, 16);
        updateLinesBox();
    }

    updateSelectionsLines();
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
    if (checked)
    {
        ui->buttonSwitchWrite->setVisible(true);
        ui->labelWritingMode->setVisible(true);
        ui->buttonSwitchWrite->setEnabled(true);
        ui->labelWritingMode->setEnabled(true);
    }
    else
    {
        ui->buttonSwitchWrite->setVisible(false);
        ui->labelWritingMode->setVisible(false);
        ui->buttonSwitchWrite->setEnabled(false);
        ui->labelWritingMode->setEnabled(false);
        writeToMemory = false;
        ui->plainTextCode->setReadOnly(false);
        ui->checkBoxCompileOnRun->setEnabled(true);
        compileOnRun = true;
        ui->labelWritingMode->setText("Code");
        ui->buttonLoad->setText("Load Code");
        ui->buttonSave->setText("Save Code");
        ui->buttonCompile->setText("Assemble");
        for (int row = 0; row < ui->tableWidgetMemory->rowCount(); row++) {
            for (int col = 0; col < ui->tableWidgetMemory->columnCount(); col++) {
                QTableWidgetItem* item = ui->tableWidgetMemory->item(row, col);
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
            }
        }
    }
}
void MainWindow::on_buttonSwitchWrite_clicked()
{
    if (writeToMemory)
    {
        writeToMemory = false;
        ui->plainTextCode->setReadOnly(false);
        ui->checkBoxCompileOnRun->setEnabled(true);
        compileOnRun = true;
        ui->labelWritingMode->setText("Code");
        ui->buttonLoad->setText("Load Code");
        ui->buttonSave->setText("Save Code");
        ui->buttonCompile->setText("Assemble");
        for (int row = 0; row < ui->tableWidgetMemory->rowCount(); row++) {
            for (int col = 0; col < ui->tableWidgetMemory->columnCount(); col++) {
                QTableWidgetItem* item = ui->tableWidgetMemory->item(row, col);
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
            }
        }
    }
    else
    {
        writeToMemory = true;
        ui->plainTextCode->setReadOnly(true);
        ui->checkBoxCompileOnRun->setEnabled(false);
        compileOnRun = false;
        ui->labelWritingMode->setText("Memory");

        ui->buttonLoad->setText("Load Memory");
        ui->buttonSave->setText("Save Memory");
        ui->buttonCompile->setText("Disassemble");
        for (int row = 0; row < ui->tableWidgetMemory->rowCount(); row++) {
            for (int col = 0; col < ui->tableWidgetMemory->columnCount(); col++) {
                QTableWidgetItem* item = ui->tableWidgetMemory->item(row, col);
                item->setFlags(item->flags() | Qt::ItemIsSelectable);
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            }
        }

    }
}
void MainWindow::on_comboBoxBreakWhen_currentIndexChanged(int index)
{
    ui->spinBoxBreakAt->setValue(0);
    ui->spinBoxBreakIs->setValue(0);
    breakWhenIndex = index;

    if (index == 13)
    {
        ui->labelAt->setVisible(true);
        ui->spinBoxBreakAt->setVisible(true);
    }
    else
    {
        ui->labelAt->setVisible(false);
        ui->spinBoxBreakAt->setVisible(false);
    }

    switch (index)
    {
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
void MainWindow::on_spinBoxBreakAt_valueChanged(int arg1)
{
    breakAtValue = arg1;
}
void MainWindow::on_spinBoxBreakIs_valueChanged(int arg1)
{
    breakIsValue = arg1;
}
void MainWindow::on_checkBoxSimpleMemory_clicked(bool checked)
{
    simpleMemory = checked;
    updateMemoryTab();
    if (checked)
    {
        ui->groupBoxSimpleMemory->setVisible(true);
        ui->groupBoxSimpleMemory->setEnabled(true);
        ui->tableWidgetMemory->setVisible(false);
        ui->tableWidgetMemory->setEnabled(false);
    }
    else
    {
        ui->tableWidgetMemory->setVisible(true);
        ui->tableWidgetMemory->setEnabled(true);
        ui->groupBoxSimpleMemory->setVisible(false);
        ui->groupBoxSimpleMemory->setEnabled(false);
        updateSelectionsLines();
    }
    updateSelectionsRunTime(PC);
}
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    currentSMScroll = arg1;
    updateMemoryTab();
}
void MainWindow::on_checkBoxAutoReset_2_clicked(bool checked)
{
    useCyclesPerSecond = checked;
    if (!useCyclesPerSecond)
    {
        ui->labelRunningCycleNum->setVisible(false);
    }
    else if (running)
    {
        ui->labelRunningCycleNum->setVisible(true);
    }

    instructionCycleCount = 0;
    currentCycleNum = 0;
}
void MainWindow::on_comboBoxDisplayStatus_currentIndexChanged(int index)
{
    if (index == 0)
    {
        externalDisplay->hide();
        ui->plainTextDisplay->setEnabled(false);
        ui->plainTextDisplay->setVisible(false);
        ui->frameDisplay->setEnabled(false);
        ui->frameDisplay->setVisible(false);
        displayStatusIndex = 0;
    }
    else if(index == 1){
        if(ui->comboBoxDisplayStatus->count() == 3){
            externalDisplay->hide();
            ui->plainTextDisplay->setEnabled(true);
            ui->plainTextDisplay->setVisible(true);
            ui->frameDisplay->setEnabled(true);
            ui->frameDisplay->setVisible(true);
            ui->plainTextDisplay->setPlainText(getDisplayText(Memory));
            displayStatusIndex = 1;
        }else{
            ui->plainTextDisplay->setEnabled(false);
            ui->plainTextDisplay->setVisible(false);
            ui->frameDisplay->setEnabled(false);
            ui->frameDisplay->setVisible(false);
            externalDisplay->show();
            displayStatusIndex = 2;
            plainTextDisplay->setPlainText(getDisplayText(Memory));
        }
    }else
    {
        ui->plainTextDisplay->setEnabled(false);
        ui->plainTextDisplay->setVisible(false);
        ui->frameDisplay->setEnabled(false);
        ui->frameDisplay->setVisible(false);
        externalDisplay->show();
        displayStatusIndex = 2;
        plainTextDisplay->setPlainText(getDisplayText(Memory));
    }
}
void MainWindow::on_lineEditBin_textChanged(const QString &arg1)
{
    if (ui->lineEditBin->text() != "X")
    {
        bool ok;
        int number = arg1.toInt(&ok, 2);
        if (ok)
        {
            ui->lineEditDec->setText(QString::number(number));
            ui->lineEditOct->setText(QString::number(number, 8));
            ui->lineEditHex->setText(QString::number(number, 16));
        }
        else
        {
            ui->lineEditDec->setText("X");
            ui->lineEditOct->setText("X");
            ui->lineEditHex->setText("X");
        }
    }
}
void MainWindow::on_lineEditOct_textChanged(const QString &arg1)
{
    if (ui->lineEditOct->text() != "X")
    {
        bool ok;
        int number = arg1.toInt(&ok, 8);

        if (ok)
        {
            ui->lineEditDec->setText(QString::number(number));
            ui->lineEditBin->setText(QString::number(number, 2));
            ui->lineEditHex->setText(QString::number(number, 16));
        }
        else
        {
            ui->lineEditDec->setText("X");
            ui->lineEditBin->setText("X");
            ui->lineEditHex->setText("X");
        }
    }
}
void MainWindow::on_lineEditHex_textChanged(const QString &arg1)
{
    if (ui->lineEditHex->text() != "X")
    {
        bool ok;
        int number = arg1.toInt(&ok, 16);
        if (ok)
        {
            ui->lineEditDec->setText(QString::number(number));
            ui->lineEditBin->setText(QString::number(number, 2));
            ui->lineEditOct->setText(QString::number(number, 8));
        }
        else
        {
            ui->lineEditDec->setText("X");
            ui->lineEditBin->setText("X");
            ui->lineEditOct->setText("X");
        }
    }
}
void MainWindow::on_lineEditDec_textChanged(const QString &arg1)
{
    if (ui->lineEditDec->text() != "X")
    {
        bool ok;
        int number = arg1.toInt(&ok);
        if (ok)
        {
            ui->lineEditBin->setText(QString::number(number, 2));
            ui->lineEditOct->setText(QString::number(number, 8));
            ui->lineEditHex->setText(QString::number(number, 16));
        }
        else
        {
            ui->lineEditBin->setText("X");
            ui->lineEditOct->setText("X");
            ui->lineEditHex->setText("X");
        }
    }
}
void MainWindow::on_spinBoxTabWidth_valueChanged(int arg1)
{
    QFontMetrics metrics(ui->plainTextCode->font());
    ui->plainTextCode->setTabStopDistance(metrics.horizontalAdvance(' ') *arg1);
}
void MainWindow::on_buttonTidyUp_clicked()
{
    QStringList lines = ui->plainTextCode->toPlainText().split("\n");

    int maxLabelLength = 0;
    for (const QString &line: lines)
    {
        if (line.isEmpty()) continue;
        int charNum = 0;
        if (line[charNum].isLetter())
        {
            for (; charNum < line.length(); ++charNum)
            {
                if (line[charNum] == ' ' || line[charNum] == '\t')
                {
                    break;
                }
            }

            maxLabelLength = std::max(maxLabelLength, charNum);
        }
    }

    int tabCount = ceil((maxLabelLength + 1.0F) / ui->spinBoxTabWidth->value());

    for (QString &line: lines)
    {
        if (line.isEmpty()) continue;
        int charNum = 0;
        if (line[charNum] == ' ' || line[charNum] == '\t')
        {
            for (; charNum < line.length(); ++charNum)
            {
                if (line[charNum] != ' ' && line[charNum] != '\t')
                {
                    break;
                }
            }

            line = QString(tabCount, '\t') + line.mid(charNum);
        }
        else
        {
            for (; charNum < line.length(); ++charNum)
            {
                if (line[charNum] == ' ' || line[charNum] == '\t')
                {
                    break;
                }
            }

            float labelEnd = charNum;
            charNum++;
            for (; charNum < line.length(); ++charNum)
            {
                if (line[charNum] != ' ' && line[charNum] != '\t')
                {
                    break;
                }
            }

            line = line.mid(0, labelEnd) + QString(tabCount - floor(labelEnd / ui->spinBoxTabWidth->value()), '\t') + line.mid(charNum);
        }
    }

    QString modifiedCode = lines.join("\n");
    ui->plainTextCode->setPlainText(modifiedCode);
}
void MainWindow::on_buttonRST_clicked()
{
    pendingInterrupt = 1;
}
void MainWindow::on_buttonNMI_clicked()
{
    if (pendingInterrupt == 0)
    {
        pendingInterrupt = 2;
    }
}
void MainWindow::on_pushButtonIRQ_clicked()
{
    if (pendingInterrupt == 0)
    {
        pendingInterrupt = 3;
    }
}




void MainWindow::on_checkBoxIncrementPC_clicked(bool checked)
{
    incrementPCOnMissingInstruction = checked;
}


void MainWindow::on_tableWidgetMemory_cellChanged(int row, int column)
{
    QTableWidgetItem* item = ui->tableWidgetMemory->item(row, column);
    if(item == ui->tableWidgetMemory->currentItem() && ui->tableWidgetMemory->isPersistentEditorOpen(item)){
        QString newText = item->text();
        bool ok;
        int value;
        if (hexReg) {
            value = newText.toInt(&ok, 16);
        }else{
            value = newText.toInt(&ok, 10);
        }
        int adr = row*16+column;
        if(!ok || value < 0 || value > 255){
            if (hexReg) {
                ui->tableWidgetMemory->item(row, column)->setText(QString("%1").arg(globalUpdateInfo.curMemory[adr], 2, 16, QChar('0')).toUpper());
            } else {
                ui->tableWidgetMemory->item(row, column)->setText(QString("%1").arg(globalUpdateInfo.curMemory[adr]));
            }
        } else{
            item->setText(newText.toUpper());
            Memory[adr] = value;
            if (!running)
            {
                if (compiled)
                {
                    setCompileStatus(false);
                }
                std::memcpy(backupMemory, Memory, sizeof(Memory));
            }
        }
    }
}


void MainWindow::on_checkBoxWAIJumps_clicked(bool checked)
{
    WAIJumpsToInterrupt = checked;
}

