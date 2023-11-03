#include "externaldisplay.h"
#include "ui_externaldisplay.h"
#include <QDebug>  // Include the necessary header for qDebug
#include <QScrollBar>

ExternalDisplay::ExternalDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExternalDisplay)
{
    ui->setupUi(this);
    connect(ui->plainTextDisplay->horizontalScrollBar(), &QScrollBar::valueChanged, this, &ExternalDisplay::handleDisplayScrollHorizontal);
    connect(ui->plainTextDisplay->verticalScrollBar(), &QScrollBar::valueChanged, this, &ExternalDisplay::handleDisplayScrollVertical);
    this->installEventFilter(this);
}

ExternalDisplay::~ExternalDisplay()
{
    delete ui;
}

bool ExternalDisplay::eventFilter(QObject* obj, QEvent* event) {
    /*/if (event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        qDebug() << "Window resized to: " << resizeEvent->size();

        int newWidth = resizeEvent->size().width();
        int newHeight = (5 * newWidth) / 7;

        this->resize(newWidth, newHeight);
    }/*/
    return true;
}
void ExternalDisplay::handleDisplayScrollHorizontal(){
    ui->plainTextDisplay->horizontalScrollBar()->setValue(0);
}
void ExternalDisplay::handleDisplayScrollVertical(){
    ui->plainTextDisplay->verticalScrollBar()->setValue(0);
}








