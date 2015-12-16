#include "s_tqspinbox.h"

s_tqSpinBox::s_tqSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    setStyleSheet("QSpinBox {background-color: transparent; border: 1px solid gray; border-radius: 5px;}");
    connect (this,SIGNAL(valueChanged(double)),this,SLOT(changedValue(double)));
}

void s_tqSpinBox::setAData(QVariant dat)
{
    this->adata = dat;
}

QVariant s_tqSpinBox::getAData()
{
    return this->adata;
}


void s_tqSpinBox::changedValue(double d)
{
    emit valueChanged(d, this);
}
