#include "tunekivcheck.h"

#include "../../datablocks/kiv/bda.h"
#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"

#include <QCoreApplication>
#include <QDialog>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <interfaces/connection.h>

TuneKIVCheck::TuneKIVCheck(S2::Configuration &config, int tuneStep, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    setupUI();
}

void TuneKIVCheck::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение схемы подключения...", &TuneKIVCheck::showScheme);
    addTuneFunc("Перевод в режим регулировки...", &TuneKIVCheck::setSMode2);
    addTuneFunc("Проверка...", &TuneKIVCheck::check);
}

Error::Msg TuneKIVCheck::setSMode2()
{
    Interface::Connection::iface()->writeCommand(Interface::Commands::C_SetMode, 0x02);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVCheck::showScheme()
{
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap(":/tunes/tunekiv1.png")));
    layout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    layout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока "
        "и напряжения в однофазной сети переменного тока, установите предел измерения "
        "по напряжению 60 В, по току - 2,5 А;"));

    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(WDFunc::NewLBL2(this, "РЕТОМ"));
    vLayout->addWidget(WDFunc::newHLine(this));
    vLayout->addWidget(WDFunc::NewLBL2(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В с углом 0 град., "
        "ток I1 равным 2,9 А с углом 89,9 град., включите выходы РЕТОМ-51;"));
    hLayout->addLayout(vLayout);
    hLayout->addWidget(WDFunc::newVLine(this));
    vLayout = new QVBoxLayout;
    vLayout->addWidget(WDFunc::NewLBL2(this, "ИМИТАТОР"));
    vLayout->addWidget(WDFunc::newHLine(this));
    vLayout->addWidget(WDFunc::NewLBL2(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В, "
        "ток I1 равным 29 мА, tgδ равным +0,2 %"));
    hLayout->addLayout(vLayout);
    layout->addLayout(hLayout);
    layout->addWidget(WDFunc::NewLBL2(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника "
        "составляет 60 ± 0,25 В, ток – 0,290 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    layout->addWidget(WDFunc::NewLBL2(
        this, "5. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    layout->addWidget(WDFunc::NewLBL2(this, "6. Установите на магазине сопротивлений сопротивление 100,0 Ом."));
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
    BdaA284 *bda = new BdaA284;
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
    Interface::Connection::iface()->writeFileSync(S2::FilesEnum::Config, s2file);
    return Error::Msg::NoError;
FaultLabel:
    Interface::Connection::iface()->writeFileSync(S2::FilesEnum::Config, s2file);
    return Error::Msg::GeneralError;
}

void TuneKIVCheck::showEvent(QShowEvent *e)
{
    e->accept();
}
