#include <QTime>
#include <QtMath>
#include <QTabWidget>
#include <QEventLoop>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QInputDialog>
#include <QCoreApplication>
#include <QFileDialog>
#include "tunedialoga1.h"
#include "../canal.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

TuneDialogA1::TuneDialogA1(QWidget *parent) :
    AbstractTuneDialog(parent)
{
    CA1 = new ConfigA1(S2Config);
    ChA1 = new CheckA1;
    SetBac(&Bac_block, BT_BASE, sizeof(Bac_block));
    SetupUI();
}

void TuneDialogA1::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Отображение схемы подключения...");
    lbls.append("6.3.1. Получение настроечных коэффициентов...");
    lbls.append("6.2. Проверка правильности измерения сигналов...");
    lbls.append("6.3.2.1. КПТ: получение блока данных и усреднение...");
    lbls.append("6.3.2.2. КПТ: ввод данных от энергомонитора...");
    lbls.append("6.3.2.3. КПТ: расчёт регулировочных коэффициентов...");
    lbls.append("6.3.3.1. КТС: подтверждение установки 80 Ом...");
    lbls.append("6.3.3.2. КТС: получение блока данных...");
    lbls.append("6.3.3.3. КТС: подтверждение установки 120 Ом...");
    lbls.append("6.3.3.4. КТС: получение блока данных и расчёт регулировочных коэффициентов...");
    lbls.append("6.3.4. КМТ2: подтверждение установки 4 мА...");
    lbls.append("6.3.5.1. КМТ2: получение блока данных...");
    lbls.append("6.3.5.2. КМТ2: подтверждение установки 20 мА...");
    lbls.append("6.3.5.3. КМТ2: получение блока данных и расчёт регулировочных коэффициентов...");
    lbls.append("6.3.6. КМТ1: подтверждение установки 4 мА...");
    lbls.append("6.3.7.1. КМТ1: получение блока данных...");
    lbls.append("6.3.7.2. КМТ1: подтверждение установки 20 мА...");
    lbls.append("6.3.7.3. КМТ1: получение блока данных и расчёт регулировочных коэффициентов...");
    lbls.append("6.3.8. Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("6.3.9. Проверка аналоговых данных...");
}

void TuneDialogA1::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &AbstractTuneDialog::CheckPassword; // 1. Ввод пароля
    int (AbstractTuneDialog::*func)() = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::ShowScheme);
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_1); // 6.3.1. Получение настроечных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_2);
    pf[lbls.at(count++)] = func; // 6.2. Проверка правильности измерения сигналов переменного напряжения
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_2_1); // 6.3.2.1. КПТ: получение блока данных и усреднение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_2_2); // 6.3.2.2. КПТ: ввод данных от энергомонитора
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_2_3); // 6.3.2.3. КПТ: расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_3_1); // 6.3.3.1. КТС: подтверждение установки 80 Ом
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_3_2); // 6.3.3.2. КТС: получение блока данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_3_3); // 6.3.3.3. КТС: подтверждение установки 120 Ом
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_3_4); // 6.3.3.4. КТС: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_4); // 6.3.4. КМТ2: подтверждение установки 4 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_5_1); // 6.3.5.1. КМТ2: получение блока данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_5_2); // 6.3.5.2. КМТ2: подтверждение установки 20 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_5_3); // 6.3.5.3. КМТ2: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_6); // 6.3.6. КМТ1: подтверждение установки 4 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_7_1); // 6.3.7.1. КМТ1: получение блока данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_7_2); // 6.3.7.2. КМТ1: подтверждение установки 20 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_7_3); // 6.3.7.3. КМТ1: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_8); // 6.3.8. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1::Start6_3_9); // 6.3.9. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
}

void TuneDialogA1::SetupUI()
{
    QWidget *cp1 = TuneUI();
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *TuneTW = new QTabWidget;

    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(UCONFWCLR)+"; font: bold 10px;}";
    tmps = "QWidget {background-color: "+QString(UCONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);

    TuneTW->addTab(cp1,"Настройка прибора");
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные измерений");

    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Настроечные коэффициенты");
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[0]"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune0", "", ValuesLEFormat), 0, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[1]"), 0, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune1", "", ValuesLEFormat), 0, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "K_freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune2", "", ValuesLEFormat), 1, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "DPhy"), 1, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune3", "", ValuesLEFormat), 1, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Art"), 2, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune25", "", ValuesLEFormat), 2, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Brt"), 2, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune26", "", ValuesLEFormat), 2, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama1"), 3, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune27", "", ValuesLEFormat), 3, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma1"), 3, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune28", "", ValuesLEFormat), 3, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama2"), 4, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune29", "", ValuesLEFormat), 4, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma2"), 4, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune30", "", ValuesLEFormat), 4, 4, 1, 2);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addWidget(BottomUI());
    lyout->addStretch(1);
    cp2->setLayout(lyout);

    // CP3 - Данные измерений

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Данные измерений без настройки (Bda)");
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1->BdaW(this));
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    lyout->addWidget(ChA1->Bda_inW(this));
    gb = new QGroupBox("Входные сигналы постоянного тока (Bda_in_an)");
    vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1->Bda_in_anW(this));
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    lyout->addStretch(1);
    cp3->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

int TuneDialogA1::Start6_2()
{
//    WaitNSeconds(10);
    if (ReadAnalogMeasurements() == GENERALERROR)
        return GENERALERROR;
    return CheckBdaValues();
}

int TuneDialogA1::Start6_3_1()
{
    if (MessageBox2::question(this, "Вопрос", "Будет проведена регулировка по напряжениям, выполнить?") == false)
    {
        Skipped = true;
        return ER_RESEMPTY;
    }
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBac, BT_BASE, &Bac_block, sizeof(Bac));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bac");
        return GENERALERROR;
    }
    FillBac();
    return NOERROR;
}

int TuneDialogA1::Start6_3_2_1()
{
    if (Skipped)
        return ER_RESEMPTY;
    // получение текущих аналоговых сигналов от модуля
    CheckA1::A1_Bd1 tmpst, tmpst2;
    tmpst2.Frequency = tmpst2.Phy = tmpst2.UefNat_filt[0] = tmpst2.UefNat_filt[1] = \
            tmpst2.Uef_filt[0] = tmpst2.Uef_filt[1] = tmpst2.dU = tmpst2.dUrms = 0;
    for (int i=0; i<TD_MEASNUM; ++i)
    {
        cn->Send(CN_GBd, 1, &tmpst, sizeof(CheckA1::A1_Bd1));
        tmpst2.Frequency += tmpst.Frequency;
        tmpst2.Phy += tmpst.Phy;
        tmpst2.UefNat_filt[0] += tmpst.UefNat_filt[0];
        tmpst2.UefNat_filt[1] += tmpst.UefNat_filt[1];
        tmpst2.Uef_filt[0] += tmpst.Uef_filt[0];
        tmpst2.Uef_filt[1] += tmpst.Uef_filt[1];
        tmpst2.dU += tmpst.dU;
        tmpst2.dUrms += tmpst.dUrms;
    }
    // усреднение
    tmpst2.Frequency /= TD_MEASNUM;
    tmpst2.Phy /= TD_MEASNUM;
    tmpst2.UefNat_filt[0] /= TD_MEASNUM;
    tmpst2.UefNat_filt[1] /= TD_MEASNUM;
    tmpst2.Uef_filt[0] /= TD_MEASNUM;
    tmpst2.Uef_filt[1] /= TD_MEASNUM;
    tmpst2.dU /= TD_MEASNUM;
    tmpst2.dUrms /= TD_MEASNUM;
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
        return GENERALERROR;
    }
    memcpy(&ChA1->Bda_in, &tmpst2, sizeof(CheckA1::A1_Bd1));
    ChA1->FillBda_in(this);
    return CheckAnalogValues(false);
}

int TuneDialogA1::Start6_3_2_2()
{
    if (Skipped)
        return ER_RESEMPTY;
    return GetExternalData();
}

int TuneDialogA1::Start6_3_2_3()
{
    if (Skipped)
        return ER_RESEMPTY;
    Bac_block.KmU[0] = Bac_block.KmU[0] * RealData.u1 / ChA1->Bda_in.UefNat_filt[0];
    Bac_block.KmU[1] = Bac_block.KmU[1] * RealData.u2 / ChA1->Bda_in.UefNat_filt[1];
    Bac_block.K_freq = Bac_block.K_freq * RealData.freq / ChA1->Bda_in.Frequency;
    Bac_block.DPhy = Bac_block.DPhy - ChA1->Bda_in.Phy;
    return NOERROR;
}

int TuneDialogA1::Start6_3_3_1()
{
    Skipped = false;
    if (MessageBox2::question(this, "Вопрос", "Будет проведена регулировка по входу Pt100, выполнить?") == false)
    {
        Skipped = true;
        return ER_RESEMPTY;
    }
    MessageBox2::information(this, "Требование", "Установите на магазине сопротивлений значение 80 Ом");
    return NOERROR;
}

int TuneDialogA1::Start6_3_3_2()
{
    if (Skipped)
        return ER_RESEMPTY;
    WaitNSeconds(10);
    int res = ReadAnalogMeasurements();
    if (res == NOERROR)
        RegData = ChA1->Bda_block.Pt100;
    return res;
}

int TuneDialogA1::Start6_3_3_3()
{
    if (Skipped)
        return ER_RESEMPTY;
    MessageBox2::information(this, "Требование", "Установите на магазине сопротивлений значение 120 Ом");
    return NOERROR;
}

int TuneDialogA1::Start6_3_3_4()
{
    if (Skipped)
        return ER_RESEMPTY;
    WaitNSeconds(10);
    int res = ReadAnalogMeasurements();
    if (res == NOERROR)
    {
        Bac_block.Art = (ChA1->Bda_block.Pt100 - RegData) / 40;
        Bac_block.Brt = 2*ChA1->Bda_block.Pt100 - 3*RegData;
    }
    return res;
}

int TuneDialogA1::Start6_3_4()
{
    Skipped = false;
    if (MessageBox2::question(this, "Вопрос", "Будет проведена регулировка по входу EXTmA2, выполнить?") == false)
    {
        Skipped = true;
        return ER_RESEMPTY;
    }
    MessageBox2::information(this, "Требование", "Задайте ток 20,000мА в канале EXTmA2");
    return NOERROR;
}

int TuneDialogA1::Start6_3_5_1()
{
    if (Skipped)
        return ER_RESEMPTY;
    WaitNSeconds(10);
    int res = ReadAnalogMeasurements();
    if (res == NOERROR)
        RegData = ChA1->Bda_block.EXTmA2;
    return res;
}

int TuneDialogA1::Start6_3_5_2()
{
    if (Skipped)
        return ER_RESEMPTY;
    MessageBox2::information(this, "Требование", "Задайте ток 4,000мА в канале EXTmA2");
    return NOERROR;
}

int TuneDialogA1::Start6_3_5_3()
{
    if (Skipped)
        return ER_RESEMPTY;
    WaitNSeconds(10);
    int res = ReadAnalogMeasurements();
    if (res == NOERROR)
    {
        Bac_block.Ama2 = (RegData - ChA1->Bda_block.EXTmA2) / 16;
        Bac_block.Bma2 = (4*RegData - 20*ChA1->Bda_block.EXTmA2) / 16;
    }
    return res;
}

int TuneDialogA1::Start6_3_6()
{
    Skipped = false;
    if (MessageBox2::question(this, "Вопрос", "Будет проведена регулировка по входу EXTmA1, выполнить?") == false)
    {
        Skipped = true;
        return ER_RESEMPTY;
    }
    MessageBox2::information(this, "Требование", "Задайте ток 4,000мА в канале EXTmA1");
    return NOERROR;
}

int TuneDialogA1::Start6_3_7_1()
{
    if (Skipped)
        return ER_RESEMPTY;
    WaitNSeconds(10);
    int res = ReadAnalogMeasurements();
    if (res == NOERROR)
        RegData = ChA1->Bda_block.EXTmA1;
    return res;
}

int TuneDialogA1::Start6_3_7_2()
{
    if (Skipped)
        return ER_RESEMPTY;
    MessageBox2::information(this, "Требование", "Задайте ток 20,000мА в канале EXTmA1");
    return NOERROR;
}

int TuneDialogA1::Start6_3_7_3()
{
    if (Skipped)
        return ER_RESEMPTY;
    WaitNSeconds(10);
    int res = ReadAnalogMeasurements();
    if (res == NOERROR)
    {
        Bac_block.Ama1 = (ChA1->Bda_block.EXTmA1 - RegData) / 16;
        Bac_block.Bma1 = (4*ChA1->Bda_block.EXTmA1 - 20*RegData) / 16;
    }
    return res;
}

int TuneDialogA1::Start6_3_8()
{
    if (MessageBox2::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?") == false)
        return GENERALERROR;
    SaveToFileEx();
    WriteTuneCoefs();
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start6_3_9()
{
    QEventLoop Loop;
    WaitNSeconds(10);
    QString tmps = "Пожалуйста, просмотрите текущие данные после регулировки в соответствующих окнах";
    if (TuneFileSaved)
        tmps += "\nЕсли в процессе регулировки произошла ошибка, сохранённые коэффициенты\n"
                "Вы можете загрузить из файла "+pc.SystemHomeDir+"temptune.tn1";
    tmps += "\nПосле окончания проверки нажмите Esc для завершения процедуры регулировки";
    MessageBox2::information(this, "Завершение регулировки", tmps);
    connect(this,SIGNAL(Finished()),&Loop,SLOT(quit()));
    Loop.exec();
    return NOERROR;
}

int TuneDialogA1::ShowScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap(":/pic/tunea1.png")));
    lyout->addWidget(WDFunc::NewLBL(this, "1. На выходах РЕТОМ задайте частоту 51,0 Гц, уровень напряжения фазы А 60 В с фазой 0 градусов, включите режим однофазного выхода;"));
    lyout->addWidget(WDFunc::NewLBL(this, "2. Включите питание прибора Энергомонитор-3.1КМ и настройте его на режим измерения напряжений в диапазоне 0...100 В и частоты;"));
    lyout->addWidget(WDFunc::NewLBL(this, "3. На магазине сопротивлений установите значение сопротивления 100,0 Ом;"));
    lyout->addWidget(WDFunc::NewLBL(this, "4. Подключите калибратор токовой петли на вход 2 и установите выходной ток 20 мА;"));
    lyout->addWidget(WDFunc::NewLBL(this, "5. Включите выходы РЕТОМ."));
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    if (Cancelled == true)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::CheckBdaValues(int checktype)
{
    if (checktype | CHECK_VOLT)
    {
        if (!IsWithinLimits(ChA1->Bda_block.Ueff_ADC[0], 3900000.0, 400000.0))
            return GENERALERROR;
        if (!IsWithinLimits(ChA1->Bda_block.Ueff_ADC[1], 3900000.0, 400000.0))
            return GENERALERROR;
        if (!IsWithinLimits(ChA1->Bda_block.Frequency, 51.0, 0.05))
            return GENERALERROR;
    }
    if (checktype | CHECK_PT100)
    {
        if (!IsWithinLimits(ChA1->Bda_block.Pt100, 2125.0, 1000.0))
            return GENERALERROR;
    }
    if (checktype | CHECK_MA)
    {
        if (!IsWithinLimits(ChA1->Bda_block.EXTmA1, 25.0, 25.0))
            return GENERALERROR;
        if (!IsWithinLimits(ChA1->Bda_block.EXTmA2, 3275.0, 75.0))
            return GENERALERROR;
    }
    return NOERROR;
}

int TuneDialogA1::CheckAnalogValues(bool isPrecise)
{
    double Tols[6] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.15};
    double PrecTols[6] = {0.03, 0.03, 0.03, 0.03, 0.025, 0.05};
    double *T;
    T = (isPrecise) ? &PrecTols[0] : &Tols[0];
    if (!IsWithinLimits(ChA1->Bda_in.UefNat_filt[0], 60.0, T[0]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.UefNat_filt[1], 60.0, T[1]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Uef_filt[0], 60.0, T[2]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Uef_filt[0], 60.0, T[3]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Frequency, 51.0, T[4]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Phy, 0, T[5]))
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::GetExternalData()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("extdatad");
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(dlg, "Введите значения сигналов по приборам"),0,0,1,4);
    glyout->addWidget(WDFunc::NewLBL(dlg, "Значение напряжения 1, В"), 1,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "u1", -100, 100, 0.00001, 5), 1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(dlg, "Значение напряжения 2, В"), 1,2,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "u2", -100, 100, 0.00001, 5), 1,3,1,1);
    glyout->addWidget(WDFunc::NewLBL(dlg, "Угол между напряжениями, град"), 2,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "phy", -100, 100, 0.00001, 5), 2,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(dlg, "Частота, Гц"), 2,2,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "freq", -100, 100, 0.00001, 5), 2,3,1,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetExtData()));
    glyout->addWidget(pb,4,0,1,3);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelExtData()));
    glyout->addWidget(pb,4,3,1,3);
    dlg->setLayout(glyout);
    dlg->exec();
    if (Cancelled)
        return GENERALERROR;
    return NOERROR;
}

void TuneDialogA1::GetBdAndFillMTT()
{
    cn->Send(CN_GBd, A1_BDA_IN_BN, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1));
    if (cn->result == NOERROR)
        ChA1->FillBda_in(this);
    cn->Send(CN_GBd, A1_BDA_IN_AN_BN, &ChA1->Bda_in_an, sizeof(CheckA1::A1_Bd3));
    if (cn->result == NOERROR)
        ChA1->FillBda_in_an(this);

}

// ####################### SLOTS #############################

int TuneDialogA1::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    cn->Send(CN_GBda, BT_BASE, &ChA1->Bda_block, sizeof(CheckA1::Bda));
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bda");
        return GENERALERROR;
    }
    ChA1->FillBda(this);
    return NOERROR;
}

void TuneDialogA1::FillBac()
{
    WDFunc::SetLEData(this, "tune0", QString::number(Bac_block.KmU[0], 'f', 5));
    WDFunc::SetLEData(this, "tune1", QString::number(Bac_block.KmU[1], 'f', 5));
    WDFunc::SetLEData(this, "tune2", QString::number(Bac_block.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune3", QString::number(Bac_block.DPhy, 'f', 5));
    WDFunc::SetLEData(this, "tune25", QString::number(Bac_block.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune26", QString::number(Bac_block.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune27", QString::number(Bac_block.Ama1, 'f', 5));
    WDFunc::SetLEData(this, "tune28", QString::number(Bac_block.Bma1, 'f', 5));
    WDFunc::SetLEData(this, "tune29", QString::number(Bac_block.Ama2, 'f', 5));
    WDFunc::SetLEData(this, "tune30", QString::number(Bac_block.Bma2, 'f', 5));
}

void TuneDialogA1::FillBackBac()
{
    WDFunc::LENumber(this, "tune0", Bac_block.KmU[0]);
    WDFunc::LENumber(this, "tune1", Bac_block.KmU[1]);
    WDFunc::LENumber(this, "tune2", Bac_block.K_freq);
    WDFunc::LENumber(this, "tune3", Bac_block.DPhy);
    WDFunc::LENumber(this, "tune25", Bac_block.Art);
    WDFunc::LENumber(this, "tune26", Bac_block.Brt);
    WDFunc::LENumber(this, "tune27", Bac_block.Ama1);
    WDFunc::LENumber(this, "tune28", Bac_block.Bma1);
    WDFunc::LENumber(this, "tune29", Bac_block.Ama2);
    WDFunc::LENumber(this, "tune30", Bac_block.Bma2);
}

void TuneDialogA1::SetDefCoefs()
{
    Bac_block.KmU[0] = static_cast<float>(0.974);
    Bac_block.KmU[1] = static_cast<float>(0.98307);
    Bac_block.K_freq = 1;
    Bac_block.DPhy = static_cast<float>(0.00919);
    Bac_block.Art = static_cast<float>(82.0875);
    Bac_block.Brt = static_cast<float>(6023.3);
    Bac_block.Ama1 = static_cast<float>(163.839);
    Bac_block.Bma1 = static_cast<float>(-0.4125);
    Bac_block.Ama2 = static_cast<float>(163.6494);
    Bac_block.Bma2 = static_cast<float>(-0.8425);
    FillBac();
}

void TuneDialogA1::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("extdatad");
    if (dlg == 0)
        return;
    WDFunc::SPBData(dlg, "u1", RealData.u1);
    WDFunc::SPBData(dlg, "u2", RealData.u2);
    WDFunc::SPBData(dlg, "phy", RealData.phy);
    WDFunc::SPBData(dlg, "freq", RealData.freq);
    Cancelled = false;
    dlg->close();
}

void TuneDialogA1::CancelExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("extdatad");
    if (dlg == 0)
        return;
    Cancelled = true;
    dlg->close();
}

void TuneDialogA1::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        emit Finished();
        this->close();
    }
    QDialog::keyPressEvent(e);
}
