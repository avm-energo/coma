#include "tunekivcheck.h"

#include "../../gen/files.h"
#include "../../interfaces/protocom.h"
#include "../../widgets/wd_func.h"

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

TuneKIVCheck::TuneKIVCheck(int tuneStep, ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent)
    : AbstractTuneDialog(tuneStep, parent)
{
    TKIV = kiv;
    CKIV = ckiv;
    //    S2Config = TKIV->getS2Config();
    //    m_tuneStep = 1;
    //    SetupUI();
    //    addDataBlock()
}

// void TuneKIVCheck::SetupUI()
//{
//}

void TuneKIVCheck::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Сохранение текущей конфигурации...");
    m_messages.append("3. Задание режима конфигурирования модуля...");
    m_messages.append("4. Установка новой конфигурации...");
    m_messages.append("5. Отображение схемы подключения...");
    m_messages.append("6. Ожидание 15 с...");
}

void TuneKIVCheck::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::saveWorkConfig);
    m_tuneFunctions[m_messages.at(count++)] = func;
    //    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::setSMode2);
    //    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::setNewConfig);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::showScheme);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::Wait15Seconds);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

QWidget *TuneKIVCheck::MainUI()
{
    return nullptr;
}

Error::Msg TuneKIVCheck::saveWorkConfig()
{
}

void TuneKIVCheck::FillBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVCheck::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

// void TuneKIVCheck::GetBdAndFill() { }

// Error::Msg TuneKIVCheck::saveWorkConfig(int configblocknum)
//{
//    Q_UNUSED(configblocknum)
//    if (Commands::GetFileWithRestore(CM_CONFIGFILE, S2Config) == Error::Msg::NoError)
//        memcpy(&m_BciSaveBlock, &CKIV->Bci_block, sizeof(ConfigKIV::Bci));
//    else
//        return Error::Msg::GeneralError;
//    return Error::Msg::NoError;
//}

// int TuneKIVCheck::ReadAnalogMeasurements()
//{
//    return 0;
//}

// void TuneKIVCheck::SetDefCoefs()
//{
//}

Error::Msg TuneKIVCheck::setSMode2()
{
    iface()->writeCommand(Queries::QUSB_SetMode, 0x02);
    //    return Commands::SetMode(0x02);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVCheck::setNewConfig()
{
    CKIV->Bci_block.Unom = 220;
    CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = 9000;

    return iface()->writeConfFileSync();
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
    lyout->addWidget(WDFunc::NewPB2(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB2(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", this, &TuneKIVCheck::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}
