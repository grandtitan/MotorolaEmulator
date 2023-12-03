#include "instructioninfodialog.h"
#include "qtreewidget.h"
#include "ui_instructioninfodialog.h"

InstructionInfoDialog::InstructionInfoDialog(QTreeWidgetItem item, int version, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstructionInfoDialog)
{
    ui->setupUi(this);
    ui->labelInstruction->setText(item.text(0));
    ui->labelOperation->setText(item.text(1));
    ui->labelDescription->setText(item.text(11) + "\nFlags: HINZVC:" + item.text(8));
    QStringList g{"IMH", "IMM", "DIR", "IND", "EXT", "REL"};
    int count = 0;
     for (int i = 2; i < 8; ++i) {
        if(item.text(i) != ""){
             ui->tableWidget->insertColumn(count);
            ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(g[i - 2]));
             QTableWidgetItem *newItem = new QTableWidgetItem(item.text(i));
             ui->tableWidget->setItem(0, ui->tableWidget->columnCount() - 1, newItem);
             newItem = new QTableWidgetItem(item.text(9).split(",")[count]);
             ui->tableWidget->setItem(1, ui->tableWidget->columnCount() - 1, newItem);
             newItem = new QTableWidgetItem(item.text(10).split(",")[count]);
             ui->tableWidget->setItem(2, ui->tableWidget->columnCount() - 1, newItem);
            count++;
        }
    }
     if(version == 6803){
        ui->labelDescription->setText(ui->labelDescription->text() + "\nSupported by M6803 and above.");
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            ui->tableWidget->horizontalHeaderItem(col)->setForeground(Qt::red);
            for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                QTableWidgetItem* item = ui->tableWidget->item(row, col);
                if (item) {
                    item->setForeground(Qt::red);
                }
            }
        }
    }
     if (item.text(0) == "JSR") {
        ui->tableWidget->horizontalHeaderItem(0)->setForeground(Qt::red);
        ui->tableWidget->item(0,0)->setForeground(Qt::red);
        ui->tableWidget->item(1,0)->setForeground(Qt::red);
        ui->tableWidget->item(2,0)->setForeground(Qt::red);
     }
     if(count == 0) ui->tableWidget->setVisible(false);
}


InstructionInfoDialog::~InstructionInfoDialog()
{
    delete ui;
}
