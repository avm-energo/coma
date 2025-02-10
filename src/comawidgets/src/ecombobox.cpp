#include "widgets/ecombobox.h"

EComboBox::EComboBox(QWidget *parent) : QComboBox(parent)
{
    setStyleSheet("QComboBox {background-color: rgba(0,0,0,0); border: 1px solid "
                  "gray; border-radius: 5px;}"
                  "QComboBox::drop-down {background-color: "
                  "rgba(100,100,100,255); width: 5px; border-style: none;}");
}

void EComboBox::setItems(const QMap<QString, int> &map)
{
    m_comboMap.insert(map);
    addItems(map.keys());
}

void EComboBox::setIndexByValue(const int value)
{
    QString str = m_comboMap.key(value);
    setCurrentText(str);
}

void EComboBox::setIndexByKey(const QString &key)
{
    setCurrentText(key);
}

int EComboBox::getValueByCurrentIndex() const
{
    QString str = currentText();
    return m_comboMap.value(str);
}
