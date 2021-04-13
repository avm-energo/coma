#include "checkboxgroup_p.h"

CheckBoxGroupPrivate::CheckBoxGroupPrivate()
{
}

QStringList CheckBoxGroupPrivate::description() const
{
    return m_description;
}

void CheckBoxGroupPrivate::setDescription(const QStringList &description)
{
    m_description = description;
}

void CheckBoxGroupPrivate::setHiddenPositions(const QList<int> &hiddenPositions)
{
    m_hiddenPositions = hiddenPositions;
}
