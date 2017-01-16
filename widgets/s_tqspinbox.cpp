#include "s_tqspinbox.h"

s_tqSpinBox::s_tqSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    setStyleSheet("QSpinBox {background-color: transparent; border: 1px solid gray; border-radius: 5px;}");
}
