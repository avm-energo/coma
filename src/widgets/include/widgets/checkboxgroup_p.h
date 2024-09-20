#pragma once

// #include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <gen/stdfunc.h>
#include <widgets/checkboxgroup.h>

#define BITSET_MAX_COUNT 32

class CheckBoxGroupPrivate
{
    Q_DECLARE_PUBLIC(CheckBoxGroup)

public:
    CheckBoxGroupPrivate(int count)
    {
        m_bitset = std::bitset<BITSET_MAX_COUNT>(std::size_t(std_ext::clp2(count)));
    }

    template <typename T> void setBits(const T value)
    {
        Q_Q(CheckBoxGroup);
        m_bitset =  std::bitset<BITSET_MAX_COUNT>(value);
        const T test = T(m_bitset.to_ulong());
        auto checkBoxes = q->findChildren<QCheckBox *>();
        for (QCheckBox *checkBox : checkBoxes)
        {
            auto status = false;
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
    std::bitset<BITSET_MAX_COUNT> m_bitset; // checkbox group maximum items size is 32
    QStringList m_description;
};
