#include "checkboxgroup.h"

#include <QCheckBox>
#include <QDebug>
#include <QVBoxLayout>

template <typename T, std::size_t N>
CheckBoxGroup<T, N>::CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent)
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
            if (!(state == Qt::Checked && m_bits.test(i)) && !(state == Qt::Unchecked && !m_bits.test(i)))
                m_bits.flip(i);
        });
    }
    setLayout(gridlyout);
}

template <typename T, std::size_t N>
CheckBoxGroup<T, N>::CheckBoxGroup(const QStringList &desc, QWidget *parent) : QWidget(parent), m_description(desc)
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
            if (!(state == Qt::Checked && m_bits.test(i)) && !(state == Qt::Unchecked && !m_bits.test(i)))
                m_bits.flip(i);
        });
    }
    setLayout(gridlyout);
}

template <typename T, std::size_t N> void CheckBoxGroup<T, N>::setBits(const T value)
{
    m_bits = value;
    [[maybe_unused]] const T test = (T)m_bits.to_ullong();
    QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
    for (QCheckBox *checkBox : checkBoxes)
    {
        bool status = false;
        auto number = checkBox->objectName().toUInt(&status);
        if (!status)
            continue;
        checkBox->setChecked(m_bits.test(number));
    }
    //    for (auto i = 0; i != checkBoxes.size(); ++i)
    //    {
    //        checkBoxes.at(i)->setChecked(m_bits.test(i));
    //    }
}

template class CheckBoxGroup<quint8>;
template class CheckBoxGroup<quint16>;
template class CheckBoxGroup<quint32>;
