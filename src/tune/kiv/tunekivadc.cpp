#include "tunekivadc.h"

#include "../../interfaces/baseinterface.h"
#include "../../s2/configv.h"
#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"
#include "../s2/s2util.h"
#include "../tunesteps.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/stdfunc.h>

using namespace Interface;

TuneKIVADC::TuneKIVADC(ConfigV *config, int tuneStep, QWidget *parent) : AbstractTuneDialog(config, tuneStep, parent)
{
    m_bac = new BacA284(this);
    m_bac2 = new Bac2A284(this);
    m_bda = new BdaA284(this);
    m_bdain = new BdaIn(this);
    m_bd0 = new Bd0(this);
    setBac(m_bac);
    setBac(m_bac2);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BacWidgetIndex2 = addWidgetToTabWidget(m_bac2->widget(), "Настроечные параметры 2");
    m_BdainWidgetIndex = addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
    m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    //    m_isEnergoMonitorDialogCreated = false;
    m_curTuneStep = 0;
    setupUI();
}

void TuneKIVADC::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение предупреждения...", &TuneKIVADC::showPreWarning);
    addTuneFunc("Запрос настроечных параметров...", &AbstractTuneDialog::readTuneCoefs);
    addTuneFunc("Проверка настроечных параметров...", &TuneKIVADC::checkTuneCoefs);
    addTuneFunc("Задание режима конфигурирования модуля...", &TuneKIVADC::setSMode2);
    if (m_tuneStep == KIVTS_ADCU)
    {
        addTuneFunc("Регулировка...", &TuneKIVADC::ADCCoef1);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
    }
    if (m_tuneStep == KIVTS_ADCI)
    {
        addTuneFunc("Регулировка для Кацп = 1...", &TuneKIVADC::ADCCoef1);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 2...", &TuneKIVADC::ADCCoef2);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 4...", &TuneKIVADC::ADCCoef4);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 8...", &TuneKIVADC::ADCCoef8);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 16...", &TuneKIVADC::ADCCoef16);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 32...", &TuneKIVADC::ADCCoef32);
        addTuneFunc("Отображение диалога задания входных данных...", &TuneKIVADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка канала Tmk0...", &TuneKIVADC::Tmk0);
    }
    addTuneFunc("Запись настроечных коэффициентов и восстановление конфигурации...", &TuneKIVADC::SendBac);
    addTuneFunc("Проверка регулировки...", &TuneKIVADC::CheckTune);
}

Error::Msg TuneKIVADC::showPreWarning()
{
    //    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    QWidget *w = new QWidget(this);
    w->setFixedSize(800, 600);
    lyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/tunekiv1.png")));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 500 мА;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "3. Данный этап регулировки должен выполняться при температуре"
        "окружающего воздуха +20±7 °С. Если температура окружающего воздуха отличается от указанной,"
        "разместите модуль в термокамеру с диапазоном регулирования температуры "
        "от минус 20 до +60°С. Установите нормальное значение температуры "
        "в камере 20±5°С"));
    //    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    //    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    w->setLayout(lyout);

    if (!EMessageBox::next(this, w))
        CancelTune();
    //    dlg->setLayout(lyout);
    //    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    //    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::checkTuneCoefs()
{
    QVector<float *> tcoefs = { &m_bac->data()->KmU[0], &m_bac->data()->KmI1[0], &m_bac->data()->KmI2[0],
        &m_bac->data()->KmI4[0], &m_bac->data()->KmI8[0], &m_bac->data()->KmI16[0], &m_bac->data()->KmI32[0] };
#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
    {
        for (float *coef : tcoefs)
            if (!WDFunc::floatIsWithinLimits("коэффициента по току", *(coef + i), 1.0, 0.05))
                return Error::Msg::GeneralError;
    }
    if (!WDFunc::floatIsWithinLimits("коэффициента по частоте", m_bac2->data()->K_freq, 1.0, 0.05))
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        if (!WDFunc::floatIsWithinLimits("коэффициента по углу", m_bac->data()->DPsi[i], 0.0, 1.0))
            return Error::Msg::GeneralError;
    }
#endif
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::setSMode2()
{
    BaseInterface::iface()->writeCommand(Commands::C_SetMode, 0x02);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::ADCCoef(int coef)
{
    QMap<int, int> currentMap = { { 1, 290 }, { 2, 250 }, { 4, 140 }, { 8, 80 }, { 16, 40 }, { 32, 23 } };
    m_curTuneStep = coef;
    //  CKIV->Bci_block.Unom1 = 220;
    configV->setRecordValue({ S2Util::GetIdByName("Unom1"), float(220) });

    Error::Msg res = setADCCoef(coef);
    if (res != Error::Msg::NoError)
        return res;
    showRetomDialog(coef);
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    showTWTab(m_BdainWidgetIndex);
    emit setProgressSize(StdFunc::TuneRequestCount());
    for (int i = 0; i < 6; ++i)
    {
        m_bdainBlockData.IUefNat_filt[i] = 0.0;
        m_bdainBlockData.phi_next_f[i] = 0.0;
    }
    m_bdainBlockData.Frequency = 0.0;
    int count = 0;
    while ((!StdFunc::IsCancelled()) && (count < StdFunc::TuneRequestCount()))
    {
        m_bdain->readAndUpdate();
        if (checkBdaIn(currentMap[coef]))
        {
            for (int j = 0; j < 6; ++j)
            {
                m_bdainBlockData.IUefNat_filt[j] += m_bdain->data()->IUefNat_filt[j];
                m_bdainBlockData.phi_next_f[j] += m_bdain->data()->phi_next_f[j];
            }
            m_bdainBlockData.Frequency += m_bdain->data()->Frequency;
        }
        else
            return Error::Msg::GeneralError;
        ++count;
        emit setProgressCount(count);
        StdFunc::Wait(500);
    }
    for (int i = 0; i < 6; ++i)
    {
        m_bdainBlockData.IUefNat_filt[i] /= StdFunc::TuneRequestCount();
        m_bdainBlockData.phi_next_f[i] /= StdFunc::TuneRequestCount();
    }
    m_bdainBlockData.Frequency /= StdFunc::TuneRequestCount();
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::ADCCoef1()
{
    return ADCCoef(1);
}

Error::Msg TuneKIVADC::ADCCoef2()
{
    return (m_tuneStep == KIVTS_ADCI || m_tuneStep == KIVTS_ADCU) ? ADCCoef(2) : Error::Msg::ResEmpty;
}

Error::Msg TuneKIVADC::ADCCoef4()
{
    return (m_tuneStep == KIVTS_ADCI || m_tuneStep == KIVTS_ADCU) ? ADCCoef(4) : Error::Msg::ResEmpty;
}

Error::Msg TuneKIVADC::ADCCoef8()
{
    return (m_tuneStep == KIVTS_ADCI || m_tuneStep == KIVTS_ADCU) ? ADCCoef(8) : Error::Msg::ResEmpty;
}

Error::Msg TuneKIVADC::ADCCoef16()
{
    return (m_tuneStep == KIVTS_ADCI || m_tuneStep == KIVTS_ADCU) ? ADCCoef(16) : Error::Msg::ResEmpty;
}

Error::Msg TuneKIVADC::ADCCoef32()
{
    return (m_tuneStep == KIVTS_ADCI || m_tuneStep == KIVTS_ADCU) ? ADCCoef(32) : Error::Msg::ResEmpty;
}

Error::Msg TuneKIVADC::Tmk0()
{
    emit setProgressSize(5);
    int i = 0;
    double tmk0 = 0;
    showTWTab(m_Bd0WidgetIndex);
    while ((!StdFunc::IsCancelled()) && (i < 5))
    {
        m_bd0->readAndUpdate();
        tmk0 += m_bd0->data()->Tmk;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    m_bac->data()->Tmk0 = tmk0 / 5;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::SendBac()
{
    m_bac->updateWidget();
    if (writeTuneCoefs() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    if (!loadWorkConfig())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::CheckTune()
{
    EMessageBox::information(this,
        "После закрытия данного сообщения для завершения настройки нажмите Enter\nДля отказа от настройки нажмите Esc");
    //    QMessageBox::information(this, "Информация",
    //        "После закрытия данного сообщения для завершения настройки нажмите Enter\nДля отказа от настройки нажмите
    //        Esc");
    m_finished = false;
    while ((!StdFunc::IsCancelled()) && !m_finished)
    {
        m_bdain->readAndUpdate();
        StdFunc::Wait(500);
    }
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::setADCCoef(const int coef)
{
    const QMap<int, float> adcCoefMap { { 1, 9000 }, { 2, 4500 }, { 4, 2250 }, { 8, 1124 }, { 16, 562 }, { 32, 281 } };
    S2::FLOAT_3t value { adcCoefMap.value(coef), adcCoefMap.value(coef), adcCoefMap.value(coef) };
    configV->setRecordValue(S2Util::GetIdByName("C_Pasp_ID"), value);
    return BaseInterface::iface()->writeConfFileSync(configV->getConfig());
}

Error::Msg TuneKIVADC::showRetomDialog(int coef)
{
    QWidget *w = new QWidget(this);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "РЕТОМ"));
    vlyout->addWidget(WDFunc::newHLine(this));
    IULayout(RegType::RETOM, coef, vlyout);
    hlyout->addLayout(vlyout);
    hlyout->addWidget(WDFunc::newVLine(this));

    vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "ИМИТАТОР"));
    vlyout->addWidget(WDFunc::newHLine(this));
    IULayout(RegType::IMITATOR, coef, vlyout);
    hlyout->addLayout(vlyout);

    w->setLayout(hlyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

void TuneKIVADC::IULayout(RegType type, int coef, QVBoxLayout *lyout)
{
    struct retomStruct
    {
        QString I;
        QString EMRange;
        QString Ret10Coef;
    };
    QMap<int, retomStruct> retomCoefMap = {
        { 1, { "2,9 A", "500 mA", "30:3" } }, //
        { 2, { "2,5 A", "500 mA", "30:3" } }, //
        { 4, { "1,4 A", "250 mA", "30:3" } }, //
        { 8, { "8 A", "100 mA", "300:3" } },  //
        { 16, { "4 A", "50 mA", "300:3" } },  //
        { 32, { "2.3 A", "50 mA", "300:3" } } //
    };
    QMap<int, retomStruct> ImCoefMap = {
        { 1, { "290 mA", "500 mA", "30:3" } }, //
        { 2, { "250 mA", "500 mA", "30:3" } }, //
        { 4, { "140 mA", "250 mA", "30:6" } }, //
        { 8, { "80 mA", "100 mA", "30:6" } },  //
        { 16, { "40 mA", "50 mA", "1:1" } },   //
        { 32, { "23 mA", "50 mA", "1:1" } }    //
    };
    QMap<RegType, QMap<int, retomStruct>> map = { { RegType::IMITATOR, ImCoefMap }, { RegType::RETOM, retomCoefMap } };

    QString tmps;
    if (type == RegType::IMITATOR)
        tmps = "Установите на имитаторе АВМ-КИВ tg = 2 %,\n"
               "Значения напряжений: 57.75 В";
    else
        tmps = "Задайте на РЕТОМ-51 режим однофазного напряжения и тока (ф. А)\n"
               "Угол между током и напряжением: 89.9 град.\n"
               "Значение напряжения ф. А: 57.75 В";
    if (m_tuneStep == KIVTS_ADCI)
        tmps += ", тока: " + map[type][coef].I;
    lyout->addWidget(WDFunc::NewLBL2(this, tmps));
    lyout->addWidget(
        WDFunc::NewLBL2(this, "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"));
    if (m_tuneStep == KIVTS_ADCI)
        lyout->addWidget(WDFunc::NewLBL2(this,
            "Предел измерения тока в Энергомониторе: " + map[type][coef].EMRange
                + "\nКоэффициент передачи РЕТ-10: " + map[type][coef].Ret10Coef));
    lyout->addStretch(100);
}

bool TuneKIVADC::checkBdaIn(int current)
{
#ifdef NO_LIMITS
    return true;
#endif
    for (int i = 0; i < 3; ++i)
    {
        if (WDFunc::floatIsWithinLimits("напряжения", m_bdain->data()->IUefNat_filt[i], 57.75, 3.0))
        {
            if (WDFunc::floatIsWithinLimits("напряжения", m_bdain->data()->IUeff_filtered[i], 57.75, 3.0))
            {
                if (m_tuneStep == KIVTS_ADCU)
                    continue;
                if (WDFunc::floatIsWithinLimits("тока", m_bdain->data()->IUefNat_filt[i + 3], current, 50))
                {
                    if (WDFunc::floatIsWithinLimits("тока", m_bdain->data()->IUeff_filtered[i + 3], current, 50))
                    {
                        if (WDFunc::floatIsWithinLimits("угла", m_bdain->data()->phi_next_f[i], 0, 1))
                        {
                            if (WDFunc::floatIsWithinLimits("угла", m_bdain->data()->phi_next_f[i + 3], 89, 3))
                                continue;
                        }
                    }
                }
            }
        }

        return false;
    }
    return true;
}

Error::Msg TuneKIVADC::showEnergomonitorInputDialog()
{
    EEditablePopup *popup = new EEditablePopup("Ввод значений сигналов c Энергомонитора");
    if (m_tuneStep == KIVTS_ADCU)
    {
        popup->addFloatParameter("Uэт, В", &m_midTuneStruct.uet);
        popup->addFloatParameter("fэт, Гц", &m_midTuneStruct.fet);
        popup->addFloatParameter("Yэт, град", &m_midTuneStruct.yet);
    }
    else
        popup->addFloatParameter("Iэт, мА", &m_midTuneStruct.iet);
    connect(popup, &EEditablePopup::accepted, this, &TuneKIVADC::CalcTuneCoefs);
    connect(popup, &EEditablePopup::cancelled, [] { return Error::GeneralError; });
    popup->execPopup();
    return Error::Msg::NoError;
}

void TuneKIVADC::CalcTuneCoefs()
{
    QMap<int, float *> kmimap
        = { { 1, &m_bac->data()->KmI1[0] }, { 2, &m_bac->data()->KmI2[0] }, { 4, &m_bac->data()->KmI4[0] },
              { 8, &m_bac->data()->KmI8[0] }, { 16, &m_bac->data()->KmI16[0] }, { 32, &m_bac->data()->KmI32[0] } };

    if (m_tuneStep == KIVTS_ADCI)
    {
        assert(kmimap.contains(m_curTuneStep));
        for (int i = 0; i < 3; ++i)
            *(kmimap.value(m_curTuneStep) + i)
                = *(kmimap.value(m_curTuneStep) + i) * m_midTuneStruct.iet / m_bdainBlockData.IUefNat_filt[i + 3];
    }
    else
    {
        for (int i = 0; i < 3; ++i)
            m_bac->data()->KmU[i] = m_bac->data()->KmU[i] * m_midTuneStruct.uet / m_bdainBlockData.IUefNat_filt[i];
        m_bac2->data()->K_freq = m_bac2->data()->K_freq * m_midTuneStruct.fet / m_bdainBlockData.Frequency;
        for (int i = 1; i < 3; ++i)
            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] - m_bdainBlockData.phi_next_f[i];
        for (int i = 3; i < 6; ++i)
            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] + m_midTuneStruct.yet - m_bdainBlockData.phi_next_f[i];
    }
}
