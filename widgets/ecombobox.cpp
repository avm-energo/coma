#include "ecombobox.h"

EComboBox::EComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setStyleSheet("QComboBox {background-color: rgba(0,0,0,0); border: 1px solid gray; border-radius: 5px;}"
                              "QComboBox::drop-down {background-color: rgba(100,100,100,255); width: 5px; border-style: none;}");
}
