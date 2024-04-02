#include "tune/85/tune85check.h"

#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <datablocks/kiv/bda.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <widgets/epopup.h>
#include <widgets/wd_func.h>

Tune85Check::Tune85Check(int tuneStep, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(tuneStep, device, parent)
{
    setupUI();
}

void Tune85Check::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение схемы подключения...", &Tune85Check::showScheme);
    addTuneFunc("Проверка...", &Tune85Check::check);
}

Error::Msg Tune85Check::showScheme()
{
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewIcon(this, ":/tunes/tunekiv1.png"));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));

    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "РЕТОМ"));
    vlyout->addWidget(WDFunc::newHLine(this));
    vlyout->addWidget(WDFunc::NewLBL2(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В с углом 0 град.,\n"
        "ток I1 равным 5 А с углом 89,9 град., включите выходы РЕТОМ-51;"));
    hlyout->addLayout(vlyout);
    hlyout->addWidget(WDFunc::newVLine(this));
    vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "ИМИТАТОР"));
    vlyout->addWidget(WDFunc::newHLine(this));
    vlyout->addWidget(WDFunc::NewLBL2(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В,\n"
        "ток I1 равным 29 мА, tgδ равным +0,2 %"));
    hlyout->addLayout(vlyout);
    lyout->addLayout(hlyout);

    lyout->addWidget(WDFunc::NewLBL2(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника"
        "составляет 60 ± 0,25 В, ток – 0,500 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    lyout->addWidget(WDFunc::NewLBL2(
        this, "5. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    lyout->addWidget(WDFunc::NewLBL2(this, "6. Установите на магазине сопротивлений сопротивление 100,0 Ом."));
    w->setLayout(hlyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune85Check::check()
{
    BdaA284 *bda = new BdaA284(this);
    bda->setup(m_device->getUID(), m_sync);
    bda->readAndUpdate();
#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
        if (!WDFunc::floatIsWithinLimits("напряжения", bda->data()->Ueff_ADC[i], 2150000.0, 150000.0))
            return Error::Msg::GeneralError;
    for (int i = 3; i < 6; ++i)
        if (!WDFunc::floatIsWithinLimits("тока", bda->data()->Ueff_ADC[i], 1220000.0, 60000.0))
            return Error::Msg::GeneralError;
    if (!WDFunc::floatIsWithinLimits("частоты", bda->data()->Frequency, 51.0, 0.05))
        return Error::Msg::GeneralError;
#endif
    return Error::Msg::NoError;
}

void Tune85Check::showEvent(QShowEvent *e)
{
    e->accept();
}
