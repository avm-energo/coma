#include "passwordlineedit.h"

#include "wd_func.h"

#include <QHBoxLayout>
#include <QPushButton>
PasswordLineEdit::PasswordLineEdit(QWidget *parent) : QWidget(parent)
{
}

PasswordLineEdit::PasswordLineEdit(QLineEdit::EchoMode echostyle, QWidget *parent)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLineEdit *le = WDFunc::NewLE2(parent, "pswle");
    le->setEchoMode(echostyle);
    // le->setText("");
    hlyout->addWidget(le, 10);
    QPushButton *pb = new QPushButton(parent);
    pb->setIcon(QIcon("images/pswshow.png"));
    QObject::connect(pb, &QAbstractButton::clicked, [=] {
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
