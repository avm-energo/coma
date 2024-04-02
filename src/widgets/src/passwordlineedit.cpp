#include "widgets/passwordlineedit.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <widgets/wd_func.h>

PasswordLineEdit::PasswordLineEdit(QWidget *parent) : QWidget(parent)
{
}

PasswordLineEdit::PasswordLineEdit(QLineEdit::EchoMode echostyle, QWidget *parent)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLineEdit *le = WDFunc::NewLE2(parent, "pswle");
    le->setEchoMode(echostyle);
    hlyout->addWidget(le, 10);
    QPushButton *pb = new QPushButton(parent);
    pb->setIcon(QIcon(":/icons/pswshow.png"));
    QObject::connect(pb, &QAbstractButton::clicked, le, [=] {
        if (le->echoMode() == QLineEdit::Normal)
            le->setEchoMode(QLineEdit::Password);
        else
            le->setEchoMode(QLineEdit::Normal);
    });
    hlyout->addWidget(pb, 0);
    setLayout(hlyout);
}

void PasswordLineEdit::PswLEToggleEchoMode()
{
    QLineEdit *le = this->findChild<QLineEdit *>("pswle");
    if (le == nullptr)
    {
        qDebug("LE не определён");
        return;
    }
    if (le->echoMode() == QLineEdit::Normal)
        le->setEchoMode(QLineEdit::Password);
    else
        le->setEchoMode(QLineEdit::Normal);
}
