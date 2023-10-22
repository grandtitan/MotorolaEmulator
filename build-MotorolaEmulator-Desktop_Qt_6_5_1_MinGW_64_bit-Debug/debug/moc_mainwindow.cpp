/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MotorolaEmulator/mainwindow.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "resized",
    "",
    "newSize",
    "handleVerticalScrollBarValueChanged",
    "value",
    "handleLinesScroll",
    "handleDisplayScrollVertical",
    "handleDisplayScrollHorizontal",
    "handleMemoryScrollHorizontal",
    "handleMainWindowSizeChanged",
    "on_buttonCompile_clicked",
    "on_plainTextCode_textChanged",
    "on_buttonStep_clicked",
    "on_buttonReset_clicked",
    "on_buttonRunStop_clicked",
    "on_comboBoxSpeedSelector_activated",
    "index",
    "on_checkBoxHexRegs_clicked",
    "checked",
    "on_checkBoxAdvancedInfo_clicked",
    "on_checkBoxCompileOnRun_clicked",
    "on_spinBoxLow_valueChanged",
    "arg1",
    "on_spinBoxUp_valueChanged",
    "on_buttonSave_clicked",
    "on_buttonLoad_clicked",
    "on_lineEditBin_textChanged",
    "on_lineEditOct_textChanged",
    "on_lineEditHex_textChanged",
    "on_lineEditDec_textChanged",
    "on_buttonFSDisplay_clicked",
    "on_checkBoxWriteMemory_clicked",
    "on_buttonSwitchWrite_clicked",
    "on_comboBoxBreakWhen_currentIndexChanged",
    "on_checkBoxSimpleMemory_clicked",
    "on_spinBox_valueChanged",
    "on_comboBoxVersionSelector_currentIndexChanged",
    "on_checkBoxAutoReset_2_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {
    uint offsetsAndSizes[78];
    char stringdata0[11];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[36];
    char stringdata5[6];
    char stringdata6[18];
    char stringdata7[28];
    char stringdata8[30];
    char stringdata9[29];
    char stringdata10[28];
    char stringdata11[25];
    char stringdata12[29];
    char stringdata13[22];
    char stringdata14[23];
    char stringdata15[25];
    char stringdata16[35];
    char stringdata17[6];
    char stringdata18[27];
    char stringdata19[8];
    char stringdata20[32];
    char stringdata21[32];
    char stringdata22[27];
    char stringdata23[5];
    char stringdata24[26];
    char stringdata25[22];
    char stringdata26[22];
    char stringdata27[27];
    char stringdata28[27];
    char stringdata29[27];
    char stringdata30[27];
    char stringdata31[27];
    char stringdata32[31];
    char stringdata33[29];
    char stringdata34[41];
    char stringdata35[32];
    char stringdata36[24];
    char stringdata37[47];
    char stringdata38[31];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSMainWindowENDCLASS_t qt_meta_stringdata_CLASSMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 7),  // "resized"
        QT_MOC_LITERAL(19, 0),  // ""
        QT_MOC_LITERAL(20, 7),  // "newSize"
        QT_MOC_LITERAL(28, 35),  // "handleVerticalScrollBarValueC..."
        QT_MOC_LITERAL(64, 5),  // "value"
        QT_MOC_LITERAL(70, 17),  // "handleLinesScroll"
        QT_MOC_LITERAL(88, 27),  // "handleDisplayScrollVertical"
        QT_MOC_LITERAL(116, 29),  // "handleDisplayScrollHorizontal"
        QT_MOC_LITERAL(146, 28),  // "handleMemoryScrollHorizontal"
        QT_MOC_LITERAL(175, 27),  // "handleMainWindowSizeChanged"
        QT_MOC_LITERAL(203, 24),  // "on_buttonCompile_clicked"
        QT_MOC_LITERAL(228, 28),  // "on_plainTextCode_textChanged"
        QT_MOC_LITERAL(257, 21),  // "on_buttonStep_clicked"
        QT_MOC_LITERAL(279, 22),  // "on_buttonReset_clicked"
        QT_MOC_LITERAL(302, 24),  // "on_buttonRunStop_clicked"
        QT_MOC_LITERAL(327, 34),  // "on_comboBoxSpeedSelector_acti..."
        QT_MOC_LITERAL(362, 5),  // "index"
        QT_MOC_LITERAL(368, 26),  // "on_checkBoxHexRegs_clicked"
        QT_MOC_LITERAL(395, 7),  // "checked"
        QT_MOC_LITERAL(403, 31),  // "on_checkBoxAdvancedInfo_clicked"
        QT_MOC_LITERAL(435, 31),  // "on_checkBoxCompileOnRun_clicked"
        QT_MOC_LITERAL(467, 26),  // "on_spinBoxLow_valueChanged"
        QT_MOC_LITERAL(494, 4),  // "arg1"
        QT_MOC_LITERAL(499, 25),  // "on_spinBoxUp_valueChanged"
        QT_MOC_LITERAL(525, 21),  // "on_buttonSave_clicked"
        QT_MOC_LITERAL(547, 21),  // "on_buttonLoad_clicked"
        QT_MOC_LITERAL(569, 26),  // "on_lineEditBin_textChanged"
        QT_MOC_LITERAL(596, 26),  // "on_lineEditOct_textChanged"
        QT_MOC_LITERAL(623, 26),  // "on_lineEditHex_textChanged"
        QT_MOC_LITERAL(650, 26),  // "on_lineEditDec_textChanged"
        QT_MOC_LITERAL(677, 26),  // "on_buttonFSDisplay_clicked"
        QT_MOC_LITERAL(704, 30),  // "on_checkBoxWriteMemory_clicked"
        QT_MOC_LITERAL(735, 28),  // "on_buttonSwitchWrite_clicked"
        QT_MOC_LITERAL(764, 40),  // "on_comboBoxBreakWhen_currentI..."
        QT_MOC_LITERAL(805, 31),  // "on_checkBoxSimpleMemory_clicked"
        QT_MOC_LITERAL(837, 23),  // "on_spinBox_valueChanged"
        QT_MOC_LITERAL(861, 46),  // "on_comboBoxVersionSelector_cu..."
        QT_MOC_LITERAL(908, 30)   // "on_checkBoxAutoReset_2_clicked"
    },
    "MainWindow",
    "resized",
    "",
    "newSize",
    "handleVerticalScrollBarValueChanged",
    "value",
    "handleLinesScroll",
    "handleDisplayScrollVertical",
    "handleDisplayScrollHorizontal",
    "handleMemoryScrollHorizontal",
    "handleMainWindowSizeChanged",
    "on_buttonCompile_clicked",
    "on_plainTextCode_textChanged",
    "on_buttonStep_clicked",
    "on_buttonReset_clicked",
    "on_buttonRunStop_clicked",
    "on_comboBoxSpeedSelector_activated",
    "index",
    "on_checkBoxHexRegs_clicked",
    "checked",
    "on_checkBoxAdvancedInfo_clicked",
    "on_checkBoxCompileOnRun_clicked",
    "on_spinBoxLow_valueChanged",
    "arg1",
    "on_spinBoxUp_valueChanged",
    "on_buttonSave_clicked",
    "on_buttonLoad_clicked",
    "on_lineEditBin_textChanged",
    "on_lineEditOct_textChanged",
    "on_lineEditHex_textChanged",
    "on_lineEditDec_textChanged",
    "on_buttonFSDisplay_clicked",
    "on_checkBoxWriteMemory_clicked",
    "on_buttonSwitchWrite_clicked",
    "on_comboBoxBreakWhen_currentIndexChanged",
    "on_checkBoxSimpleMemory_clicked",
    "on_spinBox_valueChanged",
    "on_comboBoxVersionSelector_currentIndexChanged",
    "on_checkBoxAutoReset_2_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  206,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    1,  209,    2, 0x0a,    3 /* Public */,
       6,    0,  212,    2, 0x0a,    5 /* Public */,
       7,    0,  213,    2, 0x0a,    6 /* Public */,
       8,    0,  214,    2, 0x0a,    7 /* Public */,
       9,    0,  215,    2, 0x0a,    8 /* Public */,
      10,    1,  216,    2, 0x08,    9 /* Private */,
      11,    0,  219,    2, 0x08,   11 /* Private */,
      12,    0,  220,    2, 0x08,   12 /* Private */,
      13,    0,  221,    2, 0x08,   13 /* Private */,
      14,    0,  222,    2, 0x08,   14 /* Private */,
      15,    0,  223,    2, 0x08,   15 /* Private */,
      16,    1,  224,    2, 0x08,   16 /* Private */,
      18,    1,  227,    2, 0x08,   18 /* Private */,
      20,    1,  230,    2, 0x08,   20 /* Private */,
      21,    1,  233,    2, 0x08,   22 /* Private */,
      22,    1,  236,    2, 0x08,   24 /* Private */,
      24,    1,  239,    2, 0x08,   26 /* Private */,
      25,    0,  242,    2, 0x08,   28 /* Private */,
      26,    0,  243,    2, 0x08,   29 /* Private */,
      27,    1,  244,    2, 0x08,   30 /* Private */,
      28,    1,  247,    2, 0x08,   32 /* Private */,
      29,    1,  250,    2, 0x08,   34 /* Private */,
      30,    1,  253,    2, 0x08,   36 /* Private */,
      31,    0,  256,    2, 0x08,   38 /* Private */,
      32,    1,  257,    2, 0x08,   39 /* Private */,
      33,    0,  260,    2, 0x08,   41 /* Private */,
      34,    1,  261,    2, 0x08,   42 /* Private */,
      35,    1,  264,    2, 0x08,   44 /* Private */,
      36,    1,  267,    2, 0x08,   46 /* Private */,
      37,    1,  270,    2, 0x08,   48 /* Private */,
      38,    1,  273,    2, 0x08,   50 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QSize,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QSize,    3,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Bool,   19,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'resized'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        // method 'handleVerticalScrollBarValueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handleLinesScroll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDisplayScrollVertical'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDisplayScrollHorizontal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleMemoryScrollHorizontal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleMainWindowSizeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QSize &, std::false_type>,
        // method 'on_buttonCompile_clicked'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_plainTextCode_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonStep_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonReset_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonRunStop_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_comboBoxSpeedSelector_activated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_checkBoxHexRegs_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_checkBoxAdvancedInfo_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_checkBoxCompileOnRun_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_spinBoxLow_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_spinBoxUp_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_buttonSave_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_buttonLoad_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_lineEditBin_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_lineEditOct_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_lineEditHex_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_lineEditDec_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_buttonFSDisplay_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_checkBoxWriteMemory_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_buttonSwitchWrite_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_comboBoxBreakWhen_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_checkBoxSimpleMemory_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_spinBox_valueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_comboBoxVersionSelector_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_checkBoxAutoReset_2_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->resized((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1]))); break;
        case 1: _t->handleVerticalScrollBarValueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->handleLinesScroll(); break;
        case 3: _t->handleDisplayScrollVertical(); break;
        case 4: _t->handleDisplayScrollHorizontal(); break;
        case 5: _t->handleMemoryScrollHorizontal(); break;
        case 6: _t->handleMainWindowSizeChanged((*reinterpret_cast< std::add_pointer_t<QSize>>(_a[1]))); break;
        case 7: { bool _r = _t->on_buttonCompile_clicked();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->on_plainTextCode_textChanged(); break;
        case 9: _t->on_buttonStep_clicked(); break;
        case 10: _t->on_buttonReset_clicked(); break;
        case 11: _t->on_buttonRunStop_clicked(); break;
        case 12: _t->on_comboBoxSpeedSelector_activated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->on_checkBoxHexRegs_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->on_checkBoxAdvancedInfo_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->on_checkBoxCompileOnRun_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->on_spinBoxLow_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->on_spinBoxUp_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->on_buttonSave_clicked(); break;
        case 19: _t->on_buttonLoad_clicked(); break;
        case 20: _t->on_lineEditBin_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->on_lineEditOct_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->on_lineEditHex_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 23: _t->on_lineEditDec_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 24: _t->on_buttonFSDisplay_clicked(); break;
        case 25: _t->on_checkBoxWriteMemory_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->on_buttonSwitchWrite_clicked(); break;
        case 27: _t->on_comboBoxBreakWhen_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->on_checkBoxSimpleMemory_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 29: _t->on_spinBox_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 30: _t->on_comboBoxVersionSelector_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 31: _t->on_checkBoxAutoReset_2_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(const QSize & );
            if (_t _q_method = &MainWindow::resized; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 32)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 32;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::resized(const QSize & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
