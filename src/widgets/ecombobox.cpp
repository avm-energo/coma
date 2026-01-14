#include "ecombobox.h"

#include "../gen/colors.h"

EComboBox::EComboBox(QWidget *parent) : QComboBox(parent)
{
    setStyleSheet("QComboBox {background-color: rgba(0,0,0,0); border: 1px solid gray; border-radius: 5px;}"
                  "QComboBox QListView {background-color: "
        + QString(DCONFOCLT) + "; width: 5px; border-style: none;}");
}
