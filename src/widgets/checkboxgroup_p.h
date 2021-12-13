#pragma once

#include "../gen/stdfunc.h"
#include "checkboxgroup.h"

#include <boost/dynamic_bitset.hpp>

class CheckBoxGroupPrivate
{
    Q_DECLARE_PUBLIC(CheckBoxGroup)

public:
    CheckBoxGroupPrivate(int count)
    {
        m_bitset = boost::dynamic_bitset<>(std::size_t(std_ext::clp2(count)));
    }
    template <typename T> void setBits(const T value)
    {
        Q_Q(CheckBoxGroup);
        m_bitset = boost::dynamic_bitset(sizeof(T) * 8, value);
        [[maybe_unused]] const T test = T(m_bitset.to_ulong());
        QList<QCheckBox *> checkBoxes = q->findChildren<QCheckBox *>();
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

    bool test(size_t i) const
    {
        return m_bitset.test(i);
    }
    void flip(size_t i)
    {
        m_bitset.flip(i);
    }
    QStringList description() const
    {
        return m_description;
    }
    void setDescription(const QStringList &description)
    {
        m_description = description;
    }

    void setHiddenPositions(const QList<int> &hiddenPositions)
    {
        m_hiddenPositions = hiddenPositions;
    }

private:
    CheckBoxGroup *q_ptr;

    QList<int> m_hiddenPositions;
    boost::dynamic_bitset<> m_bitset;
    QStringList m_description;
};
