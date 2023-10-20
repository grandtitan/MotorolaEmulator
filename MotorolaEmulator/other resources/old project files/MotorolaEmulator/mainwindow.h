#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

enum FlagToUpdate {
    HalfCarry,
    InterruptMask,
    Negative,
    Zero,
    Overflow,
    Carry
};
enum elementToUpdate {
    regPC,
    regSP,
    regA,
    regB,
    regX,
    allFlags
};
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateFlags(FlagToUpdate flag, bool value);
    void updateElement(elementToUpdate element);
    void executeInstruction();
    bool executeInstructionM6800();
    bool executeInstructionM6803();
    bool compileM6800();
    bool compileM6803();
    QString softwareVersion = "1.2";
    uint8_t Memory[0x10000] = {};
    uint8_t backupMemory[0x10000] = {};
    uint8_t aReg = 0,bReg = 0;
    uint16_t PC = 0, SP = 0xF000;
    uint16_t xRegister = 0;
    uint16_t yRegister = 0;
    uint8_t flags = 0;
    bool writeToMemory = false;
    bool breakEnabled = false;
    bool simpleMemory = false;
    int currentVersionIndex = 0;
    void test();
public slots:
    void handleVerticalScrollBarValueChanged(int value);
    void handleLinesScroll();
    void handleDisplayScrollVertical();
    void handleDisplayScrollHorizontal();
    void handleMemoryScrollHorizontal();
private:
    Ui::MainWindow *ui;
private:
    void resetEmulator();
    void updateMemoryTab();
    void updateLinesBox();
    void PrintConsole(const QString& text, int type);
    void Err(const QString& text);
    QTimer *executionTimer;
    bool running = false;
    int executionSpeed = 125;
    bool indexRegister;
    void updateMemoryCell(int address);
    bool hexReg = true;
    void updateSelectionsRunTime(int address);
    void updateSelectionsLines(int line);
protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;
signals:
    void resized(const QSize& newSize);
private slots:
    void handleMainWindowSizeChanged(const QSize& newSize);
    bool on_buttonCompile_clicked();
    void on_plainTextCode_textChanged();
    void on_buttonStep_clicked();
    void on_buttonReset_clicked();
    void on_buttonRunStop_clicked();
    void on_comboBoxSpeedSelector_activated(int index);
    void on_checkBoxHexRegs_clicked(bool checked);
    void on_checkBoxAdvancedInfo_clicked(bool checked);
    void on_checkBoxCompileOnRun_clicked(bool checked);
    void on_plainTextLines_cursorPositionChanged();
    void on_spinBoxLow_valueChanged(int arg1);
    void on_spinBoxUp_valueChanged(int arg1);
    void on_buttonSave_clicked();
    void on_buttonLoad_clicked();
    void on_lineEditBin_textChanged(const QString &arg1);
    void on_lineEditOct_textChanged(const QString &arg1);
    void on_lineEditHex_textChanged(const QString &arg1);
    void on_lineEditDec_textChanged(const QString &arg1);
    void on_buttonFSDisplay_clicked();
    void on_checkBoxWriteMemory_clicked(bool checked);
    void on_buttonSwitchWrite_clicked();
    void on_comboBoxBreakWhen_currentIndexChanged(int index);
    void on_checkBoxSimpleMemory_clicked(bool checked);
    void on_spinBox_valueChanged(int arg1);
    void on_comboBoxVersionSelector_currentIndexChanged(int index);
};

#endif // MAINWINDOW_H
