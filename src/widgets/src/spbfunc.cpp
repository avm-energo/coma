#include <gen/error.h>
#include <widgets/spbfunc.h>

SPBFunc::SPBFunc() { }

QDoubleSpinBox *SPBFunc::NewSPB2(
    QWidget *parent, const QString &spbname, const double min, const double max, const int decimals)
{
    auto dsb = new QDoubleSpinBox(parent);
    double step = qPow(0.1f, decimals);
    dsb->setObjectName(spbname);
    dsb->setSingleStep(step);
    dsb->setDecimals(decimals);
    dsb->setMinimum(min);
    dsb->setMaximum(max);
    return dsb;
}

DoubleSpinBoxGroup *SPBFunc::NewSPBG(
    QWidget *parent, const QString &spbname, int count, const double min, const double max, const int decimals)
{
    auto spinBoxGroup = new DoubleSpinBoxGroup(count, parent);
    spinBoxGroup->setObjectName(spbname);
    double step = std::pow(0.1f, decimals);
    spinBoxGroup->setSingleStep(step);
    spinBoxGroup->setDecimals(decimals);
    spinBoxGroup->setMinimum(min);
    spinBoxGroup->setMaximum(max);
    return spinBoxGroup;
}

DoubleSpinBoxGroup *SPBFunc::NewSPBG(QWidget *parent, const QString &spbname, const QStringList &list, const double min,
    const double max, const int decimals)
{
    auto spinBoxGroup = new DoubleSpinBoxGroup(list, parent);
    spinBoxGroup->setObjectName(spbname);
    double step = std::pow(0.1f, decimals);
    spinBoxGroup->setSingleStep(step);
    spinBoxGroup->setDecimals(decimals);
    spinBoxGroup->setMinimum(min);
    spinBoxGroup->setMaximum(max);
    return spinBoxGroup;
}

bool SPBFunc::SetSPBData(const QObject *parent, const QString &spbname, const double &spbvalue)
{
    auto spb = parent->findChild<QDoubleSpinBox *>(spbname);
    if (spb == nullptr)
    {
        qDebug() << Error::NullDataError << spbname;
        return false;
    }
    spb->setValue(spbvalue);
    return true;
}
