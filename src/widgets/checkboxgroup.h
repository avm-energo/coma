#pragma once
#include <QCheckBox>
#include <boost/dynamic_bitset.hpp>
class CheckBoxGroup : public QWidget
{
    Q_OBJECT
public:
    CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent = nullptr);
    CheckBoxGroup(const QStringList &desc, QWidget *parent = nullptr);
    template <typename T> void setBits(const T value)
    {
        m_bitset = boost::dynamic_bitset(sizeof(T), value);
        [[maybe_unused]] const T test = (T)m_bitset.to_ulong();
        QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
        for (QCheckBox *checkBox : checkBoxes)
        {
            bool status = false;
            auto number = checkBox->objectName().toUInt(&status);
            if (!status)
                continue;
            checkBox->setChecked(m_bitset.test(number));
        }
    }
    template <typename T> T bits() const
    {
        const T value = m_bitset.to_ulong();
        return value;
    }

    QList<int> ignorePositions() const
    {
        return m_hiddenPositions;
    }

private:
    QList<int> m_hiddenPositions;
    boost::dynamic_bitset<> m_bitset;
    QStringList m_description;
};
