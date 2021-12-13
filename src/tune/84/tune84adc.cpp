#include "tune84adc.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"
#include "../gen/configv.h"
#include "../tunesteps.h"

#include <QMessageBox>
#include <QVBoxLayout>

Tune84ADC::Tune84ADC(ConfigV *config, int tuneStep, QWidget *parent) : AbstractTuneDialog(config, tuneStep, parent)
{

    m_bac = new Bac;
    m_bda = new Bda;
    m_bdain = new BdaIn;
    m_bd0 = new Bd0;
    SetBac(m_bac);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BdainWidgetIndex = addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
    m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    m_isEnergoMonitorDialogCreated = false;
    m_curTuneStep = 0;
    SetupUI();
}

void Tune84ADC::setMessages()
{
    m_messages.append("Ввод пароля...");
    m_messages.append("Сохранение текущей конфигурации...");
    m_messages.append("Отображение предупреждения...");
    m_messages.append("Запрос настроечных параметров...");
    m_messages.append("Проверка настроечных параметров...");
    m_messages.append("Задание режима конфигурирования модуля...");
    m_messages.append("Регулировка для Кацп = 1...");
    m_messages.append("Отображение диалога задания входных данных...");
    if (m_tuneStep == TS84_ADCI)
    {
        m_messages.append("Регулировка для Кацп = 2...");
        m_messages.append("Отображение диалога задания входных данных...");
        m_messages.append("Регулировка для Кацп = 4...");
        m_messages.append("Отображение диалога задания входных данных...");
        m_messages.append("Регулировка для Кацп = 8...");
        m_messages.append("Отображение диалога задания входных данных...");
        m_messages.append("Регулировка для Кацп = 16...");
        m_messages.append("Отображение диалога задания входных данных...");
        m_messages.append("Регулировка для Кацп = 32...");
        m_messages.append("Отображение диалога задания входных данных...");
        m_messages.append("Регулировка канала Tmk0...");
    }
    m_messages.append("Запись настроечных коэффициентов и восстановление конфигурации...");
    m_messages.append("Проверка регулировки...");
}

void Tune84ADC::setTuneFunctions()
{
    m_tuneFunctions.push_back(
        reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword));
    m_tuneFunctions.push_back(
        reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig));
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showPreWarning);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::readTuneCoefs);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::checkTuneCoefs);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::setSMode2);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef1);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions.push_back(func);
    if (m_tuneStep == TS84_ADCI)
    {
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef2);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef4);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef8);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef16);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef32);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
        m_tuneFunctions.push_back(func);
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::Tmk0);
        m_tuneFunctions.push_back(func);
    }
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::SendBac);
    m_tuneFunctions.push_back(func);
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::CheckTune);
    m_tuneFunctions.push_back(func);
}

Error::Msg Tune84ADC::showPreWarning()
{
    //    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    QWidget *w = new QWidget(this);
    lyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/tunekiv1.png")));
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

    if (!EMessageBox::next(w))
        CancelTune();
    //    dlg->setLayout(lyout);
    //    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    //    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::checkTuneCoefs()
{
    QVector<float *> tcoefs = { &m_bac->data()->KmU[0], &m_bac->data()->KmI1[0], &m_bac->data()->KmI2[0],
        &m_bac->data()->KmI4[0], &m_bac->data()->KmI8[0], &m_bac->data()->KmI16[0], &m_bac->data()->KmI32[0] };
    for (int i = 0; i < 3; ++i)
    {
        foreach (float *coef, tcoefs)
            if (!WDFunc::floatIsWithinLimits(this, "коэффициента по току", *(coef + i), 1.0, 0.05))
                return Error::Msg::GeneralError;
    }
    if (!WDFunc::floatIsWithinLimits(this, "коэффициента по частоте", m_bac->data()->K_freq, 1.0, 0.05))
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        if (!WDFunc::floatIsWithinLimits(this, "коэффициента по углу", m_bac->data()->DPsi[i], 0.0, 1.0))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::setSMode2()
{
    BaseInterface::iface()->writeCommand(Queries::QUSB_SetMode, 0x02);
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::ADCCoef(int coef)
{
    QMap<int, int> currentMap = { { 1, 290 }, { 2, 250 }, { 4, 140 }, { 8, 80 }, { 16, 40 }, { 32, 23 } };
    m_curTuneStep = coef;

    configV->setRecordValue({ BciNumber::Unom1, float(220) });
    //  CKIV->Bci_block.Unom1 = 220;

    Error::Msg res = setADCCoef(coef);
    if (res != Error::Msg::NoError)
        return res;
    showRetomDialog(coef);
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    showTWTab(m_BdainWidgetIndex);
    emit setProgressSize(StdFunc::tuneRequestCount());
    for (int i = 0; i < 6; ++i)
    {
        m_bdainBlockData.IUefNat_filt[i] = 0.0;
        m_bdainBlockData.phi_next_f[i] = 0.0;
    }
    m_bdainBlockData.Frequency = 0.0;
    int count = 0;
    while ((!StdFunc::isCancelled()) && (count < StdFunc::tuneRequestCount()))
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
        m_bdainBlockData.IUefNat_filt[i] /= StdFunc::tuneRequestCount();
        m_bdainBlockData.phi_next_f[i] /= StdFunc::tuneRequestCount();
    }
    m_bdainBlockData.Frequency /= StdFunc::tuneRequestCount();
    if (StdFunc::isCancelled())
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
    while ((!StdFunc::isCancelled()) && (i < 5))
    {
        m_bd0->readAndUpdate();
        tmk0 += m_bd0->data()->Tmk;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    m_bac->data()->Tmk0 = tmk0 / 5;
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::SendBac()
{
    m_bac->updateWidget();
    if (writeTuneCoefs() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    if (!loadWorkConfig())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::CheckTune()
{
    EMessageBox::information(
        "После закрытия данного сообщения для завершения настройки нажмите Enter\nДля отказа от настройки нажмите Esc");
    m_finished = false;
    while ((!StdFunc::isCancelled()) && !m_finished)
    {
        m_bdain->readAndUpdate();
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::setADCCoef(int coef)
{
    QMap<int, float> adcCoefMap = { { 1, 9000 }, { 2, 4500 }, { 4, 2250 }, { 8, 1124 }, { 16, 562 }, { 32, 281 } };
    configV->setRecordValue({ BciNumber::C_Pasp_ID,
        DataTypes::FLOAT_3t({ adcCoefMap.value(coef), adcCoefMap.value(coef), adcCoefMap.value(coef) }) });

    // CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = adcCoefMap[coef];

    return BaseInterface::iface()->writeConfFileSync(configV->getConfig());
}

Error::Msg Tune84ADC::showRetomDialog(int coef)
{
    struct retomStruct
    {
        double i;
        double range;
        QString ret10c;
    };

    QMap<int, retomStruct> retomCoefMap
        = { { 1, { 290, 2.5, "30:3" } }, { 2, { 250, 2.5, "30:3" } }, { 4, { 140, 1, "30:6" } },
              { 8, { 80, 0.5, "30:6" } }, { 16, { 40, 0.1, "1:1" } }, { 32, { 23, 0.05, "1:1" } } };

    QWidget *w = new QWidget(this);
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "РЕТОМ"));
    vlyout->addWidget(WDFunc::newHLine(this));
    QString tmps;
    tmps = "Задайте на РЕТОМ-51 трёхфазный режим токов и напряжений (Uabc, Iabc)\n"
           "Угол между токами и напряжениями: 89.9 град.\n"
           "Значения напряжений: 57.75 В";
    if (m_tuneStep == KIVTS_ADCI)
        tmps += ", токов: " + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА";
    vlyout->addWidget(WDFunc::NewLBL2(this, tmps));
    vlyout->addWidget(
        WDFunc::NewLBL2(this, "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"));
    if (m_tuneStep == KIVTS_ADCI)
        vlyout->addWidget(WDFunc::NewLBL2(this,
            "Предел измерения тока в Энергомониторе: " + QString::number(retomCoefMap[coef].range, 'f', 2)
                + " А.\nКоэффициент передачи РЕТ-10 30:3"));
    hlyout->addLayout(vlyout);
    hlyout->addWidget(WDFunc::newVLine(this));
    vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "ИМИТАТОР"));
    vlyout->addWidget(WDFunc::newHLine(this));
    tmps = "Установите на имитаторе АВМ-КИВ tg = 2 %,\n"
           "Значения напряжений: 57.75 В";
    if (m_tuneStep == KIVTS_ADCI)
        tmps += ", токов: " + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА";
    vlyout->addWidget(WDFunc::NewLBL2(this, tmps));
    vlyout->addWidget(
        WDFunc::NewLBL2(this, "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"));
    if (m_tuneStep == KIVTS_ADCI)
        vlyout->addWidget(WDFunc::NewLBL2(this,
            "Предел измерения тока в Энергомониторе: " + QString::number(retomCoefMap[coef].range, 'f', 2)
                + " А.\nКоэффициент передачи РЕТ-10 " + retomCoefMap[coef].ret10c));
    hlyout->addLayout(vlyout);
    w->setLayout(hlyout);
    if (!EMessageBox::next(w))
        CancelTune();
    //    lyout->addWidget(WDFunc::NewLBL2(this, tmps));
    //    QPushButton *pb = new QPushButton("Готово");
    //    connect(pb, &QAbstractButton::clicked, dlg, &QWidget::close);
    //    lyout->addWidget(pb);
    //    pb = new QPushButton("Отмена");
    //    connect(pb, &QAbstractButton::clicked, this, &AbstractTuneDialog::CancelTune);
    //    connect(pb, &QAbstractButton::clicked, dlg, &QWidget::close);
    //    lyout->addWidget(pb);
    //    dlg->setLayout(lyout);
    //    dlg->exec();
    return Error::Msg::NoError;
}

bool Tune84ADC::checkBdaIn(int current)
{
    for (int i = 0; i < 3; ++i)
    {
        if (WDFunc::floatIsWithinLimits(this, "напряжения", m_bdain->data()->IUefNat_filt[i], 57.75, 3.0))
        {
            if (WDFunc::floatIsWithinLimits(this, "напряжения", m_bdain->data()->IUeff_filtered[i], 57.75, 3.0))
            {
                if (m_tuneStep == TS84_ADCU)
                    continue;
                if (WDFunc::floatIsWithinLimits(this, "тока", m_bdain->data()->IUefNat_filt[i + 3], current, 50))
                {
                    if (WDFunc::floatIsWithinLimits(this, "тока", m_bdain->data()->IUeff_filtered[i + 3], current, 50))
                    {
                        if (WDFunc::floatIsWithinLimits(this, "угла", m_bdain->data()->phi_next_f[i], 0, 1))
                        {
                            if (WDFunc::floatIsWithinLimits(this, "угла", m_bdain->data()->phi_next_f[i + 3], 89, 3))
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
    if ((m_curTuneStep != 1) && (m_tuneStep == TS84_ADCU)) // only the first input has any means
        return Error::Msg::ResEmpty;
    EEditablePopup *popup = new EEditablePopup("Ввод значений сигналов c Энергомонитора");
    if (m_tuneStep == TS84_ADCU)
    {
        popup->addFloatParameter("Uэт, В", m_midTuneStruct.uet);
        popup->addFloatParameter("fэт, Гц", m_midTuneStruct.fet);
        popup->addFloatParameter("Yэт, град", m_midTuneStruct.yet);
    }
    else
        popup->addFloatParameter("Iэт, мА", m_midTuneStruct.iet);
    return Error::Msg::NoError;
}

void Tune84ADC::CalcTuneCoefs()
{
    QMap<int, float *> kmimap
        = { { 1, &m_bac->data()->KmI1[0] }, { 2, &m_bac->data()->KmI2[0] }, { 4, &m_bac->data()->KmI4[0] },
              { 8, &m_bac->data()->KmI8[0] }, { 16, &m_bac->data()->KmI16[0] }, { 32, &m_bac->data()->KmI32[0] } };
    //    float uet, iet, yet, fet;
    //    bool ok;

    if (m_tuneStep == TS84_ADCI)
    {
        //        iet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneI"), &ok);
        //        if (ok)
        //        {
        assert(kmimap.contains(m_curTuneStep));
        for (int i = 0; i < 3; ++i)
            *(kmimap.value(m_curTuneStep) + i)
                = *(kmimap.value(m_curTuneStep) + i) * m_midTuneStruct.iet / m_bdainBlockData.IUefNat_filt[i + 3];
        QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
        if (dlg != nullptr)
            dlg->close();
        return;
        //        }
        //        else
        //        {
        //            EMessageBox::error("Не задано значение тока!");
        //        }
    }
    else
    {
        //        uet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneU"), &ok);
        //        if (ok)
        //        {
        //            yet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneY"), &ok);
        //            if (ok)
        //            {
        //                fet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneF"), &ok);
        //                if (ok)
        //                {
        for (int i = 0; i < 3; ++i)
            m_bac->data()->KmU[i] = m_bac->data()->KmU[i] * m_midTuneStruct.uet / m_bdainBlockData.IUefNat_filt[i];
        m_bac->data()->K_freq = m_bac->data()->K_freq * m_midTuneStruct.fet / m_bdainBlockData.Frequency;
        for (int i = 1; i < 3; ++i)
            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] - m_bdainBlockData.phi_next_f[i];
        for (int i = 3; i < 6; ++i)
            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] + m_midTuneStruct.yet - m_bdainBlockData.phi_next_f[i];
        //                    QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
        //                    if (dlg != nullptr)
        //                        dlg->close();
        //                    return;
        //                }
        //            }
        //        }
        //        QMessageBox::critical(this, "Ошибка!", "Не задано одно из значений!");
        //        return;
    }
    //    StdFunc::cancel();
}
