#include "tunekivcheck.h"

#include "../../gen/files.h"
#include "../../gen/stdfunc.h"
#include "../../interfaces/protocom.h"
#include "../../widgets/wd_func.h"

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

TuneKIVCheck::TuneKIVCheck(int tuneStep, TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(tuneStep, parent)
{
    TKIV = kiv;
    //    CKIV = ckiv;
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
    //    m_messages.append("3. Задание режима конфигурирования модуля...");
    m_messages.append("3. Установка новой конфигурации...");
    m_messages.append("4. Отображение схемы подключения...");
    m_messages.append("5. Ожидание 15 с...");
    m_messages.append("6. Проверка...");
}

void TuneKIVCheck::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig);
    m_tuneFunctions[m_messages.at(count++)] = func;
    //    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::setSMode2);
    //    m_tuneFunctions[m_messages.at(count++)] = func;
    //    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::setNewConfig);
    //    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::showScheme);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::Wait15Seconds);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::check);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

QWidget *TuneKIVCheck::MainUI()
{
    return nullptr;
}

void TuneKIVCheck::FillBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVCheck::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

// Error::Msg TuneKIVCheck::setSMode2()
//{
//    iface()->writeCommand(Queries::QUSB_SetMode, 0x02);
//    //    return Commands::SetMode(0x02);
//    return Error::Msg::NoError;
//}

Error::Msg TuneKIVCheck::showScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/tunekiv1.png")));
    lyout->addWidget(WDFunc::NewLBL(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "3. Включите питание источника сигналов. При работе с имитатором ИС АВМ-КИВ задайте напряжение U1 равным 60,0 "
        "В,"
        "ток I1 равным 29 мА, tgδ равным +0,2 %;\nПри работе с РЕТОМ-51 задайте напряжение U1 равным 60,0 В с углом 0 "
        "град.,"
        "ток I1 равным 5 А с углом 89,9 град., включите выходы РЕТОМ-51;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника"
        "составляет 60 ± 0,25 В, ток – 0,500 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    lyout->addWidget(WDFunc::NewLBL(
        this, "6. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    lyout->addWidget(WDFunc::NewLBL(this, "7. Установите на магазине сопротивлений сопротивление 100,0 Ом."));
    lyout->addWidget(WDFunc::NewPB2(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB2(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", this, &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVCheck::check()
{
    iface()->reqBlockSync(1, DataTypes::DataBlockTypes::BdaBlock, &TKIV->m_Bda, sizeof(TKIV->m_Bda));
    for (int i = 0; i < 6; ++i)
        if (!StdFunc::floatIsWithinLimits(TKIV->m_Bda.Ueff_ADC[i], 2150000.0, 150000.0))
            return Error::Msg::GeneralError;
    if (!StdFunc::floatIsWithinLimits(TKIV->m_Bda.Pt100, 1175.0, 120.0))
        return Error::Msg::GeneralError;
    if (!StdFunc::floatIsWithinLimits(TKIV->m_Bda.Frequency, 51.0, 0.05))
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}
