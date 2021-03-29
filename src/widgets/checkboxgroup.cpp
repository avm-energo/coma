#include "checkboxgroup.h"

#include "checkboxgroup_p.h"

#include <QDebug>
#include <QVBoxLayout>

CheckBoxGroup::CheckBoxGroup(const QStringList &desc, const QList<int> &ignorePos, QWidget *parent)
    : QWidget(parent), d_ptr(new CheckBoxGroupPrivate)
{
    Q_D(CheckBoxGroup);
    d->q_ptr = this;
    d->setDescription(desc);
    d->setHiddenPositions(ignorePos);
    QGridLayout *gridlyout = new QGridLayout;
    for (auto i = 0; i != d->description().size(); ++i)
    {
        const QString name = d->description().at(i);
        if (d->ignorePositions().contains(i))
            continue;
        QCheckBox *checkBox = new QCheckBox(name, this);
        checkBox->setObjectName(QString::number(i));
        gridlyout->addWidget(checkBox, i / 2, i % 2);
        connect(checkBox, &QCheckBox::stateChanged, this, [=](const int value) {
            Qt::CheckState state = Qt::CheckState(value);
            if (!(state == Qt::Checked && d->test(i)) && !(state == Qt::Unchecked && !d->test(i)))
                d->flip(i);
        });
    }
    setLayout(gridlyout);
}

CheckBoxGroup::CheckBoxGroup(const QStringList &desc, QWidget *parent)
    : QWidget(parent), d_ptr(new CheckBoxGroupPrivate)
{
    Q_D(CheckBoxGroup);
    d->q_ptr = this;
    d->setDescription(desc);
    QGridLayout *gridlyout = new QGridLayout;
    for (auto i = 0; i != d->description().size(); ++i)
    {
        const QString name = d->description().at(i);
        QCheckBox *checkBox = new QCheckBox(name, this);
        checkBox->setObjectName(QString::number(i));
        gridlyout->addWidget(checkBox, i / 2, i % 2);
        connect(checkBox, &QCheckBox::stateChanged, this, [=](const int value) {
            Qt::CheckState state = Qt::CheckState(value);
            if (!(state == Qt::Checked && d->test(i)) && !(state == Qt::Unchecked && !d->test(i)))
                d->flip(i);
        });
    }
    setLayout(gridlyout);
}

template <typename T> void CheckBoxGroup::setBits(const T value)
{
    Q_D(CheckBoxGroup);
    d->setBits(value);
}

template <typename T> T CheckBoxGroup::bits()
{
    Q_D(CheckBoxGroup);
    return d->bits<T>();
}

template <> void CheckBoxGroup::setBits<quint64>(const quint64 value)
{
    Q_D(CheckBoxGroup);
    d->setBits(value);
}

template <> void CheckBoxGroup::setBits<quint32>(const quint32 value)
{
    Q_D(CheckBoxGroup);
    d->setBits(value);
}

template <> quint32 CheckBoxGroup::bits()
{
    Q_D(CheckBoxGroup);
    return d->bits<quint32>();
}

template <> quint64 CheckBoxGroup::bits<quint64>()
{
    Q_D(CheckBoxGroup);
    return d->bits<quint64>();
}
