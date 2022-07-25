#include "tune82adc.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../s2/configv.h"
#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"
#include "../tunesteps.h"

#include <QMessageBox>
#include <QVBoxLayout>

Tune82ADC::Tune82ADC(ConfigV *config, int tuneStep, QWidget *parent) : AbstractTuneDialog(config, tuneStep, parent)
{
    /*    m_bac = new Bac(this);
        m_bac->setup();
        m_bda = new Bda(this);
        //    m_bdain = new BdaIn(this);
        m_bd0 = new Bd0(this);
        setBac(m_bac);
        m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
        //    m_BdainWidgetIndex = addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
        m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
        //    m_isEnergoMonitorDialogCreated = false;
        m_curTuneStep = 0;
        setupUI(); */
}

void Tune82ADC::setMessages()
{
    m_messages.append("Ввод пароля...");
    m_messages.append("Сохранение текущей конфигурации...");
    m_messages.append("Установка настроечных коэффициентов по умолчанию...");
    m_messages.append("Получение текущих аналоговых данных и их проверка...");
    m_messages.append("Сохранение значений фильтра...");
    //    m_messages.append("Установка коэффициентов...");

    m_messages.append("7.3.3. Расчёт коррекции смещений сигналов по фазе...");
    m_messages.append("7.3.4. Расчёт коррекции по частоте...");
    m_messages.append("Регулировка канала Tmk0...");
    m_messages.append("Запись настроечных коэффициентов и восстановление конфигурации...");
    m_messages.append("Проверка регулировки...");
}

void Tune82ADC::setTuneFunctions()
{
    /*    m_tuneFunctions.push_back(
            reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword));
        m_tuneFunctions.push_back(
            reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig));
        Error::Msg (AbstractTuneDialog::*func)()
            = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::setDefBac);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::getAnalogData);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::saveUeff);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::calcPhaseCorrection);
        m_tuneFunctions.push_back(func);

        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::ADCCoef1);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::showEnergomonitorInputDialog);
        m_tuneFunctions.push_back(func);
        if (m_tuneStep == KIVTS_ADCI)
        {
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::ADCCoef2);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::showEnergomonitorInputDialog);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::ADCCoef4);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::showEnergomonitorInputDialog);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::ADCCoef8);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::showEnergomonitorInputDialog);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::ADCCoef16);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::showEnergomonitorInputDialog);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::ADCCoef32);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::showEnergomonitorInputDialog);
            m_tuneFunctions.push_back(func);
            func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::Tmk0);
            m_tuneFunctions.push_back(func);
        }
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::SendBac);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune82ADC::CheckTune);
        m_tuneFunctions.push_back(func); */
}

Error::Msg Tune82ADC::setDefBac()
{
    m_bac->setDefBlock();
}

Error::Msg Tune82ADC::getAnalogData()
{
    /*    waitNSeconds(1);
        m_bda->readBlockFromModule();
        waitNSeconds(1);
        return m_bda->checkValues(60.0, 1.0, 0.0, false); */
}

Error::Msg Tune82ADC::saveUeff()
{
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i = 0; i < 6; i++)
        IUefNat_filt_old[i] = m_bda->data()->IUefNat_filt[i];
    return Error::Msg::NoError;
}

Error::Msg Tune82ADC::calcPhaseCorrection()
{
    /*    float phiMip[6];
        GED_Type = TD_GED_D;
        WaitNSeconds(5);
        GetExternalData();
        Bac_newblock.DPsi[0] = 0;
        phiMip[0] = 0;
        phiMip[1] = RealData.dpsiU[0];
        phiMip[2] = RealData.dpsiU[0] + RealData.dpsiU[1];
        phiMip[3] = RealData.d[0];
        phiMip[4] = RealData.d[1] + RealData.dpsiU[0];
        phiMip[5] = RealData.d[2] + RealData.dpsiU[0] + RealData.dpsiU[1];
        int k = (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_82) ? 3 : 6;
        for (int i = 1; i < k; ++i)
            Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - phiMip[i] - Bda_block.phi_next_f[i];
        if (ModuleBSI::GetMType(BoardTypes::BT_MEZONIN) == Config::MTM_82)
        {
            for (int i = 3; i < 6; ++i)
                Bac_newblock.DPsi[i] = Bac_block.DPsi[i] - phiMip[i] - Bda_block.phi_next_f[i];
        }
    */
    return Error::Msg::NoError;
}

Error::Msg Tune82ADC::showPreWarning()
{
    //    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    QWidget *w = new QWidget(this);
    lyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/Tune821.png")));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "3. Данный этап регулировки должен выполняться при температуре"
        "окружающего воздуха +20±7 °С. Если температура окружающего воздуха отличается от указанной,"
        "разместите модуль в термокамеру с диапазоном регулирования температуры "
        "от минус 20 до +60°С. Установите нормальное значение температуры "
        "в камере 20±5°С"));
    //    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    //    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    w->setLayout(lyout);

    //    if (!EMessageBox::next(w))
    //        CancelTune();
    //    dlg->setLayout(lyout);
    //    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    //    dlg->exec();
    return Error::Msg::NoError;
}
