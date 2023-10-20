/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPlainTextEdit *plainTextCode;
    QPlainTextEdit *plainTextLines;
    QPushButton *buttonCompile;
    QPushButton *buttonLoad;
    QPushButton *buttonSave;
    QPlainTextEdit *plainTextMemory;
    QPushButton *buttonStep;
    QPushButton *buttonRunStop;
    QPushButton *buttonReset;
    QGroupBox *groupBox;
    QLineEdit *lineEditAValue;
    QLineEdit *lineEditAName;
    QLineEdit *lineEditBName;
    QLineEdit *lineEditBValue;
    QLineEdit *lineEditXName;
    QLineEdit *lineEditXValue;
    QLineEdit *lineEditSPValue;
    QLineEdit *lineEditSName;
    QLineEdit *lineEditPCValue;
    QLineEdit *lineEditPCName;
    QLineEdit *lineEditNName;
    QLineEdit *lineEditNValue;
    QLineEdit *lineEditZName;
    QLineEdit *lineEditZValue;
    QLineEdit *lineEditVName;
    QLineEdit *lineEditVValue;
    QLineEdit *lineEditCName;
    QLineEdit *lineEditCValue;
    QLineEdit *lineEditIValue;
    QLineEdit *lineEditIName;
    QLineEdit *lineEditHValue;
    QLineEdit *lineEditHName;
    QLabel *labelRunningIndicatior;
    QTabWidget *tabWidget;
    QWidget *tabConsole;
    QPlainTextEdit *plainTextConsole;
    QWidget *tabConversion;
    QLineEdit *lineEditBin;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *lineEditOct;
    QLabel *label_5;
    QLineEdit *lineEditHex;
    QLabel *label_6;
    QLineEdit *lineEditDec;
    QComboBox *comboBoxBreakWhen;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QSpinBox *spinBoxBreakIs;
    QLabel *labelAt;
    QSpinBox *spinBoxBreakAt;
    QWidget *tabSettings;
    QCheckBox *checkBoxError;
    QCheckBox *checkBoxWarn;
    QCheckBox *checkBoxDebug;
    QCheckBox *checkBoxAdvancedInfo;
    QCheckBox *checkBoxHexRegs;
    QCheckBox *checkBoxCompileOnRun;
    QSpinBox *spinBoxLow;
    QSpinBox *spinBoxUp;
    QLabel *label;
    QLabel *label_2;
    QCheckBox *checkBoxWriteMemory;
    QCheckBox *checkBoxAutoReset;
    QCheckBox *checkBoxSimpleMemory;
    QWidget *Info;
    QTextEdit *plainTextInfo;
    QWidget *opCodes;
    QTreeWidget *treeWidget;
    QComboBox *comboBoxSpeedSelector;
    QFrame *frameDisplay;
    QPlainTextEdit *plainTextDisplay;
    QComboBox *comboBoxVersionSelector;
    QPushButton *buttonFSDisplay;
    QPushButton *buttonSwitchWrite;
    QLabel *labelWritingMode;
    QGroupBox *groupBoxSimpleMemory;
    QTableWidget *tableWidgetSM;
    QSpinBox *spinBox;
    QTableWidget *tableWidget;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 720);
        MainWindow->setMinimumSize(QSize(1280, 720));
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        font.setPointSize(9);
        font.setBold(true);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        plainTextCode = new QPlainTextEdit(centralwidget);
        plainTextCode->setObjectName("plainTextCode");
        plainTextCode->setGeometry(QRect(110, 10, 281, 651));
        QPalette palette;
        QBrush brush(QColor(254, 255, 240, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        plainTextCode->setPalette(palette);
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Courier New")});
        font1.setPointSize(11);
        font1.setBold(true);
        plainTextCode->setFont(font1);
        plainTextCode->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        plainTextCode->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        plainTextCode->setUndoRedoEnabled(false);
        plainTextCode->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextCode->setMaximumBlockCount(0);
        plainTextLines = new QPlainTextEdit(centralwidget);
        plainTextLines->setObjectName("plainTextLines");
        plainTextLines->setEnabled(true);
        plainTextLines->setGeometry(QRect(10, 10, 101, 651));
        QPalette palette1;
        QBrush brush1(QColor(255, 253, 201, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        plainTextLines->setPalette(palette1);
        plainTextLines->setFont(font1);
        plainTextLines->setAcceptDrops(false);
        plainTextLines->setAutoFillBackground(false);
        plainTextLines->setFrameShape(QFrame::StyledPanel);
        plainTextLines->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextLines->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextLines->setUndoRedoEnabled(false);
        plainTextLines->setReadOnly(true);
        plainTextLines->setTextInteractionFlags(Qt::TextSelectableByMouse);
        buttonCompile = new QPushButton(centralwidget);
        buttonCompile->setObjectName("buttonCompile");
        buttonCompile->setGeometry(QRect(10, 670, 80, 24));
        QPalette palette2;
        QBrush brush2(QColor(175, 175, 175, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonCompile->setPalette(palette2);
        buttonLoad = new QPushButton(centralwidget);
        buttonLoad->setObjectName("buttonLoad");
        buttonLoad->setGeometry(QRect(220, 670, 80, 24));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette3.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonLoad->setPalette(palette3);
        buttonSave = new QPushButton(centralwidget);
        buttonSave->setObjectName("buttonSave");
        buttonSave->setGeometry(QRect(310, 670, 80, 24));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette4.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonSave->setPalette(palette4);
        plainTextMemory = new QPlainTextEdit(centralwidget);
        plainTextMemory->setObjectName("plainTextMemory");
        plainTextMemory->setGeometry(QRect(400, 10, 506, 651));
        QPalette palette5;
        QBrush brush3(QColor(205, 248, 255, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette5.setBrush(QPalette::Active, QPalette::Base, brush3);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush3);
        plainTextMemory->setPalette(palette5);
        plainTextMemory->setFont(font1);
        plainTextMemory->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        plainTextMemory->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextMemory->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextMemory->setReadOnly(true);
        buttonStep = new QPushButton(centralwidget);
        buttonStep->setObjectName("buttonStep");
        buttonStep->setGeometry(QRect(730, 670, 80, 24));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette6.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonStep->setPalette(palette6);
        buttonRunStop = new QPushButton(centralwidget);
        buttonRunStop->setObjectName("buttonRunStop");
        buttonRunStop->setGeometry(QRect(820, 670, 80, 24));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette7.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonRunStop->setPalette(palette7);
        buttonReset = new QPushButton(centralwidget);
        buttonReset->setObjectName("buttonReset");
        buttonReset->setGeometry(QRect(630, 670, 80, 24));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette8.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonReset->setPalette(palette8);
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(910, 10, 361, 281));
        QPalette palette9;
        QBrush brush4(QColor(203, 203, 203, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette9.setBrush(QPalette::Active, QPalette::Base, brush4);
        palette9.setBrush(QPalette::Inactive, QPalette::Base, brush4);
        groupBox->setPalette(palette9);
        groupBox->setFlat(false);
        lineEditAValue = new QLineEdit(groupBox);
        lineEditAValue->setObjectName("lineEditAValue");
        lineEditAValue->setGeometry(QRect(80, 180, 81, 31));
        QPalette palette10;
        QBrush brush5(QColor(217, 215, 196, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette10.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette10.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditAValue->setPalette(palette10);
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Consolas")});
        font2.setPointSize(20);
        font2.setBold(true);
        lineEditAValue->setFont(font2);
        lineEditAValue->setAcceptDrops(false);
        lineEditAValue->setAlignment(Qt::AlignCenter);
        lineEditAValue->setReadOnly(true);
        lineEditAName = new QLineEdit(groupBox);
        lineEditAName->setObjectName("lineEditAName");
        lineEditAName->setGeometry(QRect(20, 180, 61, 31));
        QPalette palette11;
        QBrush brush6(QColor(152, 152, 152, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette11.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette11.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditAName->setPalette(palette11);
        lineEditAName->setFont(font2);
        lineEditAName->setAcceptDrops(false);
        lineEditAName->setAlignment(Qt::AlignCenter);
        lineEditAName->setReadOnly(true);
        lineEditBName = new QLineEdit(groupBox);
        lineEditBName->setObjectName("lineEditBName");
        lineEditBName->setGeometry(QRect(170, 180, 61, 31));
        QPalette palette12;
        palette12.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette12.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditBName->setPalette(palette12);
        lineEditBName->setFont(font2);
        lineEditBName->setAcceptDrops(false);
        lineEditBName->setAlignment(Qt::AlignCenter);
        lineEditBName->setReadOnly(true);
        lineEditBValue = new QLineEdit(groupBox);
        lineEditBValue->setObjectName("lineEditBValue");
        lineEditBValue->setGeometry(QRect(230, 180, 81, 31));
        QPalette palette13;
        palette13.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette13.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditBValue->setPalette(palette13);
        lineEditBValue->setFont(font2);
        lineEditBValue->setAcceptDrops(false);
        lineEditBValue->setAlignment(Qt::AlignCenter);
        lineEditBValue->setReadOnly(true);
        lineEditXName = new QLineEdit(groupBox);
        lineEditXName->setObjectName("lineEditXName");
        lineEditXName->setGeometry(QRect(20, 130, 61, 31));
        QPalette palette14;
        palette14.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette14.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditXName->setPalette(palette14);
        lineEditXName->setFont(font2);
        lineEditXName->setAcceptDrops(false);
        lineEditXName->setAlignment(Qt::AlignCenter);
        lineEditXName->setReadOnly(true);
        lineEditXValue = new QLineEdit(groupBox);
        lineEditXValue->setObjectName("lineEditXValue");
        lineEditXValue->setGeometry(QRect(80, 130, 81, 31));
        QPalette palette15;
        palette15.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette15.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditXValue->setPalette(palette15);
        lineEditXValue->setFont(font2);
        lineEditXValue->setAcceptDrops(false);
        lineEditXValue->setAlignment(Qt::AlignCenter);
        lineEditXValue->setReadOnly(true);
        lineEditSPValue = new QLineEdit(groupBox);
        lineEditSPValue->setObjectName("lineEditSPValue");
        lineEditSPValue->setGeometry(QRect(230, 30, 81, 31));
        QPalette palette16;
        palette16.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette16.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditSPValue->setPalette(palette16);
        lineEditSPValue->setFont(font2);
        lineEditSPValue->setAcceptDrops(false);
        lineEditSPValue->setAlignment(Qt::AlignCenter);
        lineEditSPValue->setReadOnly(true);
        lineEditSName = new QLineEdit(groupBox);
        lineEditSName->setObjectName("lineEditSName");
        lineEditSName->setGeometry(QRect(170, 30, 61, 31));
        QPalette palette17;
        palette17.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette17.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditSName->setPalette(palette17);
        lineEditSName->setFont(font2);
        lineEditSName->setAcceptDrops(false);
        lineEditSName->setAlignment(Qt::AlignCenter);
        lineEditSName->setReadOnly(true);
        lineEditPCValue = new QLineEdit(groupBox);
        lineEditPCValue->setObjectName("lineEditPCValue");
        lineEditPCValue->setGeometry(QRect(80, 30, 81, 31));
        QPalette palette18;
        palette18.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette18.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditPCValue->setPalette(palette18);
        lineEditPCValue->setFont(font2);
        lineEditPCValue->setAcceptDrops(false);
        lineEditPCValue->setAlignment(Qt::AlignCenter);
        lineEditPCValue->setReadOnly(true);
        lineEditPCName = new QLineEdit(groupBox);
        lineEditPCName->setObjectName("lineEditPCName");
        lineEditPCName->setGeometry(QRect(20, 30, 61, 31));
        QPalette palette19;
        palette19.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette19.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditPCName->setPalette(palette19);
        lineEditPCName->setFont(font2);
        lineEditPCName->setAcceptDrops(false);
        lineEditPCName->setAlignment(Qt::AlignCenter);
        lineEditPCName->setReadOnly(true);
        lineEditNName = new QLineEdit(groupBox);
        lineEditNName->setObjectName("lineEditNName");
        lineEditNName->setGeometry(QRect(121, 230, 20, 31));
        QPalette palette20;
        palette20.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette20.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditNName->setPalette(palette20);
        lineEditNName->setFont(font2);
        lineEditNName->setAcceptDrops(false);
        lineEditNName->setAlignment(Qt::AlignCenter);
        lineEditNName->setReadOnly(true);
        lineEditNValue = new QLineEdit(groupBox);
        lineEditNValue->setObjectName("lineEditNValue");
        lineEditNValue->setGeometry(QRect(140, 230, 21, 31));
        QPalette palette21;
        palette21.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette21.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditNValue->setPalette(palette21);
        lineEditNValue->setFont(font2);
        lineEditNValue->setAcceptDrops(false);
        lineEditNValue->setAlignment(Qt::AlignCenter);
        lineEditNValue->setReadOnly(true);
        lineEditZName = new QLineEdit(groupBox);
        lineEditZName->setObjectName("lineEditZName");
        lineEditZName->setGeometry(QRect(170, 230, 20, 31));
        QPalette palette22;
        palette22.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette22.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditZName->setPalette(palette22);
        lineEditZName->setFont(font2);
        lineEditZName->setAcceptDrops(false);
        lineEditZName->setAlignment(Qt::AlignCenter);
        lineEditZName->setReadOnly(true);
        lineEditZValue = new QLineEdit(groupBox);
        lineEditZValue->setObjectName("lineEditZValue");
        lineEditZValue->setGeometry(QRect(189, 230, 21, 31));
        QPalette palette23;
        palette23.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette23.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditZValue->setPalette(palette23);
        lineEditZValue->setFont(font2);
        lineEditZValue->setAcceptDrops(false);
        lineEditZValue->setAlignment(Qt::AlignCenter);
        lineEditZValue->setReadOnly(true);
        lineEditVName = new QLineEdit(groupBox);
        lineEditVName->setObjectName("lineEditVName");
        lineEditVName->setGeometry(QRect(220, 230, 21, 31));
        QPalette palette24;
        palette24.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette24.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditVName->setPalette(palette24);
        lineEditVName->setFont(font2);
        lineEditVName->setAcceptDrops(false);
        lineEditVName->setAlignment(Qt::AlignCenter);
        lineEditVName->setReadOnly(true);
        lineEditVValue = new QLineEdit(groupBox);
        lineEditVValue->setObjectName("lineEditVValue");
        lineEditVValue->setGeometry(QRect(240, 230, 21, 31));
        QPalette palette25;
        palette25.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette25.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditVValue->setPalette(palette25);
        lineEditVValue->setFont(font2);
        lineEditVValue->setAcceptDrops(false);
        lineEditVValue->setAlignment(Qt::AlignCenter);
        lineEditVValue->setReadOnly(true);
        lineEditCName = new QLineEdit(groupBox);
        lineEditCName->setObjectName("lineEditCName");
        lineEditCName->setGeometry(QRect(270, 230, 21, 31));
        QPalette palette26;
        palette26.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette26.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditCName->setPalette(palette26);
        lineEditCName->setFont(font2);
        lineEditCName->setAcceptDrops(false);
        lineEditCName->setAlignment(Qt::AlignCenter);
        lineEditCName->setReadOnly(true);
        lineEditCValue = new QLineEdit(groupBox);
        lineEditCValue->setObjectName("lineEditCValue");
        lineEditCValue->setGeometry(QRect(290, 230, 21, 31));
        QPalette palette27;
        palette27.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette27.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditCValue->setPalette(palette27);
        lineEditCValue->setFont(font2);
        lineEditCValue->setAcceptDrops(false);
        lineEditCValue->setAlignment(Qt::AlignCenter);
        lineEditCValue->setReadOnly(true);
        lineEditIValue = new QLineEdit(groupBox);
        lineEditIValue->setObjectName("lineEditIValue");
        lineEditIValue->setGeometry(QRect(90, 230, 21, 31));
        QPalette palette28;
        palette28.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette28.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditIValue->setPalette(palette28);
        lineEditIValue->setFont(font2);
        lineEditIValue->setAcceptDrops(false);
        lineEditIValue->setAlignment(Qt::AlignCenter);
        lineEditIValue->setReadOnly(true);
        lineEditIName = new QLineEdit(groupBox);
        lineEditIName->setObjectName("lineEditIName");
        lineEditIName->setGeometry(QRect(71, 230, 20, 31));
        QPalette palette29;
        palette29.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette29.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditIName->setPalette(palette29);
        lineEditIName->setFont(font2);
        lineEditIName->setAcceptDrops(false);
        lineEditIName->setAlignment(Qt::AlignCenter);
        lineEditIName->setReadOnly(true);
        lineEditHValue = new QLineEdit(groupBox);
        lineEditHValue->setObjectName("lineEditHValue");
        lineEditHValue->setGeometry(QRect(39, 230, 21, 31));
        QPalette palette30;
        palette30.setBrush(QPalette::Active, QPalette::Base, brush5);
        palette30.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        lineEditHValue->setPalette(palette30);
        lineEditHValue->setFont(font2);
        lineEditHValue->setAcceptDrops(false);
        lineEditHValue->setAlignment(Qt::AlignCenter);
        lineEditHValue->setReadOnly(true);
        lineEditHName = new QLineEdit(groupBox);
        lineEditHName->setObjectName("lineEditHName");
        lineEditHName->setGeometry(QRect(20, 230, 20, 31));
        QPalette palette31;
        palette31.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette31.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        lineEditHName->setPalette(palette31);
        lineEditHName->setFont(font2);
        lineEditHName->setAcceptDrops(false);
        lineEditHName->setAlignment(Qt::AlignCenter);
        lineEditHName->setReadOnly(true);
        labelRunningIndicatior = new QLabel(groupBox);
        labelRunningIndicatior->setObjectName("labelRunningIndicatior");
        labelRunningIndicatior->setGeometry(QRect(20, 60, 141, 16));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Consolas")});
        font3.setPointSize(8);
        font3.setBold(false);
        labelRunningIndicatior->setFont(font3);
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(910, 300, 361, 361));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideNone);
        tabConsole = new QWidget();
        tabConsole->setObjectName("tabConsole");
        plainTextConsole = new QPlainTextEdit(tabConsole);
        plainTextConsole->setObjectName("plainTextConsole");
        plainTextConsole->setGeometry(QRect(0, 0, 351, 331));
        QPalette palette32;
        QBrush brush7(QColor(255, 255, 255, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette32.setBrush(QPalette::Active, QPalette::WindowText, brush7);
        palette32.setBrush(QPalette::Active, QPalette::Text, brush7);
        QBrush brush8(QColor(30, 30, 30, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette32.setBrush(QPalette::Active, QPalette::Base, brush8);
        palette32.setBrush(QPalette::Inactive, QPalette::WindowText, brush7);
        palette32.setBrush(QPalette::Inactive, QPalette::Text, brush7);
        palette32.setBrush(QPalette::Inactive, QPalette::Base, brush8);
        plainTextConsole->setPalette(palette32);
        plainTextConsole->setFont(font1);
        plainTextConsole->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        plainTextConsole->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        plainTextConsole->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextConsole->setReadOnly(true);
        tabWidget->addTab(tabConsole, QString());
        tabConversion = new QWidget();
        tabConversion->setObjectName("tabConversion");
        lineEditBin = new QLineEdit(tabConversion);
        lineEditBin->setObjectName("lineEditBin");
        lineEditBin->setGeometry(QRect(110, 40, 241, 24));
        label_3 = new QLabel(tabConversion);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 40, 101, 21));
        label_4 = new QLabel(tabConversion);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 70, 101, 21));
        lineEditOct = new QLineEdit(tabConversion);
        lineEditOct->setObjectName("lineEditOct");
        lineEditOct->setGeometry(QRect(110, 70, 241, 24));
        label_5 = new QLabel(tabConversion);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(10, 100, 101, 21));
        lineEditHex = new QLineEdit(tabConversion);
        lineEditHex->setObjectName("lineEditHex");
        lineEditHex->setGeometry(QRect(110, 100, 241, 24));
        label_6 = new QLabel(tabConversion);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(10, 130, 101, 21));
        lineEditDec = new QLineEdit(tabConversion);
        lineEditDec->setObjectName("lineEditDec");
        lineEditDec->setGeometry(QRect(110, 130, 241, 24));
        comboBoxBreakWhen = new QComboBox(tabConversion);
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->addItem(QString());
        comboBoxBreakWhen->setObjectName("comboBoxBreakWhen");
        comboBoxBreakWhen->setGeometry(QRect(100, 190, 131, 31));
        label_7 = new QLabel(tabConversion);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(10, 10, 341, 20));
        label_7->setAlignment(Qt::AlignCenter);
        label_8 = new QLabel(tabConversion);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 170, 341, 20));
        label_8->setAlignment(Qt::AlignCenter);
        label_9 = new QLabel(tabConversion);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(10, 190, 81, 31));
        label_10 = new QLabel(tabConversion);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(70, 230, 21, 21));
        spinBoxBreakIs = new QSpinBox(tabConversion);
        spinBoxBreakIs->setObjectName("spinBoxBreakIs");
        spinBoxBreakIs->setGeometry(QRect(100, 230, 131, 25));
        labelAt = new QLabel(tabConversion);
        labelAt->setObjectName("labelAt");
        labelAt->setGeometry(QRect(240, 190, 21, 31));
        spinBoxBreakAt = new QSpinBox(tabConversion);
        spinBoxBreakAt->setObjectName("spinBoxBreakAt");
        spinBoxBreakAt->setGeometry(QRect(260, 190, 91, 31));
        spinBoxBreakAt->setMaximum(65535);
        tabWidget->addTab(tabConversion, QString());
        tabSettings = new QWidget();
        tabSettings->setObjectName("tabSettings");
        checkBoxError = new QCheckBox(tabSettings);
        checkBoxError->setObjectName("checkBoxError");
        checkBoxError->setGeometry(QRect(10, 10, 158, 20));
        checkBoxError->setChecked(true);
        checkBoxError->setTristate(false);
        checkBoxWarn = new QCheckBox(tabSettings);
        checkBoxWarn->setObjectName("checkBoxWarn");
        checkBoxWarn->setGeometry(QRect(10, 30, 158, 20));
        checkBoxWarn->setChecked(true);
        checkBoxDebug = new QCheckBox(tabSettings);
        checkBoxDebug->setObjectName("checkBoxDebug");
        checkBoxDebug->setGeometry(QRect(10, 50, 158, 20));
        checkBoxDebug->setChecked(true);
        checkBoxAdvancedInfo = new QCheckBox(tabSettings);
        checkBoxAdvancedInfo->setObjectName("checkBoxAdvancedInfo");
        checkBoxAdvancedInfo->setGeometry(QRect(10, 90, 241, 22));
        checkBoxHexRegs = new QCheckBox(tabSettings);
        checkBoxHexRegs->setObjectName("checkBoxHexRegs");
        checkBoxHexRegs->setGeometry(QRect(10, 110, 241, 22));
        checkBoxHexRegs->setChecked(true);
        checkBoxCompileOnRun = new QCheckBox(tabSettings);
        checkBoxCompileOnRun->setObjectName("checkBoxCompileOnRun");
        checkBoxCompileOnRun->setGeometry(QRect(10, 130, 241, 22));
        checkBoxCompileOnRun->setChecked(true);
        spinBoxLow = new QSpinBox(tabSettings);
        spinBoxLow->setObjectName("spinBoxLow");
        spinBoxLow->setGeometry(QRect(299, 210, 42, 25));
        spinBoxLow->setMinimum(1);
        spinBoxLow->setMaximum(34);
        spinBoxLow->setValue(5);
        spinBoxUp = new QSpinBox(tabSettings);
        spinBoxUp->setObjectName("spinBoxUp");
        spinBoxUp->setGeometry(QRect(300, 240, 41, 25));
        spinBoxUp->setMinimum(2);
        spinBoxUp->setMaximum(35);
        spinBoxUp->setValue(20);
        label = new QLabel(tabSettings);
        label->setObjectName("label");
        label->setGeometry(QRect(9, 210, 281, 21));
        label_2 = new QLabel(tabSettings);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(9, 240, 281, 21));
        checkBoxWriteMemory = new QCheckBox(tabSettings);
        checkBoxWriteMemory->setObjectName("checkBoxWriteMemory");
        checkBoxWriteMemory->setGeometry(QRect(10, 270, 341, 21));
        checkBoxAutoReset = new QCheckBox(tabSettings);
        checkBoxAutoReset->setObjectName("checkBoxAutoReset");
        checkBoxAutoReset->setGeometry(QRect(10, 150, 241, 22));
        checkBoxAutoReset->setChecked(true);
        checkBoxSimpleMemory = new QCheckBox(tabSettings);
        checkBoxSimpleMemory->setObjectName("checkBoxSimpleMemory");
        checkBoxSimpleMemory->setGeometry(QRect(10, 180, 241, 22));
        checkBoxSimpleMemory->setChecked(false);
        tabWidget->addTab(tabSettings, QString());
        Info = new QWidget();
        Info->setObjectName("Info");
        plainTextInfo = new QTextEdit(Info);
        plainTextInfo->setObjectName("plainTextInfo");
        plainTextInfo->setGeometry(QRect(10, 10, 341, 311));
        plainTextInfo->setReadOnly(true);
        plainTextInfo->setTabStopDistance(20.000000000000000);
        tabWidget->addTab(Info, QString());
        opCodes = new QWidget();
        opCodes->setObjectName("opCodes");
        opCodes->setEnabled(false);
        treeWidget = new QTreeWidget(opCodes);
        QBrush brush9(QColor(0, 0, 0, 255));
        brush9.setStyle(Qt::NoBrush);
        QBrush brush10(QColor(0, 0, 0, 255));
        brush10.setStyle(Qt::NoBrush);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(treeWidget);
        __qtreewidgetitem->setBackground(0, brush9);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(treeWidget);
        __qtreewidgetitem1->setBackground(2, brush10);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        new QTreeWidgetItem(treeWidget);
        treeWidget->setObjectName("treeWidget");
        treeWidget->setGeometry(QRect(10, 30, 256, 192));
        QPalette palette33;
        QBrush brush11(QColor(198, 198, 198, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette33.setBrush(QPalette::Active, QPalette::AlternateBase, brush11);
        palette33.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush11);
        palette33.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush11);
        treeWidget->setPalette(palette33);
        treeWidget->setUniformRowHeights(true);
        treeWidget->setItemsExpandable(false);
        treeWidget->setSortingEnabled(false);
        treeWidget->setWordWrap(true);
        treeWidget->header()->setCascadingSectionResizes(false);
        treeWidget->header()->setMinimumSectionSize(30);
        treeWidget->header()->setDefaultSectionSize(150);
        treeWidget->header()->setHighlightSections(false);
        tabWidget->addTab(opCodes, QString());
        comboBoxSpeedSelector = new QComboBox(centralwidget);
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->addItem(QString());
        comboBoxSpeedSelector->setObjectName("comboBoxSpeedSelector");
        comboBoxSpeedSelector->setGeometry(QRect(910, 670, 101, 24));
        comboBoxSpeedSelector->setMaxVisibleItems(11);
        frameDisplay = new QFrame(centralwidget);
        frameDisplay->setObjectName("frameDisplay");
        frameDisplay->setEnabled(false);
        frameDisplay->setGeometry(QRect(770, 500, 16, 16));
        QPalette palette34;
        QBrush brush12(QColor(85, 0, 0, 255));
        brush12.setStyle(Qt::SolidPattern);
        palette34.setBrush(QPalette::Active, QPalette::Base, brush12);
        palette34.setBrush(QPalette::Inactive, QPalette::Base, brush12);
        frameDisplay->setPalette(palette34);
        frameDisplay->setFrameShape(QFrame::StyledPanel);
        frameDisplay->setFrameShadow(QFrame::Raised);
        plainTextDisplay = new QPlainTextEdit(frameDisplay);
        plainTextDisplay->setObjectName("plainTextDisplay");
        plainTextDisplay->setEnabled(false);
        plainTextDisplay->setGeometry(QRect(0, 0, 120, 80));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plainTextDisplay->sizePolicy().hasHeightForWidth());
        plainTextDisplay->setSizePolicy(sizePolicy);
        QPalette palette35;
        palette35.setBrush(QPalette::Active, QPalette::Text, brush7);
        QBrush brush13(QColor(0, 0, 0, 255));
        brush13.setStyle(Qt::SolidPattern);
        palette35.setBrush(QPalette::Active, QPalette::Base, brush13);
        palette35.setBrush(QPalette::Inactive, QPalette::Text, brush7);
        palette35.setBrush(QPalette::Inactive, QPalette::Base, brush13);
        plainTextDisplay->setPalette(palette35);
        plainTextDisplay->setFont(font1);
        plainTextDisplay->setAcceptDrops(false);
        plainTextDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextDisplay->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        plainTextDisplay->setUndoRedoEnabled(false);
        plainTextDisplay->setLineWrapMode(QPlainTextEdit::NoWrap);
        plainTextDisplay->setReadOnly(true);
        comboBoxVersionSelector = new QComboBox(centralwidget);
        comboBoxVersionSelector->addItem(QString());
        comboBoxVersionSelector->addItem(QString());
        comboBoxVersionSelector->setObjectName("comboBoxVersionSelector");
        comboBoxVersionSelector->setGeometry(QRect(100, 670, 72, 24));
        buttonFSDisplay = new QPushButton(centralwidget);
        buttonFSDisplay->setObjectName("buttonFSDisplay");
        buttonFSDisplay->setEnabled(false);
        buttonFSDisplay->setGeometry(QRect(1129, 670, 140, 24));
        QPalette palette36;
        palette36.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette36.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette36.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        buttonFSDisplay->setPalette(palette36);
        buttonSwitchWrite = new QPushButton(centralwidget);
        buttonSwitchWrite->setObjectName("buttonSwitchWrite");
        buttonSwitchWrite->setEnabled(false);
        buttonSwitchWrite->setGeometry(QRect(410, 670, 151, 24));
        labelWritingMode = new QLabel(centralwidget);
        labelWritingMode->setObjectName("labelWritingMode");
        labelWritingMode->setEnabled(false);
        labelWritingMode->setGeometry(QRect(558, 670, 51, 21));
        labelWritingMode->setAlignment(Qt::AlignCenter);
        groupBoxSimpleMemory = new QGroupBox(centralwidget);
        groupBoxSimpleMemory->setObjectName("groupBoxSimpleMemory");
        groupBoxSimpleMemory->setEnabled(false);
        groupBoxSimpleMemory->setGeometry(QRect(400, 10, 501, 651));
        QPalette palette37;
        QBrush brush14(QColor(161, 161, 161, 255));
        brush14.setStyle(Qt::SolidPattern);
        palette37.setBrush(QPalette::Active, QPalette::Base, brush14);
        palette37.setBrush(QPalette::Inactive, QPalette::Base, brush14);
        groupBoxSimpleMemory->setPalette(palette37);
        groupBoxSimpleMemory->setAlignment(Qt::AlignCenter);
        groupBoxSimpleMemory->setFlat(false);
        tableWidgetSM = new QTableWidget(groupBoxSimpleMemory);
        if (tableWidgetSM->columnCount() < 2)
            tableWidgetSM->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetSM->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetSM->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tableWidgetSM->rowCount() < 20)
            tableWidgetSM->setRowCount(20);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(2, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(3, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(4, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(5, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(6, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(7, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(8, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(9, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(10, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(11, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(12, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(13, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(14, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(15, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(16, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(17, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(18, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tableWidgetSM->setVerticalHeaderItem(19, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tableWidgetSM->setItem(19, 0, __qtablewidgetitem22);
        tableWidgetSM->setObjectName("tableWidgetSM");
        tableWidgetSM->setGeometry(QRect(130, 20, 241, 625));
        tableWidgetSM->setFont(font2);
        tableWidgetSM->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableWidgetSM->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableWidgetSM->horizontalHeader()->setVisible(true);
        tableWidgetSM->horizontalHeader()->setDefaultSectionSize(120);
        tableWidgetSM->verticalHeader()->setVisible(false);
        spinBox = new QSpinBox(groupBoxSimpleMemory);
        spinBox->setObjectName("spinBox");
        spinBox->setGeometry(QRect(40, 19, 81, 31));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("Consolas")});
        font4.setPointSize(16);
        font4.setBold(true);
        spinBox->setFont(font4);
        spinBox->setMaximum(65516);
        spinBox->setDisplayIntegerBase(16);
        tableWidget = new QTableWidget(centralwidget);
        if (tableWidget->columnCount() < 7)
            tableWidget->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem29);
        if (tableWidget->rowCount() < 31)
            tableWidget->setRowCount(31);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(2, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(3, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(4, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(5, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(6, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(7, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(8, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(9, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(10, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(11, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(12, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(13, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(14, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(15, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(16, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(17, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(18, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(19, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(20, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(21, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(22, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(23, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(24, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(25, __qtablewidgetitem55);
        QTableWidgetItem *__qtablewidgetitem56 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(26, __qtablewidgetitem56);
        QTableWidgetItem *__qtablewidgetitem57 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(27, __qtablewidgetitem57);
        QTableWidgetItem *__qtablewidgetitem58 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(28, __qtablewidgetitem58);
        QTableWidgetItem *__qtablewidgetitem59 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(29, __qtablewidgetitem59);
        QTableWidgetItem *__qtablewidgetitem60 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(30, __qtablewidgetitem60);
        QTableWidgetItem *__qtablewidgetitem61 = new QTableWidgetItem();
        __qtablewidgetitem61->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        tableWidget->setItem(0, 0, __qtablewidgetitem61);
        QTableWidgetItem *__qtablewidgetitem62 = new QTableWidgetItem();
        tableWidget->setItem(0, 1, __qtablewidgetitem62);
        QTableWidgetItem *__qtablewidgetitem63 = new QTableWidgetItem();
        __qtablewidgetitem63->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        tableWidget->setItem(0, 2, __qtablewidgetitem63);
        QTableWidgetItem *__qtablewidgetitem64 = new QTableWidgetItem();
        tableWidget->setItem(0, 5, __qtablewidgetitem64);
        QTableWidgetItem *__qtablewidgetitem65 = new QTableWidgetItem();
        tableWidget->setItem(1, 0, __qtablewidgetitem65);
        QTableWidgetItem *__qtablewidgetitem66 = new QTableWidgetItem();
        tableWidget->setItem(1, 1, __qtablewidgetitem66);
        QTableWidgetItem *__qtablewidgetitem67 = new QTableWidgetItem();
        tableWidget->setItem(1, 2, __qtablewidgetitem67);
        QTableWidgetItem *__qtablewidgetitem68 = new QTableWidgetItem();
        tableWidget->setItem(1, 3, __qtablewidgetitem68);
        QTableWidgetItem *__qtablewidgetitem69 = new QTableWidgetItem();
        tableWidget->setItem(1, 4, __qtablewidgetitem69);
        QTableWidgetItem *__qtablewidgetitem70 = new QTableWidgetItem();
        tableWidget->setItem(2, 0, __qtablewidgetitem70);
        QTableWidgetItem *__qtablewidgetitem71 = new QTableWidgetItem();
        tableWidget->setItem(2, 1, __qtablewidgetitem71);
        QTableWidgetItem *__qtablewidgetitem72 = new QTableWidgetItem();
        tableWidget->setItem(2, 2, __qtablewidgetitem72);
        QTableWidgetItem *__qtablewidgetitem73 = new QTableWidgetItem();
        tableWidget->setItem(2, 3, __qtablewidgetitem73);
        QTableWidgetItem *__qtablewidgetitem74 = new QTableWidgetItem();
        tableWidget->setItem(2, 4, __qtablewidgetitem74);
        QTableWidgetItem *__qtablewidgetitem75 = new QTableWidgetItem();
        tableWidget->setItem(3, 0, __qtablewidgetitem75);
        QTableWidgetItem *__qtablewidgetitem76 = new QTableWidgetItem();
        tableWidget->setItem(4, 0, __qtablewidgetitem76);
        QTableWidgetItem *__qtablewidgetitem77 = new QTableWidgetItem();
        tableWidget->setItem(5, 0, __qtablewidgetitem77);
        QTableWidgetItem *__qtablewidgetitem78 = new QTableWidgetItem();
        tableWidget->setItem(6, 0, __qtablewidgetitem78);
        QTableWidgetItem *__qtablewidgetitem79 = new QTableWidgetItem();
        tableWidget->setItem(7, 0, __qtablewidgetitem79);
        QTableWidgetItem *__qtablewidgetitem80 = new QTableWidgetItem();
        tableWidget->setItem(8, 0, __qtablewidgetitem80);
        QTableWidgetItem *__qtablewidgetitem81 = new QTableWidgetItem();
        tableWidget->setItem(9, 0, __qtablewidgetitem81);
        QTableWidgetItem *__qtablewidgetitem82 = new QTableWidgetItem();
        tableWidget->setItem(10, 0, __qtablewidgetitem82);
        QTableWidgetItem *__qtablewidgetitem83 = new QTableWidgetItem();
        tableWidget->setItem(11, 0, __qtablewidgetitem83);
        QTableWidgetItem *__qtablewidgetitem84 = new QTableWidgetItem();
        tableWidget->setItem(12, 0, __qtablewidgetitem84);
        QTableWidgetItem *__qtablewidgetitem85 = new QTableWidgetItem();
        tableWidget->setItem(13, 0, __qtablewidgetitem85);
        QTableWidgetItem *__qtablewidgetitem86 = new QTableWidgetItem();
        tableWidget->setItem(14, 0, __qtablewidgetitem86);
        QTableWidgetItem *__qtablewidgetitem87 = new QTableWidgetItem();
        tableWidget->setItem(15, 0, __qtablewidgetitem87);
        QTableWidgetItem *__qtablewidgetitem88 = new QTableWidgetItem();
        tableWidget->setItem(16, 0, __qtablewidgetitem88);
        QTableWidgetItem *__qtablewidgetitem89 = new QTableWidgetItem();
        tableWidget->setItem(17, 0, __qtablewidgetitem89);
        QTableWidgetItem *__qtablewidgetitem90 = new QTableWidgetItem();
        tableWidget->setItem(18, 0, __qtablewidgetitem90);
        QTableWidgetItem *__qtablewidgetitem91 = new QTableWidgetItem();
        tableWidget->setItem(19, 0, __qtablewidgetitem91);
        QTableWidgetItem *__qtablewidgetitem92 = new QTableWidgetItem();
        tableWidget->setItem(20, 0, __qtablewidgetitem92);
        QTableWidgetItem *__qtablewidgetitem93 = new QTableWidgetItem();
        tableWidget->setItem(21, 0, __qtablewidgetitem93);
        QTableWidgetItem *__qtablewidgetitem94 = new QTableWidgetItem();
        tableWidget->setItem(22, 0, __qtablewidgetitem94);
        QTableWidgetItem *__qtablewidgetitem95 = new QTableWidgetItem();
        tableWidget->setItem(23, 0, __qtablewidgetitem95);
        QTableWidgetItem *__qtablewidgetitem96 = new QTableWidgetItem();
        tableWidget->setItem(24, 0, __qtablewidgetitem96);
        QTableWidgetItem *__qtablewidgetitem97 = new QTableWidgetItem();
        tableWidget->setItem(25, 0, __qtablewidgetitem97);
        QTableWidgetItem *__qtablewidgetitem98 = new QTableWidgetItem();
        tableWidget->setItem(26, 0, __qtablewidgetitem98);
        QTableWidgetItem *__qtablewidgetitem99 = new QTableWidgetItem();
        tableWidget->setItem(27, 0, __qtablewidgetitem99);
        QTableWidgetItem *__qtablewidgetitem100 = new QTableWidgetItem();
        tableWidget->setItem(28, 0, __qtablewidgetitem100);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setEnabled(false);
        tableWidget->setGeometry(QRect(200, 280, 16, 16));
        tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setTabKeyNavigation(false);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setTextElideMode(Qt::ElideMiddle);
        tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableWidget->horizontalHeader()->setVisible(true);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(false);
        tableWidget->horizontalHeader()->setMinimumSectionSize(40);
        tableWidget->horizontalHeader()->setDefaultSectionSize(80);
        tableWidget->horizontalHeader()->setHighlightSections(true);
        tableWidget->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        tableWidget->horizontalHeader()->setStretchLastSection(false);
        tableWidget->verticalHeader()->setVisible(true);
        tableWidget->verticalHeader()->setCascadingSectionResizes(true);
        tableWidget->verticalHeader()->setProperty("showSortIndicator", QVariant(true));
        tableWidget->verticalHeader()->setStretchLastSection(true);
        MainWindow->setCentralWidget(centralwidget);
        tableWidget->raise();
        frameDisplay->raise();
        plainTextCode->raise();
        plainTextLines->raise();
        buttonCompile->raise();
        buttonLoad->raise();
        buttonSave->raise();
        buttonStep->raise();
        buttonRunStop->raise();
        buttonReset->raise();
        groupBox->raise();
        tabWidget->raise();
        comboBoxSpeedSelector->raise();
        plainTextMemory->raise();
        comboBoxVersionSelector->raise();
        buttonFSDisplay->raise();
        buttonSwitchWrite->raise();
        labelWritingMode->raise();
        groupBoxSimpleMemory->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1280, 19));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);
        comboBoxSpeedSelector->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        plainTextCode->setPlainText(QString());
        plainTextLines->setPlainText(QCoreApplication::translate("MainWindow", "line:", nullptr));
        buttonCompile->setText(QCoreApplication::translate("MainWindow", "Compile", nullptr));
        buttonLoad->setText(QCoreApplication::translate("MainWindow", "Load", nullptr));
        buttonSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        plainTextMemory->setPlainText(QCoreApplication::translate("MainWindow", "0000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01", nullptr));
        buttonStep->setText(QCoreApplication::translate("MainWindow", "Step", nullptr));
        buttonRunStop->setText(QCoreApplication::translate("MainWindow", "Run/Stop", nullptr));
        buttonReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Registers:", nullptr));
        lineEditAValue->setText(QCoreApplication::translate("MainWindow", "00", nullptr));
        lineEditAName->setText(QCoreApplication::translate("MainWindow", "A:", nullptr));
        lineEditBName->setText(QCoreApplication::translate("MainWindow", "B:", nullptr));
        lineEditBValue->setText(QCoreApplication::translate("MainWindow", "00", nullptr));
        lineEditXName->setText(QCoreApplication::translate("MainWindow", "X:", nullptr));
        lineEditXValue->setText(QCoreApplication::translate("MainWindow", "0000", nullptr));
        lineEditSPValue->setText(QCoreApplication::translate("MainWindow", "0000", nullptr));
        lineEditSName->setText(QCoreApplication::translate("MainWindow", "SP:", nullptr));
        lineEditPCValue->setText(QCoreApplication::translate("MainWindow", "0000", nullptr));
        lineEditPCName->setText(QCoreApplication::translate("MainWindow", "PC:", nullptr));
        lineEditNName->setText(QCoreApplication::translate("MainWindow", "N", nullptr));
        lineEditNValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEditZName->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        lineEditZValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEditVName->setText(QCoreApplication::translate("MainWindow", "V", nullptr));
        lineEditVValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEditCName->setText(QCoreApplication::translate("MainWindow", "C", nullptr));
        lineEditCValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEditIValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEditIName->setText(QCoreApplication::translate("MainWindow", "I", nullptr));
        lineEditHValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        lineEditHName->setText(QCoreApplication::translate("MainWindow", "H", nullptr));
        labelRunningIndicatior->setText(QCoreApplication::translate("MainWindow", "Operation/second:", nullptr));
        plainTextConsole->setPlainText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tabConsole), QCoreApplication::translate("MainWindow", "Console", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Binary:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Octal:", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Hexadecimal:", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Decimal:", nullptr));
        comboBoxBreakWhen->setItemText(0, QCoreApplication::translate("MainWindow", "No break", nullptr));
        comboBoxBreakWhen->setItemText(1, QCoreApplication::translate("MainWindow", "Line", nullptr));
        comboBoxBreakWhen->setItemText(2, QCoreApplication::translate("MainWindow", "PC", nullptr));
        comboBoxBreakWhen->setItemText(3, QCoreApplication::translate("MainWindow", "SP", nullptr));
        comboBoxBreakWhen->setItemText(4, QCoreApplication::translate("MainWindow", "X register", nullptr));
        comboBoxBreakWhen->setItemText(5, QCoreApplication::translate("MainWindow", "accumulator A", nullptr));
        comboBoxBreakWhen->setItemText(6, QCoreApplication::translate("MainWindow", "accumulator B", nullptr));
        comboBoxBreakWhen->setItemText(7, QCoreApplication::translate("MainWindow", "flag H", nullptr));
        comboBoxBreakWhen->setItemText(8, QCoreApplication::translate("MainWindow", "flag I", nullptr));
        comboBoxBreakWhen->setItemText(9, QCoreApplication::translate("MainWindow", "flag N", nullptr));
        comboBoxBreakWhen->setItemText(10, QCoreApplication::translate("MainWindow", "flag Z", nullptr));
        comboBoxBreakWhen->setItemText(11, QCoreApplication::translate("MainWindow", "flag V", nullptr));
        comboBoxBreakWhen->setItemText(12, QCoreApplication::translate("MainWindow", "flag C", nullptr));
        comboBoxBreakWhen->setItemText(13, QCoreApplication::translate("MainWindow", "Memory value", nullptr));

        label_7->setText(QCoreApplication::translate("MainWindow", "Number converter:", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Break execution:", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Break when:", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "is", nullptr));
        labelAt->setText(QCoreApplication::translate("MainWindow", "at", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabConversion), QCoreApplication::translate("MainWindow", "Debug tools", nullptr));
        checkBoxError->setText(QCoreApplication::translate("MainWindow", "Show errors", nullptr));
        checkBoxWarn->setText(QCoreApplication::translate("MainWindow", "Show warnings", nullptr));
        checkBoxDebug->setText(QCoreApplication::translate("MainWindow", "Show debug", nullptr));
        checkBoxAdvancedInfo->setText(QCoreApplication::translate("MainWindow", "Advanced code info", nullptr));
        checkBoxHexRegs->setText(QCoreApplication::translate("MainWindow", "Show values of registers in hex", nullptr));
        checkBoxCompileOnRun->setText(QCoreApplication::translate("MainWindow", "Compile on run", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Scroll up when current line is above:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Scroll down when current line is below:", nullptr));
        checkBoxWriteMemory->setText(QCoreApplication::translate("MainWindow", "Enable manual writing to Memory", nullptr));
        checkBoxAutoReset->setText(QCoreApplication::translate("MainWindow", "Auto reset", nullptr));
        checkBoxSimpleMemory->setText(QCoreApplication::translate("MainWindow", "Simple memory", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSettings), QCoreApplication::translate("MainWindow", "Settings", nullptr));
        plainTextInfo->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Consolas'; font-size:9pt; font-weight:700; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00007f;\">Assembly Statement Format:</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; color:#00007f;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><"
                        "span style=\" font-size:10pt; font-weight:400;\">Assembly statements contain the following fields:</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">[Label] *tab* Instruction *space* [operand] *;* [comment]</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Label:</span><span style=\" font-size:10pt; font-weight:400;\"> Can be used to define a symbol, to skip the field use a tab. "
                        "Label must beggin with a letter and can contain letters numbers or *_*;</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Instruction/Operation:</span><span style=\" font-size:10pt; font-weight:400;\"> Must be preceded with a tab. Defines the opcode or directive. Opcodes are not case-sensitive.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Operand:</span><span style=\" font-size:10pt; font-weight:400;\"> Contains an address or data.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Comment:</span><span style=\" font-size:10pt; font-weight:400;\"> Optional and used for software documentation. Preceded by a semicolon</span></p>\n"
"<p style=\"-qt-paragraph-t"
                        "ype:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00007f;\">Compiler instructions:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">.ORG - sets the current address of compilation to its operand.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">.BYTE - sets the current address of "
                        "compilation to its operand. The label will be the location of the set byte.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">.EQU - sets its label to the value of the operand. (calling LABEL .ORG 56 will return 56)</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00007f;\">Emulator functionality:</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bo"
                        "ttom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Lines tab:</span><span style=\" font-size:10pt; font-weight:400;\"> On the most left of the window. Displays the line number and the hexadecimal address of the line's instruction(when compiled), which are separated by a colon. If the setting \342\200\234Advanced code info\342\200\235 is ticked, the lines tab will also display opcodes and operands. If the current code is compiled you will also be able to click on a line and mark it and its address in the memory with a green background. To unmark a line use the right click mouse button.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br "
                        "/></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Code tab:</span><span style=\" font-size:10pt; font-weight:400;\"> Next to the lines tab. Click on the tab to be able to write assembly code. For the syntax, refer to the assembly statement format section above.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Memory tab:</span><span style=\" font-size:10pt; font-weight:400;\"> Next to the code tab. </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">When th"
                        "e &quot;Simple memory&quot; setting is disabled, the tab will display 16 memory locations per line. Each one of these memory cells contains the data at that memory address in hex. </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">When the &quot;Simple memory&quot; setting is enabled, the tab will display 20 memory locations per page in a 2x20 grid. The address is displayed on the left and the content of the memory location is displayed on the right.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Multiple tab selection:</span><span style=\" font-size:10pt; font-weigh"
                        "t:400;\"> On the bottom right.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Console:</span><span style=\" font-size:10pt; font-weight:400;\"> Displays errors, debugging and warnings based on your preferences in the settings tab.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Debug tools:</span><span style=\" font-size:10pt; font-weight:400;\"> Contains a number converter(dec, hex, oct, bin) and a break function that stops the program's automatic execution when a specific event occurs.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Settings:</span><span style=\" font-size:10pt; font-weight:400;\"> Contains optional features that can be changed by the"
                        " user:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Show errors: Display errors in the console tab.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Show warnings: Display warnings in the console tab.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Show debug: Display debugging information in the console tab(mostly in case of suspecting an emulator bug).</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px;"
                        " margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Advanced lines info: Show the hex machine code for each instruction in the lines tab.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Show values of registers in hex: If toggled registers will display contents in hexadecimal.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Compile on run: If toggled the compile button will become unnecessary.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Auto reset: If the contents of memory at the address of PC is 0 then reset the emulator a"
                        "s soon as Run/Stop is pressed.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Simple memory: Replaces the 16 memory addresses per line with a simple table of 20 addresses per page. This representation does not have all the features of the original and does not support manual editing.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Enable manual writing to Memory: Enable a button that enables switching writing modes:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><s"
                        "pan style=\" font-size:10pt; font-weight:400;\">		Click the button labelled &quot;Switch writing mode&quot; to change the writing mode. The label next to the button shows the current active mode:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">		Memory: Click on the memory tab to set focus to it and then use the arrow keys to select memory cells. After selecting a memory cell type a hex number(characters from 0-9 and A-F) to replace the contents of memory at that address. During &quot;memory&quot; mode you will not be able to compile or change the assembly code.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">		Code: This is the default mode. You are unable to change the contents of memory manually.</span></p>\n"
"<p style=\" margin-top:0p"
                        "x; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Scroll up when the current line is above: If the current line of the next instruction is above the current X line relative to the current scroll(if the first line visible is 6 then the decisive line is (6 + X)) the code tab will automatically scroll up.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Scroll down when the current line is below: If the current line of the next instruction is below the current X line relative to the current scroll(if the first line visible is 6 then the decisive line is (6 + X)) the code tab will automatically scroll down.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:"
                        "400;\">Info: Displays this page with information about the emulators features and functionality.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Registers:</span><span style=\" font-size:10pt; font-weight:400;\"> In the top right. Displays registers, accumulators and status flags.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Display tab:</span><span style=\" font-size:10pt; font-weight:400"
                        ";\"> Expand the window horizontally to show the display tab. The display uses a buffer to display ASCII characters. The display also receives keyboard input, mouse location(x and y of the character that the mouse is currently hovering over) and mouse input. When the display and its input buffers are active it will be outlined in blue.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Buttons and selectors:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Compile button to compile the script inside of the script editor tab using the selected version of the Motorola MCU"
                        ". This button will have a red outline if the code is uncompiled or has changed since the last compilation and a green outline if the code is currently compiled.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Version selection button to change the version of the processor. Choose between m6800 and mc6803(same instruction set as m6801)</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Load button to load a script from a local file.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Save button to save the script to a local file.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; "
                        "margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Reset button to reset the emulator. Sets all registers to default value, and returns memory to previously compiled machine code.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Step button to execute the instruction in memory at the current address of PC. If the instruction in memory at the current address of PC is unknown PC will be incremented by one and a warning will be displayed in the console tab.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Run/Stop button to run or pause the execution of instructions at x per second speed. Execution will stop if the content of memory at the address of PC is 0.</span></p>\n"
"<p style=\" "
                        "margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	Speed selection menu to select the instruction per second speed.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">	</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00007f;\">Buffers and interrupt pointers:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Display buffer:</span><span style=\" font-size:10pt; font-weight:400;\"> The Display Buffer spans memory addresses 0xFB00 to 0xFF37 inclusively. This memory region functions as a dynamic visual representation for the system. As"
                        " data changes within this range, the display instantaneously updates, reflecting alterations in real-time using the standard ASCII encoding.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Keyboard buffer:</span><span style=\" font-size:10pt; font-weight:400;\"> When the display tab is active (outlined in blue), it becomes the recipient of keyboard input. This input is then used to update the content of memory address 0xFFF0 with the corresponding keycode, following the standard ASCII encoding.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Mouse key buffer:</span><span style=\" font-size:10pt; font-weight:400;\"> If the display tab is in focus address 0xFFF1 will be set to 1 when left mouse button is pressed. It will be set to 2 when right mouse button is pressed. It wil"
                        "l be set to 3 when middle mouse button is pressed. It will be set to 4 when left mouse button is released. It will be set to 5 when right mouse button is released. It will be set to 6 when middle mouse button is released. This will happen instantaneously and will not wait for the next instruction.</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Mouse location buffer:</span><span style=\" font-size:10pt; font-weight:400;\"> The position of the mouse pointer relative to the display tab is denoted by its X and Y coordinates, corresponding to the cell/character over which the mouse hovers. Before executing a pending instruction, if the display tab is in focus, the content of memory address 0xFFF2 will be set to the X coordinate, and the content of memory address 0xFFF3 will be set to the Y coordinate. </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-"
                        "left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Interrupt pointers:</span><span style=\" font-size:10pt; font-weight:400;\"> A block of memory is reserved for pointers, which provide for read-only storage of the addresses of programs that are to be executed in the event of a reset (or power on), a low state of the Non-Maskable Interrupt control input, a software interrupt, or a response to an interrupt signal from a peripheral device. The respective pointers each occupy two bytes of memory and are disposed at locations from &quot;n - 7&quot;(0xFFF8) to &quot;n&quot;(0xFFFF).</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">Internal interrupt pointer spans fro"
                        "m n-7 to n-6. n-7 contains the most significant byte of the address. n-6 contains the least significant byte of the address. *NOT IMPLEMENTED*</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">Software interrupt pointer spans from n-5 to n-4. (called with the SWI instruction) </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">Non-maskable interrupt pointer spans from n-3 to n-2. *NOT IMPLEMENTED* </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">Reset pointer spans from n-1 to n-0. *NOT IMPLEMENTED* </span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin"
                        "-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">The WAI instruction will not act as an interrupt because there is already a buffer for that. That instruction will only suspend the execution of the program until the display tab receives a keyboard input(0xFFF0 changes).</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00007f;\">Credits:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span styl"
                        "e=\" font-size:10pt; font-weight:400;\">Vladimir Janu\305\241.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">Made in August 2023.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:400;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:400;\">Current version: 1.3.2</span></p></body></html>", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(Info), QCoreApplication::translate("MainWindow", "Info", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(8, QCoreApplication::translate("MainWindow", "flags", nullptr));
        ___qtreewidgetitem->setText(7, QCoreApplication::translate("MainWindow", "REL", nullptr));
        ___qtreewidgetitem->setText(6, QCoreApplication::translate("MainWindow", "IND", nullptr));
        ___qtreewidgetitem->setText(5, QCoreApplication::translate("MainWindow", "EXT", nullptr));
        ___qtreewidgetitem->setText(4, QCoreApplication::translate("MainWindow", "DIR", nullptr));
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "IMM", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "INH", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Description", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Instruction", nullptr));

        const bool __sortingEnabled = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(8, QCoreApplication::translate("MainWindow", "*-****", nullptr));
        ___qtreewidgetitem1->setText(2, QCoreApplication::translate("MainWindow", "1B", nullptr));
        ___qtreewidgetitem1->setText(1, QCoreApplication::translate("MainWindow", "ACCA <- (ACCA) + (ACCB)", nullptr));
        ___qtreewidgetitem1->setText(0, QCoreApplication::translate("MainWindow", "ABA", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget->topLevelItem(1);
        ___qtreewidgetitem2->setText(8, QCoreApplication::translate("MainWindow", "*-****", nullptr));
        ___qtreewidgetitem2->setText(6, QCoreApplication::translate("MainWindow", "A9", nullptr));
        ___qtreewidgetitem2->setText(5, QCoreApplication::translate("MainWindow", "B9", nullptr));
        ___qtreewidgetitem2->setText(4, QCoreApplication::translate("MainWindow", "99", nullptr));
        ___qtreewidgetitem2->setText(3, QCoreApplication::translate("MainWindow", "89", nullptr));
        ___qtreewidgetitem2->setText(1, QCoreApplication::translate("MainWindow", "ACCA <- (ACCA) + (M) + (C)", nullptr));
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("MainWindow", "ADCA", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = treeWidget->topLevelItem(2);
        ___qtreewidgetitem3->setText(8, QCoreApplication::translate("MainWindow", "*-****", nullptr));
        ___qtreewidgetitem3->setText(6, QCoreApplication::translate("MainWindow", "E9", nullptr));
        ___qtreewidgetitem3->setText(5, QCoreApplication::translate("MainWindow", "F9", nullptr));
        ___qtreewidgetitem3->setText(4, QCoreApplication::translate("MainWindow", "D9", nullptr));
        ___qtreewidgetitem3->setText(3, QCoreApplication::translate("MainWindow", "C9", nullptr));
        ___qtreewidgetitem3->setText(1, QCoreApplication::translate("MainWindow", "ACCB <- (ACCB) + (M) + (C", nullptr));
        ___qtreewidgetitem3->setText(0, QCoreApplication::translate("MainWindow", "ADCB", nullptr));
        QTreeWidgetItem *___qtreewidgetitem4 = treeWidget->topLevelItem(3);
        ___qtreewidgetitem4->setText(8, QCoreApplication::translate("MainWindow", "*-****", nullptr));
        ___qtreewidgetitem4->setText(6, QCoreApplication::translate("MainWindow", "AB", nullptr));
        ___qtreewidgetitem4->setText(5, QCoreApplication::translate("MainWindow", "BB", nullptr));
        ___qtreewidgetitem4->setText(4, QCoreApplication::translate("MainWindow", "9B", nullptr));
        ___qtreewidgetitem4->setText(3, QCoreApplication::translate("MainWindow", "8B", nullptr));
        ___qtreewidgetitem4->setText(1, QCoreApplication::translate("MainWindow", "ACCA <- (ACCA) + (M)", nullptr));
        ___qtreewidgetitem4->setText(0, QCoreApplication::translate("MainWindow", "ADDA", nullptr));
        QTreeWidgetItem *___qtreewidgetitem5 = treeWidget->topLevelItem(4);
        ___qtreewidgetitem5->setText(8, QCoreApplication::translate("MainWindow", "*-****", nullptr));
        ___qtreewidgetitem5->setText(6, QCoreApplication::translate("MainWindow", "EB", nullptr));
        ___qtreewidgetitem5->setText(5, QCoreApplication::translate("MainWindow", "FB", nullptr));
        ___qtreewidgetitem5->setText(4, QCoreApplication::translate("MainWindow", "DB", nullptr));
        ___qtreewidgetitem5->setText(3, QCoreApplication::translate("MainWindow", "CB", nullptr));
        ___qtreewidgetitem5->setText(1, QCoreApplication::translate("MainWindow", "ACCB <- (ACCB) + (M)", nullptr));
        ___qtreewidgetitem5->setText(0, QCoreApplication::translate("MainWindow", "ADDB", nullptr));
        treeWidget->setSortingEnabled(__sortingEnabled);

        tabWidget->setTabText(tabWidget->indexOf(opCodes), QCoreApplication::translate("MainWindow", "OPC", nullptr));
        comboBoxSpeedSelector->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        comboBoxSpeedSelector->setItemText(1, QCoreApplication::translate("MainWindow", "2", nullptr));
        comboBoxSpeedSelector->setItemText(2, QCoreApplication::translate("MainWindow", "4", nullptr));
        comboBoxSpeedSelector->setItemText(3, QCoreApplication::translate("MainWindow", "8", nullptr));
        comboBoxSpeedSelector->setItemText(4, QCoreApplication::translate("MainWindow", "16", nullptr));
        comboBoxSpeedSelector->setItemText(5, QCoreApplication::translate("MainWindow", "32", nullptr));
        comboBoxSpeedSelector->setItemText(6, QCoreApplication::translate("MainWindow", "64", nullptr));
        comboBoxSpeedSelector->setItemText(7, QCoreApplication::translate("MainWindow", "128", nullptr));
        comboBoxSpeedSelector->setItemText(8, QCoreApplication::translate("MainWindow", "256", nullptr));
        comboBoxSpeedSelector->setItemText(9, QCoreApplication::translate("MainWindow", "512", nullptr));
        comboBoxSpeedSelector->setItemText(10, QCoreApplication::translate("MainWindow", "1024", nullptr));

        plainTextDisplay->setPlainText(QCoreApplication::translate("MainWindow", "                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                                                      \n"
"                      "
                        "                                \n"
"                                                      \n"
"                                                       ,", nullptr));
        comboBoxVersionSelector->setItemText(0, QCoreApplication::translate("MainWindow", "m6800", nullptr));
        comboBoxVersionSelector->setItemText(1, QCoreApplication::translate("MainWindow", "m6803", nullptr));

        buttonFSDisplay->setText(QCoreApplication::translate("MainWindow", "Fullscreen display", nullptr));
        buttonSwitchWrite->setText(QCoreApplication::translate("MainWindow", "Switch writing mode:", nullptr));
        labelWritingMode->setText(QCoreApplication::translate("MainWindow", "Code", nullptr));
        groupBoxSimpleMemory->setTitle(QCoreApplication::translate("MainWindow", "Memory", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetSM->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ADDRESS", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetSM->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "VALUE", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetSM->verticalHeaderItem(0);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidgetSM->verticalHeaderItem(1);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidgetSM->verticalHeaderItem(2);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidgetSM->verticalHeaderItem(3);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidgetSM->verticalHeaderItem(4);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidgetSM->verticalHeaderItem(5);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidgetSM->verticalHeaderItem(6);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidgetSM->verticalHeaderItem(7);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidgetSM->verticalHeaderItem(8);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidgetSM->verticalHeaderItem(9);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidgetSM->verticalHeaderItem(10);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidgetSM->verticalHeaderItem(11);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tableWidgetSM->verticalHeaderItem(12);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tableWidgetSM->verticalHeaderItem(13);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tableWidgetSM->verticalHeaderItem(14);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = tableWidgetSM->verticalHeaderItem(15);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = tableWidgetSM->verticalHeaderItem(16);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = tableWidgetSM->verticalHeaderItem(17);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = tableWidgetSM->verticalHeaderItem(18);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = tableWidgetSM->verticalHeaderItem(19);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));

        const bool __sortingEnabled1 = tableWidgetSM->isSortingEnabled();
        tableWidgetSM->setSortingEnabled(false);
        tableWidgetSM->setSortingEnabled(__sortingEnabled1);

        QTableWidgetItem *___qtablewidgetitem22 = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "Symbol", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "IMM", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "DIR", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "IND", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "EXT", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = tableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "INH", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = tableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "REL", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "ABA", nullptr));
        QTableWidgetItem *___qtablewidgetitem30 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem30->setText(QCoreApplication::translate("MainWindow", "ADCA", nullptr));
        QTableWidgetItem *___qtablewidgetitem31 = tableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem31->setText(QCoreApplication::translate("MainWindow", "ADCB", nullptr));
        QTableWidgetItem *___qtablewidgetitem32 = tableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem32->setText(QCoreApplication::translate("MainWindow", "ADDA", nullptr));
        QTableWidgetItem *___qtablewidgetitem33 = tableWidget->verticalHeaderItem(4);
        ___qtablewidgetitem33->setText(QCoreApplication::translate("MainWindow", "ADDB", nullptr));
        QTableWidgetItem *___qtablewidgetitem34 = tableWidget->verticalHeaderItem(5);
        ___qtablewidgetitem34->setText(QCoreApplication::translate("MainWindow", "ANDA", nullptr));
        QTableWidgetItem *___qtablewidgetitem35 = tableWidget->verticalHeaderItem(6);
        ___qtablewidgetitem35->setText(QCoreApplication::translate("MainWindow", "ANDB", nullptr));
        QTableWidgetItem *___qtablewidgetitem36 = tableWidget->verticalHeaderItem(7);
        ___qtablewidgetitem36->setText(QCoreApplication::translate("MainWindow", "ASL", nullptr));
        QTableWidgetItem *___qtablewidgetitem37 = tableWidget->verticalHeaderItem(8);
        ___qtablewidgetitem37->setText(QCoreApplication::translate("MainWindow", "ASLA", nullptr));
        QTableWidgetItem *___qtablewidgetitem38 = tableWidget->verticalHeaderItem(9);
        ___qtablewidgetitem38->setText(QCoreApplication::translate("MainWindow", "ASLB", nullptr));
        QTableWidgetItem *___qtablewidgetitem39 = tableWidget->verticalHeaderItem(10);
        ___qtablewidgetitem39->setText(QCoreApplication::translate("MainWindow", "ASR", nullptr));
        QTableWidgetItem *___qtablewidgetitem40 = tableWidget->verticalHeaderItem(11);
        ___qtablewidgetitem40->setText(QCoreApplication::translate("MainWindow", "ASRA", nullptr));
        QTableWidgetItem *___qtablewidgetitem41 = tableWidget->verticalHeaderItem(12);
        ___qtablewidgetitem41->setText(QCoreApplication::translate("MainWindow", "ASRB", nullptr));
        QTableWidgetItem *___qtablewidgetitem42 = tableWidget->verticalHeaderItem(13);
        ___qtablewidgetitem42->setText(QCoreApplication::translate("MainWindow", "BCC", nullptr));
        QTableWidgetItem *___qtablewidgetitem43 = tableWidget->verticalHeaderItem(14);
        ___qtablewidgetitem43->setText(QCoreApplication::translate("MainWindow", "BCS", nullptr));
        QTableWidgetItem *___qtablewidgetitem44 = tableWidget->verticalHeaderItem(15);
        ___qtablewidgetitem44->setText(QCoreApplication::translate("MainWindow", "BEQ", nullptr));
        QTableWidgetItem *___qtablewidgetitem45 = tableWidget->verticalHeaderItem(16);
        ___qtablewidgetitem45->setText(QCoreApplication::translate("MainWindow", "BGE", nullptr));
        QTableWidgetItem *___qtablewidgetitem46 = tableWidget->verticalHeaderItem(17);
        ___qtablewidgetitem46->setText(QCoreApplication::translate("MainWindow", "BGT", nullptr));
        QTableWidgetItem *___qtablewidgetitem47 = tableWidget->verticalHeaderItem(18);
        ___qtablewidgetitem47->setText(QCoreApplication::translate("MainWindow", "BHI", nullptr));
        QTableWidgetItem *___qtablewidgetitem48 = tableWidget->verticalHeaderItem(19);
        ___qtablewidgetitem48->setText(QCoreApplication::translate("MainWindow", "BITA", nullptr));
        QTableWidgetItem *___qtablewidgetitem49 = tableWidget->verticalHeaderItem(20);
        ___qtablewidgetitem49->setText(QCoreApplication::translate("MainWindow", "BITB", nullptr));
        QTableWidgetItem *___qtablewidgetitem50 = tableWidget->verticalHeaderItem(21);
        ___qtablewidgetitem50->setText(QCoreApplication::translate("MainWindow", "BLE", nullptr));
        QTableWidgetItem *___qtablewidgetitem51 = tableWidget->verticalHeaderItem(22);
        ___qtablewidgetitem51->setText(QCoreApplication::translate("MainWindow", "BLS", nullptr));
        QTableWidgetItem *___qtablewidgetitem52 = tableWidget->verticalHeaderItem(23);
        ___qtablewidgetitem52->setText(QCoreApplication::translate("MainWindow", "BLT", nullptr));
        QTableWidgetItem *___qtablewidgetitem53 = tableWidget->verticalHeaderItem(24);
        ___qtablewidgetitem53->setText(QCoreApplication::translate("MainWindow", "BMI", nullptr));
        QTableWidgetItem *___qtablewidgetitem54 = tableWidget->verticalHeaderItem(25);
        ___qtablewidgetitem54->setText(QCoreApplication::translate("MainWindow", "BNE", nullptr));
        QTableWidgetItem *___qtablewidgetitem55 = tableWidget->verticalHeaderItem(26);
        ___qtablewidgetitem55->setText(QCoreApplication::translate("MainWindow", "BPL", nullptr));
        QTableWidgetItem *___qtablewidgetitem56 = tableWidget->verticalHeaderItem(27);
        ___qtablewidgetitem56->setText(QCoreApplication::translate("MainWindow", "BRA", nullptr));
        QTableWidgetItem *___qtablewidgetitem57 = tableWidget->verticalHeaderItem(28);
        ___qtablewidgetitem57->setText(QCoreApplication::translate("MainWindow", "BSR", nullptr));
        QTableWidgetItem *___qtablewidgetitem58 = tableWidget->verticalHeaderItem(29);
        ___qtablewidgetitem58->setText(QCoreApplication::translate("MainWindow", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem59 = tableWidget->verticalHeaderItem(30);
        ___qtablewidgetitem59->setText(QCoreApplication::translate("MainWindow", "BVS", nullptr));

        const bool __sortingEnabled2 = tableWidget->isSortingEnabled();
        tableWidget->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem60 = tableWidget->item(0, 0);
        ___qtablewidgetitem60->setText(QCoreApplication::translate("MainWindow", "A\342\206\220B+A", nullptr));
        QTableWidgetItem *___qtablewidgetitem61 = tableWidget->item(0, 5);
        ___qtablewidgetitem61->setText(QCoreApplication::translate("MainWindow", "1B", nullptr));
        QTableWidgetItem *___qtablewidgetitem62 = tableWidget->item(1, 0);
        ___qtablewidgetitem62->setText(QCoreApplication::translate("MainWindow", "A\342\206\220M+A+C", nullptr));
        QTableWidgetItem *___qtablewidgetitem63 = tableWidget->item(1, 1);
        ___qtablewidgetitem63->setText(QCoreApplication::translate("MainWindow", "89", nullptr));
        QTableWidgetItem *___qtablewidgetitem64 = tableWidget->item(1, 2);
        ___qtablewidgetitem64->setText(QCoreApplication::translate("MainWindow", "99", nullptr));
        QTableWidgetItem *___qtablewidgetitem65 = tableWidget->item(1, 3);
        ___qtablewidgetitem65->setText(QCoreApplication::translate("MainWindow", "A9", nullptr));
        QTableWidgetItem *___qtablewidgetitem66 = tableWidget->item(1, 4);
        ___qtablewidgetitem66->setText(QCoreApplication::translate("MainWindow", "B9", nullptr));
        QTableWidgetItem *___qtablewidgetitem67 = tableWidget->item(2, 0);
        ___qtablewidgetitem67->setText(QCoreApplication::translate("MainWindow", "B\342\206\220M+B+C", nullptr));
        QTableWidgetItem *___qtablewidgetitem68 = tableWidget->item(2, 1);
        ___qtablewidgetitem68->setText(QCoreApplication::translate("MainWindow", "C9", nullptr));
        QTableWidgetItem *___qtablewidgetitem69 = tableWidget->item(2, 2);
        ___qtablewidgetitem69->setText(QCoreApplication::translate("MainWindow", "D9", nullptr));
        QTableWidgetItem *___qtablewidgetitem70 = tableWidget->item(2, 3);
        ___qtablewidgetitem70->setText(QCoreApplication::translate("MainWindow", "E9", nullptr));
        QTableWidgetItem *___qtablewidgetitem71 = tableWidget->item(2, 4);
        ___qtablewidgetitem71->setText(QCoreApplication::translate("MainWindow", "F9", nullptr));
        QTableWidgetItem *___qtablewidgetitem72 = tableWidget->item(3, 0);
        ___qtablewidgetitem72->setText(QCoreApplication::translate("MainWindow", "A\342\206\220A+M", nullptr));
        QTableWidgetItem *___qtablewidgetitem73 = tableWidget->item(4, 0);
        ___qtablewidgetitem73->setText(QCoreApplication::translate("MainWindow", "B\342\206\220B+M", nullptr));
        QTableWidgetItem *___qtablewidgetitem74 = tableWidget->item(5, 0);
        ___qtablewidgetitem74->setText(QCoreApplication::translate("MainWindow", "A\342\206\220M*A", nullptr));
        QTableWidgetItem *___qtablewidgetitem75 = tableWidget->item(6, 0);
        ___qtablewidgetitem75->setText(QCoreApplication::translate("MainWindow", "B\342\206\220M*B", nullptr));
        QTableWidgetItem *___qtablewidgetitem76 = tableWidget->item(7, 0);
        ___qtablewidgetitem76->setText(QCoreApplication::translate("MainWindow", "Arithmetic shift left", nullptr));
        QTableWidgetItem *___qtablewidgetitem77 = tableWidget->item(8, 0);
        ___qtablewidgetitem77->setText(QCoreApplication::translate("MainWindow", "Arithmetic shift left", nullptr));
        QTableWidgetItem *___qtablewidgetitem78 = tableWidget->item(9, 0);
        ___qtablewidgetitem78->setText(QCoreApplication::translate("MainWindow", "Arithmetic shift left", nullptr));
        QTableWidgetItem *___qtablewidgetitem79 = tableWidget->item(10, 0);
        ___qtablewidgetitem79->setText(QCoreApplication::translate("MainWindow", "Arithmetic shift right", nullptr));
        QTableWidgetItem *___qtablewidgetitem80 = tableWidget->item(11, 0);
        ___qtablewidgetitem80->setText(QCoreApplication::translate("MainWindow", "Arithmetic shift right", nullptr));
        QTableWidgetItem *___qtablewidgetitem81 = tableWidget->item(12, 0);
        ___qtablewidgetitem81->setText(QCoreApplication::translate("MainWindow", "Arithmetic shift right", nullptr));
        QTableWidgetItem *___qtablewidgetitem82 = tableWidget->item(13, 0);
        ___qtablewidgetitem82->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem83 = tableWidget->item(14, 0);
        ___qtablewidgetitem83->setText(QCoreApplication::translate("MainWindow", "Branch (C=1)", nullptr));
        QTableWidgetItem *___qtablewidgetitem84 = tableWidget->item(15, 0);
        ___qtablewidgetitem84->setText(QCoreApplication::translate("MainWindow", "Branch (Z=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem85 = tableWidget->item(16, 0);
        ___qtablewidgetitem85->setText(QCoreApplication::translate("MainWindow", "Branch (N\342\212\225V=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem86 = tableWidget->item(17, 0);
        ___qtablewidgetitem86->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem87 = tableWidget->item(18, 0);
        ___qtablewidgetitem87->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem88 = tableWidget->item(19, 0);
        ___qtablewidgetitem88->setText(QCoreApplication::translate("MainWindow", "M*A", nullptr));
        QTableWidgetItem *___qtablewidgetitem89 = tableWidget->item(20, 0);
        ___qtablewidgetitem89->setText(QCoreApplication::translate("MainWindow", "M*B", nullptr));
        QTableWidgetItem *___qtablewidgetitem90 = tableWidget->item(21, 0);
        ___qtablewidgetitem90->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem91 = tableWidget->item(22, 0);
        ___qtablewidgetitem91->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem92 = tableWidget->item(23, 0);
        ___qtablewidgetitem92->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem93 = tableWidget->item(24, 0);
        ___qtablewidgetitem93->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem94 = tableWidget->item(25, 0);
        ___qtablewidgetitem94->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem95 = tableWidget->item(26, 0);
        ___qtablewidgetitem95->setText(QCoreApplication::translate("MainWindow", "Branch (C=0)", nullptr));
        QTableWidgetItem *___qtablewidgetitem96 = tableWidget->item(27, 0);
        ___qtablewidgetitem96->setText(QCoreApplication::translate("MainWindow", "Branch always", nullptr));
        QTableWidgetItem *___qtablewidgetitem97 = tableWidget->item(28, 0);
        ___qtablewidgetitem97->setText(QCoreApplication::translate("MainWindow", "Branch subroutine", nullptr));
        tableWidget->setSortingEnabled(__sortingEnabled2);

    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
