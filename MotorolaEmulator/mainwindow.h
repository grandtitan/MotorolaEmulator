#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "InstructionList.h"
#include "externaldisplay.h"
#include "qfuturewatcher.h"
#include "qtreewidget.h"
#include <QLineEdit>

enum FlagToUpdate {
    HalfCarry,
    InterruptMask,
    Negative,
    Zero,
    Overflow,
    Carry
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
    QString softwareVersion = "1.5";
    QFutureWatcher<void> futureWatcher;
    int lastInput = -1;
private:
    Ui::MainWindow *ui;
    ExternalDisplay *externalDisplay;
    InstructionList instructionList;
    QPlainTextEdit *plainTextDisplay;

    int displayStatusIndex;

    struct UpdateInfo {
        int whatToUpdate = 0;
        uint8_t curMemory[0x10000];
        int curCycle;
        uint8_t curFlags;
        uint16_t curPC;
        uint16_t curSP;
        uint8_t curA;
        uint8_t curB;
        uint16_t curX;
    };
    UpdateInfo globalUpdateInfo;
    void updateFlags(FlagToUpdate flag, bool value);
    void updateMemoryTab();
    void updateLinesBox();


    int previousRunTimeSelectionAddress = 0;
    int previousRunTimeSelectionSMAddress = 0;
    QVector<int> lineSelectionLines;
    QVector<int> lineSelectionAddresses;
    QList<QTextEdit::ExtraSelection> linesExtraSelectionsRunTime;
    QList<QTextEdit::ExtraSelection> codeExtraSelectionsRunTime;
    QList<QTextEdit::ExtraSelection> linesExtraSelectionsLines;
    QList<QTextEdit::ExtraSelection> codeExtraSelectionsLines;
    void updateSelectionsRunTime(int address);


    void updateSelectionsLines(int line = -1);
    void clearSelectionLines();

    void updateSelectionsMemoryEdit(int address = -1);
    void updateSelectionCompileError(int charNum);
    void clearSelection(int clearWhat);

    void PrintConsole(const QString& text, int type);
    void Err(const QString& text);

    int pendingInterrupt = 0; //1 RST //2 NMI //3 IRQ
    int oldCursorX = 0;
    int oldCursorY = 0;
    uint8_t backupMemory[0x10000] = {};

    uint8_t Memory[0x10000] = {};
    uint8_t aReg = 0,bReg = 0;
    uint16_t PC = 0, SP = 0x00FF;
    uint16_t xRegister = 0;
    uint8_t flags = 0;
    int currentCycleNum = 1;
    int instructionCycleCount = 0;

    void updateUi();
    void updateCurUi();
    bool incrementPCOnMissingInstruction = false;
    int interruptLocations = 0xFFFF;
    int executeInstruction();
    //uint16_t yRegister = 0; //not implemented
    //bool indexRegister = true; //true x false y not implemented properly

    void updateIfReady();
    QTimer *uiUpdateTimer;
    float uiUpdateSpeed = 256;
    int stepSkipCount = 0;
    int executionSpeed = 0;
    bool running = false;
    void stopExecution();
    void startExecution();
    void resetEmulator(bool failedCompile);
    int breakWhenIndex = 0;
    int breakAtValue = 0;
    int breakIsValue = 0;



    int previousScrollCode = 0;
    int compilerVersionIndex = 0;
    int currentCompilerLine = 0;
    int currentCompilerAddress = 0;
    std::unordered_map<QString, int> labelValMap;
    std::unordered_map<int, QString> callLabelMap;
    std::unordered_map<int, QString> callLabelRelMap;
    std::unordered_map<int, QString> callLabelRazMap;
    bool compileMix(int ver);
    bool compiled = false;
    void setCompileStatus(bool isCompile);
    QString uncompiledButton = "QPushButton{\n	color: rgb(0,0,0);\n	background-color: rgb(225,225,225);\n	border: 2px solid rgb(255,30,30);\n}\nQPushButton:hover{\n    background-color: rgb(229, 241, 251);\n    border: 2px solid rgb(255, 0, 50);\n}\nQPushButton:pressed{\n background-color: rgb(204, 228, 247);\n border: 2px solid rgb(255, 0, 50);\n}";
    QString compiledButton = "QPushButton{\n	color: rgb(0,0,0);\n	background-color: rgb(225,225,225);\n	border: 2px solid rgb(0,180,0);\n}\nQPushButton:hover{\n    background-color: rgb(229, 241, 251);\n    border: 2px solid rgb(0, 180, 20);\n}\nQPushButton:pressed{\n background-color: rgb(204, 228, 247);\n border: 2px solid rgb(0, 180, 20);\n}";

    int inputNextAddress(int curAdr, QString err);
    bool reverseCompile(int ver, int begLoc);

    bool simpleMemory = false;
    int currentSMScroll = 0;
    bool writeToMemory = false;
    bool useCyclesPerSecond = false;
    bool hexReg = true;
    bool compileOnRun = true;
    int autoScrollUpLimit = 20;
    int autoScrollDownLimit = 5;

    QStringList specialInstructions = { ".EQU", ".BYTE", ".ORG", ".WORD", ".RMB", ".SETB", ".SETW", ".STR" };
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
    int cycleCountArray[256] = {
        0, 2, 0, 0, 3, 3, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2,
        2, 2, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 0, 0, 0,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 4, 4, 3, 3, 3, 3, 5, 5, 3, 10, 4, 10, 9, 12,
        2, 0, 0, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 2,
        2, 0, 0, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 2,
        6, 0, 0, 6, 6, 0, 6, 6, 6, 6, 6, 0, 6, 6, 3, 6,
        6, 0, 0, 6, 6, 0, 6, 6, 6, 6, 6, 0, 6, 6, 3, 6,
        2, 2, 2, 4, 2, 2, 2, 0, 2, 2, 2, 2, 4, 6, 3, 0,
        3, 3, 3, 5, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 4, 4,
        4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 5, 5,
        4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 5, 5,
        2, 2, 2, 4, 2, 2, 2, 0, 2, 2, 2, 2, 3, 0, 3, 0,
        3, 3, 3, 5, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
        4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
        4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5
    };
protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;
signals:
    void resized(const QSize& newSize);
public slots:
    void setUiUpdateData(int whatToUpdate, const uint8_t* curMemory, int curCycle, uint8_t curFlags , uint16_t curPC, uint16_t curSP, uint8_t curA, uint8_t curB, uint16_t curX);
    void setUiUpdateData(int whatToUpdate, int curCycle);
    void showContextMenu(const QPoint &);
    void showMnemonicInfo();
    void showInstructionInfoWindow(QString instruction, int version);
private slots:
    void stopUiUpdateTimer();
    void handleCodeVerticalScrollBarValueChanged(int value);
    void handleLinesScroll();
    void handleDisplayScrollVertical();
    void handleDisplayScrollHorizontal();
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
    void on_spinBoxLow_valueChanged(int arg1);
    void on_spinBoxUp_valueChanged(int arg1);
    void on_buttonSave_clicked();
    void on_buttonLoad_clicked();
    void on_lineEditBin_textChanged(const QString &arg1);
    void on_lineEditOct_textChanged(const QString &arg1);
    void on_lineEditHex_textChanged(const QString &arg1);
    void on_lineEditDec_textChanged(const QString &arg1);
    void on_checkBoxWriteMemory_clicked(bool checked);
    void on_buttonSwitchWrite_clicked();
    void on_comboBoxBreakWhen_currentIndexChanged(int index);
    void on_checkBoxSimpleMemory_clicked(bool checked);
    void on_spinBox_valueChanged(int arg1);
    void on_comboBoxVersionSelector_currentIndexChanged(int index);
    void on_checkBoxAutoReset_2_clicked(bool checked);
    void on_comboBoxDisplayStatus_currentIndexChanged(int index);
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_spinBoxTabWidth_valueChanged(int arg1);
    void on_buttonTidyUp_clicked();
    void on_buttonRST_clicked();
    void on_buttonNMI_clicked();
    void on_pushButtonIRQ_clicked();
    void on_spinBoxBreakAt_valueChanged(int arg1);
    void on_spinBoxBreakIs_valueChanged(int arg1);
    void on_checkBoxIncrementPC_clicked(bool checked);
    void on_tableWidgetMemory_cellChanged(int row, int column);
};
#endif // MAINWINDOW_H
