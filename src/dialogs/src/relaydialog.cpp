#include "dialogs/relaydialog.h"

#include "../../widgets/wd_func.h"

RelayDialog::RelayDialog(int relayCount, Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(RelayWidget(relayCount, this));
    setLayout(lyout);
    setSuccessMsg("");
    for (auto i = 0; i != relayCount; ++i)
    {
        relays.insert({ startRelayReg + i, false });
    }
}

QWidget *RelayDialog::RelayWidget(int relayCount, QWidget *parent)
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addSpacing(100);
    QWidget *w = new QWidget(parent);

    for (auto i = 0; i != relayCount; ++i)
    {
        QHBoxLayout *hlayout = new QHBoxLayout;
        QPushButton *button = new QPushButton("Переключить реле №" + QString::number(i + 1));
        hlayout->addWidget(button);
        QLabel *label = WDFunc::NewLBL2(parent, "Разомкнуто", "lbl" + QString::number(i + startRelayReg));
        hlayout->addWidget(label);
        connect(button, &QPushButton::clicked, this, [=] { changeRelay(i + startRelayReg); });
        vlyout->addLayout(hlayout);
    }

    QHBoxLayout *hlayout = new QHBoxLayout;
    QPushButton *button = new QPushButton("Сбросить все реле");
    connect(button, &QPushButton::clicked, this, [=] { resetRelays(relayCount); });
    hlayout->addWidget(button);

    vlyout->addLayout(hlayout);
    vlyout->addSpacing(100);
    w->setLayout(vlyout);

    return w;
}

void RelayDialog::paintEvent(QPaintEvent *event)
{
    if (!isReset)
    {
        resetRelays(relays.size());
        isReset = true;
    }
    QWidget::paintEvent(event);
}

void RelayDialog::changeRelay(int number)
{
    bool state = !relays.at(number);
    changeRelay(number, state);
}

void RelayDialog::changeRelay(int number, bool state)
{
    DataTypes::SingleCommand cmd;
    cmd.addr = quint32(number);
    cmd.value = state;
    engine()->currentConnection()->writeCommand(Commands::C_WriteSingleCommand, QVariant::fromValue(cmd));
    relays.at(number) = state;
    QString text = "Разомкнуто";
    if (state)
        text = "Замкнуто";
    WDFunc::SetLBLText(this, "lbl" + QString::number(number), text);
}

void RelayDialog::resetRelays(int count)
{
    for (auto i = startRelayReg; i != startRelayReg + count; ++i)
    {
        changeRelay(i, false);
    }
}
