#include "s_tqcombobox.h"

s_tqComboBox::s_tqComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setStyleSheet("QComboBox {background-color: rgba(0,0,0,0); border: 1px solid gray; border-radius: 5px;}"
//                              "QComboBox::drop-down {background-color: rgba(0,0,0,0); border: 1px solid gray;}");
                              "QComboBox::drop-down {background-color: rgba(0,0,0,0); width: 0px; border-style: none;}");
    connect (this, SIGNAL(currentIndexChanged(QString)), this, SLOT(changetext(QString)));
    connect (this, SIGNAL(currentIndexChanged(int)), this, SLOT(changetext(int)));
}

void s_tqComboBox::setAData(QVariant adata)
{
    this->adata = adata;
}

QVariant s_tqComboBox::getAData()
{
    return this->adata;
}

void s_tqComboBox::changetext(const QString &str)
{
    emit textChanged(str, this);
}

void s_tqComboBox::changetext(const int num)
{
    emit textChanged(num, this);
}

void s_tqComboBox::insertItem(int index, const QString &text, const QVariant &userData)
{
    this->adata = index;
    QComboBox::insertItem(index, text, userData);
}
