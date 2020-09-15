#include "edoublespinbox.h"

#include <QWheelEvent>

EDoubleSpinBox::EDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void EDoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (!hasFocus())
    {
        event->ignore();
    }
    else
    {
        QDoubleSpinBox::wheelEvent(event);
    }
}
