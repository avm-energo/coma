#include "s_tqspinbox.h"

s_tqspinbox::s_tqspinbox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    setStyleSheet("QSpinBox {background-color: transparent; border: 1px solid gray; border-radius: 5px;}");
    connect (this,SIGNAL(valueChanged(double)),this,SLOT(changedValue(double)));
}

void s_tqspinbox::setAData(QVariant dat)
{
    this->adata = dat;
}

QVariant s_tqspinbox::getAData()
{
    return this->adata;
}

void s_tqspinbox::changedValue(double d)
{
    emit valueChanged(d, this);
}
