#ifndef INSTRUCTIONINFODIALOG_H
#define INSTRUCTIONINFODIALOG_H

#include "qtreewidget.h"
#include <QDialog>

namespace Ui {
class InstructionInfoDialog;
}

class InstructionInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionInfoDialog(QTreeWidgetItem item, int version, QWidget *parent = nullptr);
    ~InstructionInfoDialog();

private:
    Ui::InstructionInfoDialog *ui;
};

#endif // INSTRUCTIONINFODIALOG_H
