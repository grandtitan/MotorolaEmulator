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
    QString softwareVersion = "1.4";
public slots:
    void handleVerticalScrollBarValueChanged(int value);
    void handleLinesScroll();
    void handleDisplayScrollVertical();
    void handleDisplayScrollHorizontal();
    void handleMemoryScrollHorizontal();
private:
    Ui::MainWindow *ui;
private:
    void updateFlags(FlagToUpdate flag, bool value);
    void updateElement(elementToUpdate element);
    void updateMemoryTab();
    void updateLinesBox();
    void updateMemoryCell(int address);
    void updateSelectionsRunTime(int address);
    void updateSelectionsLines(int line);
    void updateSelectionsMemoryEdit(int address);
    void updateSelectionCompileError(int charNum);
    void clearSelection(int clearWhat);
    void PrintConsole(const QString& text, int type);
    void Err(const QString& text);

    QTimer *executionTimer;
    bool running = false;
    int executionSpeed = 125;
    void executeLoop();
    void stopExecution();
    void startExecution();
    int executeInstruction();

    void resetEmulator(bool failedCompile);
    void breakCompile();

    int compilerVersionIndex = 0;
    int currentCompilerLine = 0;
    int currentCompilerAddress = 0;
    std::unordered_map<QString, int> labelValMap;
    std::unordered_map<int, QString> callLabelMap;
    std::unordered_map<int, QString> callLabelRelMap;
    std::unordered_map<int, QString> callLabelRazMap;
    bool compileMix(int ver);
    bool compiled = false;
    QString uncompiledButton = "border: 2px solid red;";
    QString compiledButton = "border: 2px solid green;";

    uint8_t Memory[0x10000] = {};
    uint8_t backupMemory[0x10000] = {};
    uint8_t aReg = 0,bReg = 0;
    uint16_t PC = 0, SP = 0xF000;
    uint16_t xRegister = 0;
    //uint16_t yRegister = 0; //not implemented
    //bool indexRegister = true; //true x false y not implemented properly
    uint8_t flags = 0;
    int waitCycles = 0;
    int cycleNum = 1;
    int interruptLocations = 0xFFFF;
    int lastInput = -1;


    bool simpleMemory = false;
    int currentSMScroll = 0;
    bool writeToMemory = false;
    bool breakEnabled = false;
    bool useCyclesPerSecond = false;
    bool hexReg = true;
    bool compileOnRun = true;

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
    void on_checkBoxAutoReset_2_clicked(bool checked);
};
#endif // MAINWINDOW_H
