#include <gen/error.h>
#include <widgets/chbfunc.h>

ChBFunc::ChBFunc() { }

QCheckBox *ChBFunc::NewChB(QWidget *parent, const QString &chbname, const QString &chbtext)
{
    auto chb = new QCheckBox(parent);
    chb->setObjectName(chbname);
    chb->setText(chbtext);
    return chb;
}

bool ChBFunc::ChBData(const QWidget *parent, const QString &chbname, bool &data)
{
    auto chb = parent->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    data = chb->isChecked();
    return true;
}

bool ChBFunc::ChBData(const QWidget *parent, const QString &chbname)
{
    auto chb = parent->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
    {
        qDebug() << Error::NullDataError;
        return false;
    }
    return chb->isChecked();
}

bool ChBFunc::SetChBData(QWidget *parent, const QString &chbname, bool data)
{
    auto chb = parent->findChild<QCheckBox *>(chbname);
    if (chb == nullptr)
        return false;
    chb->setChecked(data);
    return true;
}
