#include "tunekivcheck.h"

#include "../../usb/commands.h"
#include "../../widgets/wd_func.h"

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

TuneKIVCheck::TuneKIVCheck(ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(parent)
{
    TKIV = kiv;
    CKIV = ckiv;
    S2Config = TKIV->getS2Config();
    m_tuneStep = 1;
    SetupUI();
}

void TuneKIVCheck::SetupUI() { }

void TuneKIVCheck::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("3. Задание режима конфигурирования модуля...");
    lbls.append("4. Установка новой конфигурации...");
    lbls.append("5. Отображение схемы подключения...");
    lbls.append("6. Ожидание 15 с...");
}

void TuneKIVCheck::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &AbstractTuneDialog::CheckPassword;
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::SaveWorkConfig);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::setSMode2);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::setNewConfig);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::showScheme);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::Wait15Seconds);
    pf[lbls.at(count++)] = func;
}

void TuneKIVCheck::FillBac(int bacnum) { Q_UNUSED(bacnum) }

void TuneKIVCheck::FillBackBac(int bacnum) { Q_UNUSED(bacnum) }

void TuneKIVCheck::GetBdAndFill() { }

Error::Msg TuneKIVCheck::SaveWorkConfig(int configblocknum)
{
    Q_UNUSED(configblocknum)
    if (Commands::GetFileWithRestore(CM_CONFIGFILE, S2Config) == Error::Msg::NoError)
        memcpy(&m_BciSaveBlock, &CKIV->Bci_block, sizeof(ConfigKIV::Bci));
    else
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

int TuneKIVCheck::ReadAnalogMeasurements() { return 0; }

void TuneKIVCheck::SetDefCoefs() { }

Error::Msg TuneKIVCheck::setSMode2() { return Commands::SetMode(0x02); }

Error::Msg TuneKIVCheck::setNewConfig()
{
    CKIV->Bci_block.Unom = 220;
    CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = 9000;
    return Commands::WriteFile(CM_CONFIGFILE, S2Config);
}

Error::Msg TuneKIVCheck::showScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/tunekiv.png")));
    lyout->addWidget(WDFunc::NewLBL(this, "1. Соберите схему подключения по вышеприведённой картинке;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "3. Включите питание источника сигналов. Задайте на имитаторе ИС АВМ-КИВ напряжение U1 равным 60,0 В,"
        "ток I1 равным 29 мА, tgδ равным +0,2 %;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника"
        "составляет 60 ± 0,25 В, ток – 0,500 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    lyout->addWidget(WDFunc::NewLBL(
        this, "6. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    lyout->addWidget(WDFunc::NewLBL(this, "7. Установите на магазине сопротивлений сопротивление 100,0 Ом."));
    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", dlg, SLOT(close())));
    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", dlg, SLOT(close())));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", this, SLOT(CancelTune()));
    dlg->exec();
    return Error::Msg::NoError;
}
