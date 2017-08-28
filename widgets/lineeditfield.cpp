#include <QPaintEvent>
#include <QFontMetrics>
#include <QLine>
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
    QFont font;
    QFontMetrics fm(font);
    QPainter painter(this);
    QRect r = rect();
    painter.setPen(Qt::black);
    painter.drawLine(0,size().height()-4,size().width(),size().height()-4);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, this->text());
    QLine cursor(this->cursorRect().x()+fm.width(text()), this->cursorRect().y(),
                 this->cursorRect().x()+fm.width(text()), this->cursorRect().y()+this->cursorRect().height()-1);
    painter.setPen(QPen(QBrush(QColor(0,0,0,255)), 3.0));
    painter.drawLine(cursor);
    QLineEdit::paintEvent(e);
//    e->accept();
}
