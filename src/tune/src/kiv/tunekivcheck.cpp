#include "tune/kiv/tunekivcheck.h"

#include <datablocks/kiv/bda.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <widgets/epopup.h>
#include <widgets/graphfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/waitwidget.h>
#include <widgets/wdfunc.h>

#include <QCoreApplication>
#include <QDialog>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

TuneKIVCheck::TuneKIVCheck(Device::CurrentDevice *device, QWidget *parent) : AbstractTuneDialog(device, parent)
{
    setupUI();
}

void TuneKIVCheck::setTuneFunctions()
{
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение схемы подключения...", &TuneKIVCheck::showScheme);
    addTuneFunc("Проверка...", &TuneKIVCheck::check);
}

Error::Msg TuneKIVCheck::showScheme()
{
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->addWidget(GraphFunc::NewIcon(this, ":/tunes/tunekiv1.png"));
    layout->addWidget(LBLFunc::NewLBL(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    layout->addWidget(LBLFunc::NewLBL(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока "
        "и напряжения в однофазной сети переменного тока, установите предел измерения "
        "по напряжению 60 В, по току - 2,5 А;"));

    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(LBLFunc::NewLBL(this, "РЕТОМ"));
    vLayout->addWidget(GraphFunc::newHLine(this));
    vLayout->addWidget(LBLFunc::NewLBL(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В с углом 0 град., "
        "ток I1 равным 2,9 А с углом 89,9 град., включите выходы РЕТОМ-51;"));
    hLayout->addLayout(vLayout);
    hLayout->addWidget(GraphFunc::newVLine(this));
    vLayout = new QVBoxLayout;
    vLayout->addWidget(LBLFunc::NewLBL(this, "ИМИТАТОР"));
    vLayout->addWidget(GraphFunc::newHLine(this));
    vLayout->addWidget(LBLFunc::NewLBL(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В, "
        "ток I1 равным 29 мА, tgδ равным +0,2 %"));
    hLayout->addLayout(vLayout);
    layout->addLayout(hLayout);
    layout->addWidget(LBLFunc::NewLBL(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника "
        "составляет 60 ± 0,25 В, ток – 0,290 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    layout->addWidget(LBLFunc::NewLBL(
        this, "5. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    layout->addWidget(LBLFunc::NewLBL(this, "6. Установите на магазине сопротивлений сопротивление 100,0 Ом."));
    widget->setLayout(layout);
    if (!EMessageBox::next(this, widget))
    {
        CancelTune();
        return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg TuneKIVCheck::check()
{
    std::vector<std::pair<QString, S2::valueType>> recordList {
        { "C_Pasp_ID", S2::FLOAT_3t { 9000, 9000, 9000 } }, //
        { "Unom1", float(220) }                             //
    };

    if (sendChangedConfig(recordList) != Error::NoError)
    {
        EMessageBox::error(this, "Ошибка при записи конфигурации");
        return Error::GeneralError;
    }

    WaitWidget *ww = new WaitWidget(this);
    ww->SetMessage("Пожалуйста, подождите");
    ww->Start();
    QElapsedTimer *tmr = new QElapsedTimer;
    tmr->start();
    while (tmr->elapsed() < TIMEFORBDATOSETINMS)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    ww->Stop();
    BdaA284 *bda = new BdaA284(this);
    bda->setup(m_device->getUID(), m_sync);
    bda->readAndUpdate();
    auto s2file = config.toByteArray();

#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
        if (!WDFunc::floatIsWithinLimits("напряжения", bda->data()->Ueff_ADC[i], 2150000.0, 150000.0))
            goto FaultLabel;
    for (int i = 3; i < 6; ++i)
        if (!WDFunc::floatIsWithinLimits("тока", bda->data()->Ueff_ADC[i], 1220000.0, 60000.0))
            goto FaultLabel;
    if (!WDFunc::floatIsWithinLimits("сопротивления", bda->data()->Pt100, 2123.0, 120.0))
        goto FaultLabel;
    if (!WDFunc::floatIsWithinLimits("частоты", bda->data()->Frequency, 51.0, 0.05))
        goto FaultLabel;
#endif
    m_sync->writeFileSync(S2::FilesEnum::Config, s2file);
    return Error::Msg::NoError;
FaultLabel:
    m_sync->writeFileSync(S2::FilesEnum::Config, s2file);
    return Error::Msg::GeneralError;
}

void TuneKIVCheck::showEvent(QShowEvent *e)
{
    e->accept();
}
