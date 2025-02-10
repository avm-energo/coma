#include <widgets/espinbox.h>

ESpinBox::ESpinBox(QWidget *parent) : QSpinBox(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void ESpinBox::wheelEvent(QWheelEvent *event)
{
    if (!hasFocus())
    {
        event->ignore();
    }
    else
    {
        QSpinBox::wheelEvent(event);
    }
}
