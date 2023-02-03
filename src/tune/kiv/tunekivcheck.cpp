#include "tunekivcheck.h"

#include "../../datablocks/kiv/bda.h"
#include "../../interfaces/protocom.h"
#include "../../widgets/epopup.h"
#include "../../widgets/wd_func.h"

#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <gen/files.h>
#include <gen/stdfunc.h>

TuneKIVCheck::TuneKIVCheck(ConfigV *config, int tuneStep, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    setupUI();
}

void TuneKIVCheck::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение схемы подключения...", &TuneKIVCheck::showScheme);
    addTuneFunc("Проверка...", &TuneKIVCheck::check);
}

Error::Msg TuneKIVCheck::showScheme()
{
    auto dialog = new QDialog(this);
    dialog->setFixedSize(800, 600);
    // auto widget = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/tunekiv1.png")));
    layout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    layout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));

    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(WDFunc::NewLBL2(this, "РЕТОМ"));
    vLayout->addWidget(WDFunc::newHLine(this));
    vLayout->addWidget(WDFunc::NewLBL2(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В с углом 0 град.,\n"
        "ток I1 равным 5 А с углом 89,9 град., включите выходы РЕТОМ-51;"));
    hLayout->addLayout(vLayout);
    hLayout->addWidget(WDFunc::newVLine(this));
    vLayout = new QVBoxLayout;
    vLayout->addWidget(WDFunc::NewLBL2(this, "ИМИТАТОР"));
    vLayout->addWidget(WDFunc::newHLine(this));
    vLayout->addWidget(WDFunc::NewLBL2(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 60,0 В,\n"
        "ток I1 равным 29 мА, tgδ равным +0,2 %"));
    hLayout->addLayout(vLayout);
    layout->addLayout(hLayout);
    layout->addWidget(WDFunc::NewLBL2(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника\n"
        "составляет 60 ± 0,25 В, ток – 0,500 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    layout->addWidget(WDFunc::NewLBL2(
        this, "5. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    layout->addWidget(WDFunc::NewLBL2(this, "6. Установите на магазине сопротивлений сопротивление 100,0 Ом."));
    layout->addWidget(WDFunc::NewPB(this, "", "Готово", [dialog] { dialog->close(); }));
    layout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dialog] { dialog->close(); }));
    // widget->setLayout(hLayout);
    // if (!EMessageBox::next(this, widget))
    //    CancelTune();
    dialog->setLayout(layout);
    WDFunc::PBConnect(dialog, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    dialog->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVCheck::check()
{
    BdaA284 *bda = new BdaA284;
    bda->readAndUpdate();
#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
        if (!WDFunc::floatIsWithinLimits("напряжения", bda->data()->Ueff_ADC[i], 2150000.0, 150000.0))
            return Error::Msg::GeneralError;
    for (int i = 3; i < 6; ++i)
        if (!WDFunc::floatIsWithinLimits("тока", bda->data()->Ueff_ADC[i], 1220000.0, 60000.0))
            return Error::Msg::GeneralError;
    if (!WDFunc::floatIsWithinLimits("сопротивления", bda->data()->Pt100, 2123.0, 120.0))
        return Error::Msg::GeneralError;
    if (!WDFunc::floatIsWithinLimits("частоты", bda->data()->Frequency, 51.0, 0.05))
        return Error::Msg::GeneralError;
#endif
    return Error::Msg::NoError;
}

void TuneKIVCheck::showEvent(QShowEvent *e)
{
    e->accept();
}
