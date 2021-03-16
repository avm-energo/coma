#include "tune84adc.h"

#include "../../gen/colors.h"
#include "../../gen/s2.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>

Tune84ADC::Tune84ADC(int tuneStep, /*ConfigKIV *ckiv,*/ QWidget *parent) : AbstractTuneDialog(tuneStep, parent)
{
    //  CKIV = ckiv;
    m_bac = new Bac;
    m_bda = new Bda;
    m_bdain = new BdaIn;
    m_bd0 = new Bd0;
    SetBac(m_bac);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BdainWidgetIndex = addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
    m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    m_isEnergoMonitorDialogCreated = false;
    SetupUI();
}

void Tune84ADC::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Сохранение текущей конфигурации...");
    m_messages.append("3. Отображение предупреждения...");
    m_messages.append("4. Запрос настроечных параметров...");
    m_messages.append("5. Проверка настроечных параметров...");
    m_messages.append("6. Задание режима конфигурирования модуля...");
    m_messages.append("7. Регулировка для Кацп = 1...");
    m_messages.append("8. Отображение диалога задания входных данных...");
    m_messages.append("9. Регулировка для Кацп = 2...");
    m_messages.append("10. Отображение диалога задания входных данных...");
    m_messages.append("11. Регулировка для Кацп = 4...");
    m_messages.append("12. Отображение диалога задания входных данных...");
    m_messages.append("13. Регулировка для Кацп = 8...");
    m_messages.append("14. Отображение диалога задания входных данных...");
    m_messages.append("15. Регулировка для Кацп = 16...");
    m_messages.append("16. Отображение диалога задания входных данных...");
    m_messages.append("17. Регулировка для Кацп = 32...");
    m_messages.append("18. Отображение диалога задания входных данных...");
    m_messages.append("19. Регулировка канала Tmk0...");
    m_messages.append("20. Запись настроечных коэффициентов и восстановление конфигурации...");
    m_messages.append("21. Проверка регулировки...");
}

void Tune84ADC::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showPreWarning);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::readTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::checkTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::setSMode2);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef1);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef2);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef4);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef8);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef16);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::ADCCoef32);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::Tmk0);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::SendBac);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84ADC::CheckTune);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

Error::Msg Tune84ADC::showPreWarning()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

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
    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg Tune84ADC::checkTuneCoefs()
{
    QVector<float *> tcoefs = { &m_bac->data()->KmU[0], &m_bac->data()->KmI1[0], &m_bac->data()->KmI2[0],
        &m_bac->data()->KmI4[0], &m_bac->data()->KmI8[0], &m_bac->data()->KmI16[0], &m_bac->data()->KmI32[0] };
    for (int i = 0; i < 3; ++i)
    {
        foreach (float *coef, tcoefs)
            if (!StdFunc::floatIsWithinLimits(this, *(coef + i), 1.0, 0.05))
                return Error::Msg::GeneralError;
    }
    if (!StdFunc::floatIsWithinLimits(this, m_bac->data()->K_freq, 1.0, 0.05))
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        if (!StdFunc::floatIsWithinLimits(this, m_bac->data()->DPsi[i], 0.0, 1.0))
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
    S2::setRecordValue({ BciNumber::Unom1, float(220) });
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
    return ADCCoef(2);
}

Error::Msg Tune84ADC::ADCCoef4()
{
    return ADCCoef(4);
}

Error::Msg Tune84ADC::ADCCoef8()
{
    return ADCCoef(8);
}

Error::Msg Tune84ADC::ADCCoef16()
{
    return ADCCoef(16);
}

Error::Msg Tune84ADC::ADCCoef32()
{
    return ADCCoef(32);
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
    QMessageBox::information(this, "Информация",
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
    S2::setRecordValue({ BciNumber::C_Pasp_ID,
        DataTypes::FLOAT_3t({ adcCoefMap.value(coef), adcCoefMap.value(coef), adcCoefMap.value(coef) }) });
    // CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = adcCoefMap[coef];

    return BaseInterface::iface()->writeConfFileSync();
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
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL2(this,
        "Задайте на РЕТОМ-51 или имитаторе АВМ-КИВ трёхфазный режим токов и напряжений (Uabc, Iabc)"
        "Угол между токами и напряжениями: 89.9 град. (tg 2 % в имитаторе),\n"
        "Значения напряжений: 57.75 В, токов: "
            + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"
        "Предел измерения тока в Энергомониторе: "
            + QString::number(retomCoefMap[coef].range, 'f', 2)
            + " А.\n"
              "Коэффициент передачи РЕТ-10 для режима с имитатором: "
            + retomCoefMap[coef].ret10c + " (для РЕТОМ-51 коэффициент 30:3)."));
    QPushButton *pb = new QPushButton("Готово");
    connect(pb, SIGNAL(clicked()), dlg, SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked()), this, SLOT(CancelTune()));
    connect(pb, SIGNAL(clicked()), dlg, SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    return Error::Msg::NoError;
}

bool Tune84ADC::checkBdaIn(int current)
{
    for (int i = 0; i < 3; ++i)
    {
        if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUefNat_filt[i], 57.75, 3.0))
        {
            if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUeff_filtered[i], 57.75, 3.0))
            {
                if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUefNat_filt[i + 3], current, 50))
                {
                    if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUeff_filtered[i + 3], current, 50))
                    {
                        if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->phi_next_f[i], 0, 1))
                        {
                            if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->phi_next_f[i + 3], 89, 3))
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
    bool enabled = (m_curTuneStep > 1) ? false : true;
    if (!m_isEnergoMonitorDialogCreated)
    {
        QDialog *dlg = new QDialog(this);
        dlg->setObjectName("energomonitordlg");
        QVBoxLayout *vlyout = new QVBoxLayout;
        vlyout->addWidget(WDFunc::NewLBL2(this, "Ввод значений сигналов c Энергомонитора"));
        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Uэт, В", "ValuetuneU", enabled));
        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Iэт, мА", "ValuetuneI", true));
        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Yэт, град", "ValuetuneY", enabled));
        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "fэт, Гц:", "ValuetuneF", enabled));

        QPushButton *pb = new QPushButton("Продолжить");
        connect(pb, SIGNAL(clicked()), this, SLOT(CalcTuneCoefs()));
        vlyout->addWidget(pb);

        foreach (QString str, QStringList({ "U", "Y", "F" }))
            WDFunc::SetVisible(this, "Valuetune" + str, enabled);
        dlg->setLayout(vlyout);
        m_isEnergoMonitorDialogCreated = true;
        dlg->exec();
    }
    else
    {
        QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
        if (dlg != nullptr)
        {
            foreach (QString str, QStringList({ "U", "I", "Y", "F" }))
                WDFunc::SetLEData(this, "Valuetune" + str, "");
            foreach (QString str, QStringList({ "U", "Y", "F" }))
                WDFunc::SetVisible(this, "Valuetune" + str, enabled);
            //            WDFunc::SetEnabled(this, "ValuetuneF", enabled);
            //            WDFunc::SetEnabled(this, "ValuetuneY", enabled);
            dlg->exec();
        }
    }
    return Error::Msg::NoError;
}

void Tune84ADC::CalcTuneCoefs()
{
    QMap<int, float *> kmimap = { { 2, &m_bac->data()->KmI2[0] }, { 4, &m_bac->data()->KmI4[0] },
        { 8, &m_bac->data()->KmI8[0] }, { 16, &m_bac->data()->KmI16[0] }, { 32, &m_bac->data()->KmI32[0] } };
    float uet, iet, yet, fet;
    bool ok;
    bool checkuyf = (m_curTuneStep > 1) ? false : true;

    iet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneI"), &ok);
    if (ok)
    {
        if (checkuyf)
        {
            uet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneU"), &ok);
            if (ok)
            {
                yet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneY"), &ok);
                if (ok)
                {
                    fet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneF"), &ok);
                    if (ok)
                    {
                        for (int i = 0; i < 3; ++i)
                        {
                            m_bac->data()->KmU[i] = m_bac->data()->KmU[i] * uet / m_bdainBlockData.IUefNat_filt[i];
                            m_bac->data()->KmI1[i]
                                = m_bac->data()->KmI1[i] * iet / m_bdainBlockData.IUefNat_filt[i + 3];
                        }
                        m_bac->data()->K_freq = m_bac->data()->K_freq * fet / m_bdainBlockData.Frequency;
                        for (int i = 1; i < 3; ++i)
                            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] - m_bdainBlockData.phi_next_f[i];
                        for (int i = 3; i < 6; ++i)
                            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] + yet - m_bdainBlockData.phi_next_f[i];
                        QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
                        if (dlg != nullptr)
                            dlg->close();
                        return;
                    }
                }
            }
            QMessageBox::critical(this, "Ошибка!", "Не задано одно из значений!");
            return;
        }
        assert(kmimap.contains(m_curTuneStep));
        for (int i = 0; i < 3; ++i)
            *(kmimap.value(m_curTuneStep) + i)
                = *(kmimap.value(m_curTuneStep) + i) * iet / m_bdainBlockData.IUefNat_filt[i + 3];
        QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
        if (dlg != nullptr)
            dlg->close();
        return;
    }
    QMessageBox::critical(this, "Ошибка!", "Не задано одно из значений!");
    StdFunc::cancel();
}
