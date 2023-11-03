#ifndef EXTERNALDISPLAY_H
#define EXTERNALDISPLAY_H

#include "ui_externaldisplay.h"
#include <QDialog>
#include <QPlainTextEdit>

namespace Ui {
class ExternalDisplay;
}

class ExternalDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit ExternalDisplay(QWidget *parent = nullptr);
    ~ExternalDisplay();
    bool eventFilter(QObject* obj, QEvent* event);
    QPlainTextEdit* getPlainTextEdit() {
        return ui->plainTextDisplay;
    }
    void checkMousePos();
private:
    Ui::ExternalDisplay *ui;
private slots:
    void handleDisplayScrollVertical();
    void handleDisplayScrollHorizontal();
};

#endif // EXTERNALDISPLAY_H
