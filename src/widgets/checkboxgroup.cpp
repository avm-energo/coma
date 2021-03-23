#include "checkboxgroup.h"

#include <QDebug>
#include <QVBoxLayout>

CheckBoxGroup::CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent)
    : QWidget(parent), m_hiddenPositions(ignorePos), m_description(desc)
{
    QGridLayout *gridlyout = new QGridLayout;
    for (auto i = 0; i != m_description.size(); ++i)
    {
        const QString name = m_description.at(i);
        if (ignorePositions().contains(i))
            continue;
        QCheckBox *checkBox = new QCheckBox(name, this);
        checkBox->setObjectName(QString::number(i));
        gridlyout->addWidget(checkBox, i / 2, i % 2);
        connect(checkBox, &QCheckBox::stateChanged, this, [=](const int value) {
            Qt::CheckState state = Qt::CheckState(value);
            if (!(state == Qt::Checked && m_bitset.test(i)) && !(state == Qt::Unchecked && !m_bitset.test(i)))
                m_bitset.flip(i);
        });
    }
    setLayout(gridlyout);
}

CheckBoxGroup::CheckBoxGroup(const QStringList &desc, QWidget *parent) : QWidget(parent), m_description(desc)
{
    QGridLayout *gridlyout = new QGridLayout;
    for (auto i = 0; i != m_description.size(); ++i)
    {
        const QString name = m_description.at(i);
        QCheckBox *checkBox = new QCheckBox(name, this);
        checkBox->setObjectName(QString::number(i));
        gridlyout->addWidget(checkBox, i / 2, i % 2);
        connect(checkBox, &QCheckBox::stateChanged, this, [=](const int value) {
            Qt::CheckState state = Qt::CheckState(value);
            if (!(state == Qt::Checked && m_bitset.test(i)) && !(state == Qt::Unchecked && !m_bitset.test(i)))
                m_bitset.flip(i);
        });
    }
    setLayout(gridlyout);
}
