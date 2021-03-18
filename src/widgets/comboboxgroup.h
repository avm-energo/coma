#pragma once
#include <QMultiMap>
#include <QWidget>
#include <bitset>
class CheckBoxGroup : public QWidget
{
    Q_OBJECT
public:
    explicit CheckBoxGroup(QWidget *parent = nullptr);

signals:
private:
    std::bitset<32> m_bits;
    QStringList m_description;
    QMultiMap<decltype(m_bits)::reference, int> map;
};
