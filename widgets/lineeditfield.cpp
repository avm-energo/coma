#include <QPaintEvent>
#include <QPainter>
#include <QRect>
#include "lineeditfield.h"

LineEditField::LineEditField(QWidget *parent) : QLineEdit(parent)
{

}

LineEditField::LineEditField(const QString &text, QWidget *parent) : QLineEdit(parent)
{
    this->setText(text);
}

void LineEditField::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::darkGray);
    painter.drawLine(0,size().height()-4,size().width(),size().height()-4);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, this->text());
    e->accept();
}
