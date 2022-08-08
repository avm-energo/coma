#include "tune82adc.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../s2/configv.h"
#include "../../s2/s2.h"
#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"
#include "../mip.h"
#include "../tunesteps.h"

#include <QMessageBox>
#include <QVBoxLayout>

Tune82ADC::Tune82ADC(ConfigV *config, Modules::MezzanineBoard type, int tuneStep, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    m_typeM = type;
    m_bac = new Bac82(this);
    m_bd1 = new Bd182(type, this);
    m_bda = new Bda82(this);
    m_bd0 = new Bd0(this);
    setBac(m_bac);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BdaWidgetIndex = addWidgetToTabWidget(m_bda->widget(), "Текущие данные");
    m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    //    m_isEnergoMonitorDialogCreated = false;
    m_curTuneStep = 0;
    setupUI();
}

void Tune82ADC::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Установка настроечных коэффициентов по умолчанию...", &Tune82ADC::setDefBac);
    addTuneFunc("Получение текущих аналоговых данных и их проверка...", &Tune82ADC::getAnalogData);
    addTuneFunc("Сохранение значений фильтра...", &Tune82ADC::saveUeff);
    //    addTuneFunc("Установка коэффициентов...");

    addTuneFunc("7.3.3. Расчёт коррекции смещений сигналов по фазе...", &Tune82ADC::calcPhaseCorrection);
    /*    addTuneFunc("7.3.4. Расчёт коррекции по частоте...");
        addTuneFunc("Регулировка канала Tmk0...");
        addTuneFunc("Запись настроечных коэффициентов и восстановление конфигурации...");
        addTuneFunc("Проверка регулировки..."); */
}

Error::Msg Tune82ADC::setDefBac()
{
    m_bac->setDefBlock();
    return Error::NoError;
}

Error::Msg Tune82ADC::getAnalogData()
{
    waitNSeconds(1);
    m_bd1->readBlockFromModule();
    waitNSeconds(1);
    return m_bda->checkValues(m_typeM, { 1, 1, 1, 1, 1, 1 });
}

Error::Msg Tune82ADC::saveUeff()
{
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i = 0; i < 6; i++)
        IUefNat_filt_old[i] = m_bd1->data()->IUefNat_filt[i];
    return Error::Msg::NoError;
}

Error::Msg Tune82ADC::calcPhaseCorrection()
{
    Mip::MipDataStruct mipdata;
    Mip *mip = new Mip(false, m_typeM, this);
    DataTypes::FLOAT_6t inom = configV->getRecord(S2::GetIdByName("I2nom")).value<DataTypes::FLOAT_6t>();
    assert(inom.size() > 3);
    mip->takeOneMeasurement(inom.at(3));

    float phiMip[6];
    GED_Type = TD_GED_D;

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
