#include "s_tqcheckbox.h"

s_tqCheckBox::s_tqCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    connect(this,SIGNAL(stateChanged(int)),this,SLOT(changestate(int)));
}

void s_tqCheckBox::setAData(QVariant dat)
{
    this->adata = dat;
}

QVariant s_tqCheckBox::getAData()
{
    return this->adata;
}

void s_tqCheckBox::changestate(int state)
{
    emit statechanged(state,this);
}
