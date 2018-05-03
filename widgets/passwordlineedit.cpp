#include <QHBoxLayout>
#include <QPushButton>
#include "wd_func.h"
#include "passwordlineedit.h"

PasswordLineEdit::PasswordLineEdit(QWidget *parent, QLineEdit::EchoMode echostyle)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLineEdit *le = WDFunc::NewLE(parent, "pswle");
    le->setEchoMode(echostyle);
    le->setText("");
    hlyout->addWidget(le, 10);
    QPushButton *pb = new QPushButton(parent);
    pb->setIcon(QIcon("images/pswshow.png"));
    QObject::connect(pb, SIGNAL(clicked(bool)), this, SLOT(PswLEToggleEchoMode()));
    hlyout->addWidget(pb, 0);
    setLayout(hlyout);
}

void PasswordLineEdit::PswLEToggleEchoMode()
{
    QLineEdit *le = this->findChild<QLineEdit *>("pswle");
    if (le == 0)
        return;
    if (le->echoMode() == QLineEdit::Normal)
        le->setEchoMode(QLineEdit::Password);
    else
        le->setEchoMode(QLineEdit::Normal);
}
