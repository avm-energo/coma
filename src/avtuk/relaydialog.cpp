#include "relaydialog.h"

#include "../widgets/wd_func.h"
RelayDialog::RelayDialog(int relayCount, QWidget *parent) : UDialog(parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(RelayWidget(relayCount, this));
    setLayout(lyout);
}

QWidget *RelayDialog::RelayWidget(int relayCount, QWidget *parent)
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addSpacing(100);
    QWidget *w = new QWidget(parent);

    for (auto i = 0; i != relayCount; ++i)
    {
        QHBoxLayout *hlayout = new QHBoxLayout;
        QPushButton *button = new QPushButton("Подать команду на реле №" + QString::number(i));
        hlayout->addWidget(button);
        QLabel *label = WDFunc::NewLBL2(parent, "Разомкнуто", "lbl" + QString::number(i));
        hlayout->addWidget(label);
        // NOTE Clicked command
        connect(button, &QPushButton::clicked, [] {});
        vlyout->addLayout(hlayout);
    }

    QHBoxLayout *hlayout = new QHBoxLayout;
    QPushButton *button = new QPushButton("Сбросить все реле");
    // NOTE Clicked command
    connect(button, &QPushButton::clicked, [] {});
    hlayout->addWidget(button);

    vlyout->addLayout(hlayout);
    vlyout->addSpacing(100);
    w->setLayout(vlyout);

    return w;
}
