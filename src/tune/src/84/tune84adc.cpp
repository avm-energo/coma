#include "tune/84/tune84adc.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/stdfunc.h>
#include <tune/tunesteps.h>
#include <widgets/epopup.h>
#include <widgets/waitwidget.h>
#include <widgets/wd_func.h>

using namespace Interface;

Tune84ADC::Tune84ADC(int tuneStep, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(tuneStep, device, parent)
    , m_bac(new BacA284(this))
    , m_bac2(new Bac2A284(this))
    , m_bda(new BdaA284(this))
    , m_bdain(new BdaIn(this))
    , m_bd0(new Bd0(this))
{
    m_bac->setup(m_device->getUID(), m_sync);
    m_bac2->setup(m_device->getUID(), m_sync);
    m_bda->setup(m_device->getUID(), m_sync);
    m_bdain->setup(m_device->getUID(), m_sync);
    m_bd0->setup(m_device->getUID(), m_sync);

    setBac(m_bac);
    setBac(m_bac2);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac2->widget(), "Настроечные параметры 2");
    m_BdainWidgetIndex = addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
    m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    m_isEnergoMonitorDialogCreated = false;
    m_curTuneStep = 0;
    setupUI();
}

void Tune84ADC::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение предупреждения...", &Tune84ADC::showPreWarning);
    addTuneFunc("Запрос настроечных параметров...", &AbstractTuneDialog::readTuneCoefs);
    addTuneFunc("Проверка настроечных параметров...", &Tune84ADC::checkTuneCoefs);
    addTuneFunc("Задание режима конфигурирования модуля...", &Tune84ADC::setSMode2);
    if (m_tuneStep == TS84_ADCU)
    {
        addTuneFunc("Регулировка...", &Tune84ADC::ADCCoef1);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
    }
    if (m_tuneStep == TS84_ADCI)
    {
        addTuneFunc("Регулировка для Кацп = 1...", &Tune84ADC::ADCCoef1);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 2...", &Tune84ADC::ADCCoef2);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 4...", &Tune84ADC::ADCCoef4);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 8...", &Tune84ADC::ADCCoef8);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 16...", &Tune84ADC::ADCCoef16);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка для Кацп = 32...", &Tune84ADC::ADCCoef32);
        addTuneFunc("Отображение диалога задания входных данных...", &Tune84ADC::showEnergomonitorInputDialog);
        addTuneFunc("Регулировка канала Tmk0...", &Tune84ADC::Tmk0);
    }
    addTuneFunc("Запись настроечных коэффициентов и восстановление конфигурации...", &Tune84ADC::SendBac);
    addTuneFunc("Проверка регулировки...", &Tune84ADC::CheckTune);
}

Error::Msg Tune84ADC::showPreWarning()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QWidget *w = new QWidget(this);
    lyout->addWidget(WDFunc::NewIcon(this, ":/tunes/tunekiv1.png"));
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
    w->setLayout(lyout);

    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::checkTuneCoefs()
{
    QVector<float *> tcoefs = { &m_bac->data()->KmU[0], &m_bac->data()->KmI1[0], &m_bac->data()->KmI2[0],
        &m_bac->data()->KmI4[0], &m_bac->data()->KmI8[0], &m_bac->data()->KmI16[0], &m_bac->data()->KmI32[0] };
    for (int i = 0; i < 3; ++i)
    {
        foreach (float *coef, tcoefs)
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
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::setSMode2()
{
    m_async->writeCommand(Commands::C_SetMode, 0x02);
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::ADCCoef(int coef)
{
    QMap<int, int> currentMap = { { 1, 290 }, { 2, 250 }, { 4, 140 }, { 8, 80 }, { 16, 40 }, { 32, 23 } };
    m_curTuneStep = coef;
    config.setRecord("Unom1", float(220));

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

Error::Msg Tune84ADC::ADCCoef1()
{
    return ADCCoef(1);
}

Error::Msg Tune84ADC::ADCCoef2()
{
    return (m_tuneStep == TS84_ADCI || m_tuneStep == TS84_ADCU) ? ADCCoef(2) : Error::Msg::ResEmpty;
}

Error::Msg Tune84ADC::ADCCoef4()
{
    return (m_tuneStep == TS84_ADCI || m_tuneStep == TS84_ADCU) ? ADCCoef(4) : Error::Msg::ResEmpty;
}

Error::Msg Tune84ADC::ADCCoef8()
{
    return (m_tuneStep == TS84_ADCI || m_tuneStep == TS84_ADCU) ? ADCCoef(8) : Error::Msg::ResEmpty;
}

Error::Msg Tune84ADC::ADCCoef16()
{
    return (m_tuneStep == TS84_ADCI || m_tuneStep == TS84_ADCU) ? ADCCoef(16) : Error::Msg::ResEmpty;
}

Error::Msg Tune84ADC::ADCCoef32()
{
    return (m_tuneStep == TS84_ADCI || m_tuneStep == TS84_ADCU) ? ADCCoef(32) : Error::Msg::ResEmpty;
}

Error::Msg Tune84ADC::Tmk0()
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

Error::Msg Tune84ADC::SendBac()
{
    m_bac->updateWidget();
    if (writeTuneCoefs() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    if (loadWorkConfig() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::CheckTune()
{
    EMessageBox::information(this,
        "После закрытия данного сообщения для завершения настройки нажмите Enter\nДля отказа от настройки нажмите Esc");
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

Error::Msg Tune84ADC::setADCCoef(int coef)
{
    QMap<int, float> adcCoefMap = { { 1, 9000 }, { 2, 4500 }, { 4, 2250 }, { 8, 1124 }, { 16, 562 }, { 32, 281 } };
    const auto adcCoef = adcCoefMap.value(coef);
    config.setRecord("C_Pasp_ID", S2::FLOAT_3t { adcCoef, adcCoef, adcCoef });
    const auto s2file = config.toByteArray();
    return m_sync->writeFileSync(S2::FilesEnum::Config, s2file);
}

Error::Msg Tune84ADC::showRetomDialog(int coef)
{
    struct retomStruct
    {
        double i;
        double range;
        QString ret10_imit;
        QString ret10_retom;
    };

    QMap<int, retomStruct> retomCoefMap = {
        { 1, { 290, 500, "30:3", "30:3" } }, //
        { 2, { 250, 250, "30:3", "30:3" } }, //
        { 4, { 140, 250, "30:6", "30:3" } }, //
        { 8, { 80, 100, "30:6", "300:3" } }, //
        { 16, { 40, 50, "1:1", "300:3" } },  //
        { 32, { 23, 50, "1:1", "300:3" } }   //
    };

    QWidget *w = new QWidget(this);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "РЕТОМ"));
    vlyout->addWidget(WDFunc::newHLine(this));
    QString tmps;
    tmps = "Задайте на РЕТОМ-51 трёхфазный режим токов и напряжений (Uabc, Iabc)\n"
           "Угол между токами и напряжениями: 89.9 град.\n"
           "Значения напряжений: 57.75 В";
    if (m_tuneStep == TS84_ADCI)
        tmps += ", токов: " + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА";
    vlyout->addWidget(WDFunc::NewLBL2(this, tmps));
    vlyout->addWidget(
        WDFunc::NewLBL2(this, "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"));
    if (m_tuneStep == TS84_ADCI)
        vlyout->addWidget(WDFunc::NewLBL2(this,
            "Предел измерения тока в Энергомониторе: " + QString::number(retomCoefMap[coef].range, 'f', 2)
                + " мА.\nКоэффициент передачи РЕТ-10: " + retomCoefMap[coef].ret10_retom));
    hlyout->addLayout(vlyout);
    hlyout->addWidget(WDFunc::newVLine(this));
    vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "ИМИТАТОР"));
    vlyout->addWidget(WDFunc::newHLine(this));
    tmps = "Установите на имитаторе АВМ-КИВ tg = 2 %,\n"
           "Значения напряжений: 57.75 В";
    if (m_tuneStep == TS84_ADCI)
        tmps += ", токов: " + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА";
    vlyout->addWidget(WDFunc::NewLBL2(this, tmps));
    vlyout->addWidget(
        WDFunc::NewLBL2(this, "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"));
    if (m_tuneStep == TS84_ADCI)
        vlyout->addWidget(WDFunc::NewLBL2(this,
            "Предел измерения тока в Энергомониторе: " + QString::number(retomCoefMap[coef].range, 'f', 2)
                + " мА.\nКоэффициент передачи РЕТ-10: " + retomCoefMap[coef].ret10_imit));
    hlyout->addLayout(vlyout);
    w->setLayout(hlyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    if (m_tuneStep == TS84_ADCU)
        StdFunc::Wait(1000);
    return Error::Msg::NoError;
}

bool Tune84ADC::checkBdaIn(int current)
{
    for (int i = 0; i < 3; ++i)
    {
        if (WDFunc::floatIsWithinLimits("напряжения", m_bdain->data()->IUefNat_filt[i], 57.75, 3.0))
        {
            if (WDFunc::floatIsWithinLimits("напряжения", m_bdain->data()->IUeff_filtered[i], 57.75, 3.0))
            {
                if (m_tuneStep == TS84_ADCU)
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

Error::Msg Tune84ADC::showEnergomonitorInputDialog()
{
    EEditablePopup *popup = new EEditablePopup("Ввод значений сигналов c Энергомонитора");
    if (m_tuneStep == TS84_ADCU)
    {
        popup->addFloatParameter("Uэт, В", &m_midTuneStruct.uet);
        popup->addFloatParameter("fэт, Гц", &m_midTuneStruct.fet);
        popup->addFloatParameter("Yэт, град", &m_midTuneStruct.yet);
    }
    else
        popup->addFloatParameter("Iэт, мА", &m_midTuneStruct.iet);
    connect(popup, &EEditablePopup::accepted, this, &Tune84ADC::CalcTuneCoefs);
    connect(popup, &EEditablePopup::cancelled, [] { return Error::GeneralError; });
    popup->execPopup();
    return Error::Msg::NoError;
}

void Tune84ADC::CalcTuneCoefs()
{
    QMap<int, float *> kmimap
        = { { 1, &m_bac->data()->KmI1[0] }, { 2, &m_bac->data()->KmI2[0] }, { 4, &m_bac->data()->KmI4[0] },
              { 8, &m_bac->data()->KmI8[0] }, { 16, &m_bac->data()->KmI16[0] }, { 32, &m_bac->data()->KmI32[0] } };

    if (m_tuneStep == TS84_ADCI)
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
