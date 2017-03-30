#include "hiddendialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>

HiddenDialog::HiddenDialog(int type, QWidget *parent) :
    QDialog(parent)
{
    switch (type)
    {
    case BYMY:
        BGImage=":/BM.png";
    case BNMY:
        BGImage=":/BnM.png";
    case BNMY:
        BGImage=":/BMn.png";
    case BNMY:
        BGImage=":/BnMn.png";
    default:
        BGImage="";
    }
}

void HiddenDialog::SetupUI()
{
    QString tmps = "QMainWindow {background-color: "+QString(MAINWINCLR)+";}";
    setStyleSheet(tmps);
    setMinimumSize(QSize(800, 600));
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;

}

void HiddenDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(BGImage));
    e->accept();
}
