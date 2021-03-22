#pragma once
#include <QDebug>
#include <QMultiMap>
#include <QWidget>
#include <bitset>
template <typename T, std::size_t N = sizeof(T) * 8> class CheckBoxGroup : public QWidget
{

public:
    CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent = nullptr);
    CheckBoxGroup(const QStringList &desc, QWidget *parent = nullptr);
    void setBits(const T value);
    T bits() const
    {
        const T value = (T)m_bits.to_ullong();
        return value;
    }

    QList<int> ignorePositions() const
    {
        return m_hiddenPositions;
    }

private:
    QList<int> m_hiddenPositions;
    std::bitset<N> m_bits;
    QStringList m_description;
    QMultiMap<typename decltype(m_bits)::reference, int> map;
};
