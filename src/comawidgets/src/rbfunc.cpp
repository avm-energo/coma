#include <widgets/rbfunc.h>

RBFunc::RBFunc() { }

QRadioButton *RBFunc::NewRB2(QWidget *parent, const QString &rbtext, const QString &rbname)
{
    auto rb = new QRadioButton(parent);
    rb->setObjectName(rbname);
    rb->setText(rbtext);
    return rb;
}

bool RBFunc::RBData(QWidget *parent, const QString &rbname, bool &data)
{
    auto rb = parent->findChild<QRadioButton *>(rbname);
    if (rb == nullptr)
        return false;
    data = rb->isChecked();
    return true;
}

bool RBFunc::SetRBData(QWidget *parent, const QString &rbname, bool data)
{
    auto rb = parent->findChild<QRadioButton *>(rbname);
    if (rb == nullptr)
        return false;
    rb->setChecked(data);
    return true;
}
