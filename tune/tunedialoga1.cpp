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
#include "tunedialoga1.h"
#include "../publicclass.h"
#include "../canal.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

TuneDialogA1::TuneDialogA1(QWidget *parent) :
    QDialog(parent)
{
    CA1 = new ConfigA1(S2Config);
    ChA1 = new CheckA1;
    setAttribute(Qt::WA_DeleteOnClose);
    int count = 0;
    MeasurementEnabled = false;
    MeasurementTimer = new QTimer;
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout()));
    pf[lbls().at(count++)] = &TuneDialogA1::CheckPassword; // 1. Ввод пароля
    pf[lbls().at(count++)] = &TuneDialogA1::ShowScheme; // 2. Отображение схемы подключения
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_2; // 6.2. Проверка правильности измерения сигналов переменного напряжения
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_1; // 6.3.1. Получение настроечных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_2_1; // 6.3.2.1. КПТ: получение блока данных и усреднение
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_2_2; // 6.3.2.2. КПТ: ввод данных от энергомонитора
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_2_3; // 6.3.2.3. КПТ: расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_1; // 6.3.3.1. КТС: подтверждение установки 80 Ом
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_2; // 6.3.3.2. КТС: получение блока данных
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_3; // 6.3.3.3. КТС: подтверждение установки 120 Ом
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_3_4; // 6.3.3.4. КТС: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_4; // 6.3.4. КМТ2: подтверждение установки 4 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_5_1; // 6.3.5.1. КМТ2: получение блока данных
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_5_2; // 6.3.5.2. КМТ2: подтверждение установки 20 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_5_3; // 6.3.5.3. КМТ2: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_6; // 6.3.6. КМТ1: подтверждение установки 4 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_7_1; // 6.3.7.1. КМТ1: получение блока данных
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_7_2; // 6.3.7.2. КМТ1: подтверждение установки 20 мА
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_7_3; // 6.3.7.3. КМТ1: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_8; // 6.3.8. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls().at(count++)] = &TuneDialogA1::Start6_3_9; // 6.3.9. Проверка аналоговых данных
    pf[lblsDN().at(count++)] = &TuneDialogA1::CheckPassword; // 7.2.2. Ввод пароля
    pf[lblsDN().at(count++)] = &TuneDialogA1::ShowSchemeDN; // 7.2.1. Отображение диалога проверки схемы подключения
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_3_1; // 7.2.3. Установка 20%, проверка и сохранение
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_3_2; // 7.2.5. Установка 40%, проверка и сохранение
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_3_3; // 7.2.5. Установка 60%, проверка и сохранение
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_3_4; // 7.2.5. Установка 80%, проверка и сохранение
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_3_5; // 7.2.5. Установка 100%, проверка и сохранение
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_3_6; // 7.2.5. Установка 120%, проверка и сохранение
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_67; // 7.2.6. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lblsDN().at(count++)] = &TuneDialogA1::Start7_2_8; // 7.2.8. Проверка аналоговых данных
    SetupUI();
}

void TuneDialogA1::SetupUI()
{
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QWidget *cp4 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *TuneTW = new QTabWidget;
    int i;

    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(UCONFWCLR)+"; font: bold 10px;}";
    tmps = "QWidget {background-color: "+QString(UCONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
    QLabel *lbl;

    TuneTW->addTab(cp1,"Настройка прибора");
    TuneTW->addTab(cp2,"Настройка штатного ДН");
    TuneTW->addTab(cp3,"Коэффициенты");
    TuneTW->addTab(cp4,"Данные измерений");

    // CP1 - НАСТРОЙКА ПРИБОРА

    lyout = new QVBoxLayout;
    QPushButton *pb = new QPushButton("Начать настройку прибора");
    pb->setObjectName("starttune");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTune()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    for (int i = 0; i < lbls().size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        lbl=new QLabel(lbls().at(i));
        lbl->setVisible(false);
        lbl->setObjectName("tunemsg"+QString::number(i));
        hlyout->addWidget(lbl);
        lbl=new QLabel("");
        lbl->setVisible(false);
        lbl->setObjectName("tunemsgres"+QString::number(i));
        hlyout->addWidget(lbl);
        hlyout->addStretch(1);
        lyout->addLayout(hlyout);
    }
    lyout->addStretch(1);
    cp1->setLayout(lyout);

    // CP2 - НАСТРОЙКА ДН

    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    lyout = new QVBoxLayout;
    pb = new QPushButton("Начать настройку делителя");
    pb->setObjectName("starttuneDN");
    connect(pb,SIGNAL(clicked()),this,SLOT(StartTuneDN()));
    if (pc.Emul)
        pb->setEnabled(false);
    lyout->addWidget(pb);
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Результаты измерений");
    glyout->addWidget(WDFunc::NewLBL(this, "U1"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", ValuesFormat, ""), 0, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "U2"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", ValuesFormat, ""), 0, 3, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "ΔU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", ValuesFormat, ""), 0, 5, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", ValuesFormat, ""), 1, 5, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreq", ValuesFormat, ""), 1, 1, 1, 2);
    pb = new QPushButton("Годится");
    pb->setObjectName("GoodDN");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Good()));
    glyout->addWidget(pb, 1, 4, 1, 1);
    pb = new QPushButton("Отмена");
    pb->setObjectName("NoGoodDN");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(NoGood()));
    glyout->addWidget(pb, 1, 5, 1, 1);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    for (int i = 0; i < lblsDN().size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        lbl=new QLabel(lblsDN().at(i));
        lbl->setVisible(false);
        lbl->setObjectName("tunemsg"+QString::number(LBLSDN+i));
        hlyout->addWidget(lbl);
        lbl=new QLabel("");
        lbl->setVisible(false);
        lbl->setObjectName("tunemsgres"+QString::number(LBLSDN+i));
        hlyout->addWidget(lbl);
        hlyout->addStretch(1);
        lyout->addLayout(hlyout);
    }
    cp2->setLayout(lyout);

    // CP3 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    lyout = new QVBoxLayout;
    glyout = new QGridLayout;
    gb = new QGroupBox("Настроечные коэффициенты");
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[0]"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune0", "", ValuesLEFormat), 0, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[1]"), 0, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune1", "", ValuesLEFormat), 0, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "K_freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune2", "", ValuesLEFormat), 1, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "DPhy"), 1, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune3", "", ValuesLEFormat), 1, 4, 1, 2);
    for (i = 0; i < 6; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "U1kDN["+QString::number(i)+"]"),2,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+4), "", ValuesLEFormat),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "U2kDN["+QString::number(i)+"]"),4,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+10), "", ValuesLEFormat),5,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PhyDN["+QString::number(i)+"]"),6,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+16), "", ValuesLEFormat),7,i,1,1);
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Art"), 8, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune22", "", ValuesLEFormat), 8, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Brt"), 8, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune23", "", ValuesLEFormat), 8, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama1"), 9, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune24", "", ValuesLEFormat), 9, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma1"), 9, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune25", "", ValuesLEFormat), 9, 4, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama2"), 10, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune26", "", ValuesLEFormat), 10, 1, 1, 2);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma2"), 10, 3, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune27", "", ValuesLEFormat), 10, 4, 1, 2);

    pb = new QPushButton("Установить настроечные коэффициенты по умолчанию");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetDefCoefs()));
    glyout->addWidget(pb, 11, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из модуля");
    connect(pb,SIGNAL(clicked()),this,SLOT(ReadTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    glyout->addWidget(pb, 12, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в модуль");
    connect(pb,SIGNAL(clicked()),this,SLOT(WriteTuneCoefs()));
    if (pc.Emul)
        pb->setEnabled(false);
    glyout->addWidget(pb, 13, 0, 1, 6);
    pb = new QPushButton("Прочитать настроечные коэффициенты из файла");
    pb->setIcon(QIcon(":/load.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadFromFile()));
    glyout->addWidget(pb, 14, 0, 1, 6);
    pb = new QPushButton("Записать настроечные коэффициенты в файл");
    pb->setIcon(QIcon(":/save.png"));
    connect(pb,SIGNAL(clicked()),this,SLOT(SaveToFile()));
    glyout->addWidget(pb, 15, 0, 1, 6);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addStretch(1);
    cp3->setLayout(lyout);

    // CP4 - Данные измерений

    lyout = new QVBoxLayout;
    gb = new QGroupBox("Данные измерений без настройки (Bda)");
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1.BdaW(this));
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    gb = new QGroupBox("Напряжения в масштабе входных сигналов (Bda_in)");
    vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1.Bda_inW(this));
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
    gb = new QGroupBox("Входные сигналы постоянного тока (Bda_in_an)");
    vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1.Bda_in_anW(this));
    gb->setLayout(vlyout);
    lyout->addWidget(gb);

    lyout->addStretch(1);
    cp4->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

int TuneDialogA1::Start6_2()
{
    WaitNSeconds(10);
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
    tmpst2.Frequency = tmpst2.Phy = tmpst2.UefNat_filt[0] = tmpst2.UefNat_filt[1] = tmpst2.Uef_filt[0] = tmpst2.Uef_filt[1] = 0;
    for (int i=0; i<TDA1_MEASNUM; ++i)
    {
        cn->Send(CN_GBd, 1, &tmpst, sizeof(CheckA1::A1_Bd1));
        tmpst2.Frequency += tmpst.Frequency;
        tmpst2.Phy += tmpst.Phy;
        tmpst2.UefNat_filt[0] += tmpst.UefNat_filt[0];
        tmpst2.UefNat_filt[1] += tmpst.UefNat_filt[1];
        tmpst2.Uef_filt[0] += tmpst.Uef_filt[0];
        tmpst2.Uef_filt[1] += tmpst.Uef_filt[1];
    }
    // усреднение
    tmpst2.Frequency /= TDA1_MEASNUM;
    tmpst2.Phy /= TDA1_MEASNUM;
    tmpst2.UefNat_filt[0] /= TDA1_MEASNUM;
    tmpst2.UefNat_filt[1] /= TDA1_MEASNUM;
    tmpst2.Uef_filt[0] /= TDA1_MEASNUM;
    tmpst2.Uef_filt[1] /= TDA1_MEASNUM;
    if (cn->result != NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
        return GENERALERROR;
    }
    memcpy(&ChA1.Bda_in, &tmpst2, sizeof(CheckA1::A1_Bd1));
    ChA1.FillBda_in(this);
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
    Bac_block.KmU[0] = Bac_block.KmU[0] * RealData.u1 / ChA1.Bda_in.UefNat_filt[0];
    Bac_block.KmU[1] = Bac_block.KmU[1] * RealData.u2 / ChA1.Bda_in.UefNat_filt[1];
    Bac_block.K_freq = Bac_block.K_freq * RealData.freq / ChA1.Bda_in.Frequency;
    Bac_block.DPhy = Bac_block.DPhy - ChA1.Bda_in.Phy;
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
        RegData = ChA1.Bda_block.Pt100;
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
        Bac_block.Art = (ChA1.Bda_block.Pt100 - RegData) / 40;
        Bac_block.Brt = 2*ChA1.Bda_block.Pt100 - 3*RegData;
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
        RegData = ChA1.Bda_block.EXTmA2;
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
        Bac_block.Ama2 = (RegData - ChA1.Bda_block.EXTmA2) / 16;
        Bac_block.Bma2 = (4*RegData - 20*ChA1.Bda_block.EXTmA2) / 16;
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
        RegData = ChA1.Bda_block.EXTmA1;
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
        Bac_block.Ama1 = (ChA1.Bda_block.EXTmA1 - RegData) / 16;
        Bac_block.Bma1 = (4*ChA1.Bda_block.EXTmA1 - 20*RegData) / 16;
    }
    return res;
}

int TuneDialogA1::Start6_3_8()
{
    if (MessageBox2::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?") == false)
        return GENERALERROR;
    SaveToFileEx();
    cn->Send(CN_WBac, BT_BASE, &Bac_block, sizeof(Bac));
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start6_3_9()
{
    WaitNSeconds(10);
    if (ReadAnalogMeasurements() != NOERROR)
        return GENERALERROR;
    return CheckAnalogValues(true);
}

int TuneDialogA1::Start6_3_10()
{
    cn->Send(CN_GBd, 5, &ChA1.Bda_out_an, sizeof(CheckA1::A1_Bd4));
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::Start7_2_2()
{
    cn->Send(CN_GF, BT_NONE,NULL,0,1,S2Config);
    if (cn->result == NOERROR)
        return NOERROR;
    return GENERALERROR;
}

int TuneDialogA1::Start7_2_3_1()
{
    Start7_2_345(0);
}

int TuneDialogA1::Start7_2_3_2()
{
    Start7_2_345(1);
}

int TuneDialogA1::Start7_2_3_3()
{
    Start7_2_345(2);
}

int TuneDialogA1::Start7_2_3_4()
{
    Start7_2_345(3);
}

int TuneDialogA1::Start7_2_3_5()
{
    Start7_2_345(4);
}

int TuneDialogA1::Start7_2_3_6()
{
    Start7_2_345(5);
}

int TuneDialogA1::Start7_2_345(int counter)
{
    const int Percents = {20,40,60,80,100,120};
    if (counter > 5)
        return GENERALERROR;
    int VoltageInkV = CA1->Bci_block.K_DN * Percents[counter] / 1732;
    if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + QString::number(VoltageInkV) + " кВ") == false)
    {
        Cancelled = true;
        return GENERALERROR;
    }
    MeasurementEnabled = true;
    while (MeasurementEnabled && !Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    if (Cancelled)
        return GENERALERROR;
    // теперь в ChA1->Bda_block лежат нужные нам значения
    Bac_block2.U1kDN[counter] = ChA1->Bda_out.Uef_filt[0];
    Bac_block2.U2kDN[counter] = ChA1->Bda_out.Uef_filt[1];
    Bac_block2.PhyDN[counter] = ChA1->Bda_out.Phy;
}

int TuneDialogA1::Start7_2_67()
{
    QDialog *dlg
}

int TuneDialogA1::Start7_2_8()
{

}

void TuneDialogA1::ProcessTune(int count, QStringList &sl)
{
    Cancelled = Skipped = false;
    MsgClear(); // очистка экрана с сообщениями
    for (int i=0; i<sl.size(); ++i)
    {
        MsgSetVisible(count);
        int res = (this->*pf[sl.at(i)])();
        if ((res == GENERALERROR) || (Cancelled))
        {
            ErMsgSetVisible(count);
            WDFunc::SetEnabled(this, "starttune", true);
            WARNMSG(sl.at(i));
            return;
        }
        else if (res == ER_RESEMPTY)
            SkMsgSetVisible(count);
        else
            OkMsgSetVisible(count);
        ++count;
    }
}

int TuneDialogA1::CheckPassword()
{
    QEventLoop PasswordLoop;
    KeyPressDialog *dlg = new KeyPressDialog;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc"));
    connect(dlg,SIGNAL(Cancelled()),this,SLOT(CancelTune()));
    connect(dlg,SIGNAL(Finished(QString)),this,SLOT(PasswordCheck(QString)));
    connect(this,SIGNAL(PasswordChecked()),&PasswordLoop,SLOT(quit()));
    dlg->setLayout(vlyout);
    dlg->show();
    PasswordLoop.exec();
    if (!ok)
    {
        MessageBox2::error(this, "Неправильно", "Пароль введён неверно");
        return GENERALERROR;
    }
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
    lyout->addWidget(WDFunc::NewLBL(this, "4. Включите калибратор токовой петли и установите выходной ток 20 мА;"));
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

int TuneDialogA1::CheckBdaValues()
{
    if (!IsWithinLimits(ChA1.Bda_block.Ueff_ADC[0], 3900000.0, 400000.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.Ueff_ADC[1], 3900000.0, 400000.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.Frequency, 51.0, 0.05))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.Pt100, 2125.0, 75.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.EXTmA1, 25.0, 25.0))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_block.EXTmA2, 3275.0, 75.0))
        return GENERALERROR;
    return NOERROR;
}

int TuneDialogA1::CheckAnalogValues(bool isPrecise)
{
    double Tols[6] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.15};
    double PrecTols[6] = {0.03, 0.03, 0.03, 0.03, 0.025, 0.05};
    double *T;
    T = (isPrecise) ? &PrecTols[0] : &Tols[0];
    if (!IsWithinLimits(ChA1.Bda_in.UefNat_filt[0], 60.0, T[0]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_in.UefNat_filt[1], 60.0, T[1]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_in.Uef_filt[0], 60.0, T[2]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_in.Uef_filt[0], 60.0, T[3]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_in.Frequency, 51.0, T[4]))
        return GENERALERROR;
    if (!IsWithinLimits(ChA1.Bda_in.Phy, 0, T[5]))
        return GENERALERROR;
    return NOERROR;
}

bool TuneDialogA1::IsWithinLimits(double number, double base, double threshold)
{
    float tmpf = fabs(number-base);
    if (tmpf<fabs(threshold))
        return true;
    else
    {
        MessageBox2::error(this, "Ошибка", "Ошибочное значение: должно быть "+QString::number(base, 'f', 5) + \
                           ", а получили: "+QString::number(number, 'f', 5));
        return false;
    }
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

void TuneDialogA1::MsgSetVisible(int msg, bool Visible)
{
    WDFunc::SetVisible(this, "tunemsg"+QString::number(msg), Visible);
}

void TuneDialogA1::OkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/ok.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialogA1::ErMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/cross.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialogA1::SkMsgSetVisible(int msg, bool Visible)
{
    QPixmap *pm = new QPixmap(":/pic/hr.png");
    WDFunc::SetVisible(this, "tunemsgres"+QString::number(msg), Visible);
    WDFunc::SetLBLImage(this, "tunemsgres"+QString::number(msg), pm);
}

void TuneDialogA1::MsgClear()
{
    for (int i=0; i<lbls().size(); ++i)
    {
        MsgSetVisible(i, false);
        OkMsgSetVisible(i, false);
    }
}

void TuneDialogA1::WaitNSeconds(int Seconds)
{
    SecondsToEnd15SecondsInterval = Seconds;
    WaitWidget *w = new WaitWidget;
    QTimer *tmr = new QTimer;
    tmr->setInterval(1000);
    connect(tmr,SIGNAL(timeout()),this,SLOT(UpdateNSecondsWidget()));
    connect(this,SIGNAL(SecondsRemaining(QString)),w,SLOT(SetMessage(QString)));
    tmr->start();
    w->Start();
    while (SecondsToEnd15SecondsInterval > 0)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < 20)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    tmr->stop();
    w->Stop();
}

void TuneDialogA1::SaveToFileEx()
{
    int res = pc.SaveFile("Tune files (*.tn)", &Bac_block, sizeof(Bac_block));
    switch (res)
    {
    case NOERROR:
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        break;
    case ER_FILEWRITE:
        MessageBox2::error(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case ER_FILENAMEEMP:
        MessageBox2::error(this, "Ошибка", "Пустое имя файла!");
        break;
    case ER_FILEOPEN:
        MessageBox2::error(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

int TuneDialogA1::ShowSchemeDN()
{
    if (MessageBox2::question(this, "Подтверждение", "Подключите выход своего делителя напряжения ко входу U1 прибора\n"
                              "Выход эталонного делителя - ко входу U2") == false)
    {
        Cancelled = true;
        return GENERALERROR;
    }
    return NOERROR;
}

// ####################### SLOTS #############################

void TuneDialogA1::StartTune()
{
    WDFunc::SetEnabled(this, "starttune", false);
    ProcessTune(0, lbls());
}

void TuneDialogA1::StartTuneDN()
{
    WDFunc::SetEnabled(this, "starttuneDN", false);
    ProcessTune(LBLSDN, lblsDN());
}

void TuneDialogA1::PasswordCheck(QString psw)
{
    if (psw == "121941")
        ok = true;
    else
        ok = false;
    emit PasswordChecked();
}

void TuneDialogA1::ReadTuneCoefs()
{
    cn->Send(CN_GBac, BT_BASE, &Bac_block, sizeof(Bac));
    if (cn->result == NOERROR)
        FillBac();
}

void TuneDialogA1::WriteTuneCoefs()
{
    FillBackBac();
    cn->Send(CN_WBac, BT_BASE, &Bac_block, sizeof(Bac));
    if (cn->result == NOERROR)
        MessageBox2::information(this, "Внимание", "Записано успешно!");
}

void TuneDialogA1::SaveToFile()
{
    FillBackBac();
    SaveToFileEx();
}

void TuneDialogA1::LoadFromFile()
{
    QByteArray ba = pc.LoadFile("Tune files (*.tn)");
    memcpy(&Bac_block,&(ba.data()[0]),sizeof(Bac_block));
    FillBac();
    MessageBox2::information(this, "Внимание", "Загрузка прошла успешно!");
}

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

void TuneDialogA1::Good()
{
    FillBackBdOut();
    MeasurementEnabled = false;
}

void TuneDialogA1::NoGood()
{
    Cancelled = true;
    MeasurementEnabled = false;
}

void TuneDialogA1::FillBac()
{
    WDFunc::SetLEData(this, "tune0", QString::number(Bac_block.KmU[0], 'f', 5));
    WDFunc::SetLEData(this, "tune1", QString::number(Bac_block.KmU[1], 'f', 5));
    WDFunc::SetLEData(this, "tune2", QString::number(Bac_block.K_freq, 'f', 5));
    WDFunc::SetLEData(this, "tune3", QString::number(Bac_block.DPhy, 'f', 5));
    WDFunc::SetLEData(this, "tune22", QString::number(Bac_block.Art, 'f', 5));
    WDFunc::SetLEData(this, "tune23", QString::number(Bac_block.Brt, 'f', 5));
    WDFunc::SetLEData(this, "tune24", QString::number(Bac_block.Ama1, 'f', 5));
    WDFunc::SetLEData(this, "tune25", QString::number(Bac_block.Bma1, 'f', 5));
    WDFunc::SetLEData(this, "tune26", QString::number(Bac_block.Ama2, 'f', 5));
    WDFunc::SetLEData(this, "tune27", QString::number(Bac_block.Bma2, 'f', 5));
}

void TuneDialogA1::FillBac2()
{
    for (int i = 0; i < 6; ++i)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+4), QString::number(Bac_block2.U1kDN[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+10), QString::number(Bac_block2.U2kDN[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+16), QString::number(Bac_block2.PhyDN[i], 'f', 5));
    }
}

void TuneDialogA1::FillBackBac()
{
    WDFunc::LENumber(this, "tune0", Bac_block.KmU[0]);
    WDFunc::LENumber(this, "tune1", Bac_block.KmU[1]);
    WDFunc::LENumber(this, "tune2", Bac_block.K_freq);
    WDFunc::LENumber(this, "tune3", Bac_block.DPhy);
    WDFunc::LENumber(this, "tune22", Bac_block.Art);
    WDFunc::LENumber(this, "tune23", Bac_block.Brt);
    WDFunc::LENumber(this, "tune24", Bac_block.Ama1);
    WDFunc::LENumber(this, "tune25", Bac_block.Bma1);
    WDFunc::LENumber(this, "tune26", Bac_block.Ama2);
    WDFunc::LENumber(this, "tune27", Bac_block.Bma2);
}

void TuneDialogA1::FillBackBac2()
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::LENumber(this, "tune"+QString::number(i+4), Bac_block2.U1kDN[i]);
        WDFunc::LENumber(this, "tune"+QString::number(i+10), Bac_block2.U2kDN[i]);
        WDFunc::LENumber(this, "tune"+QString::number(i+16), Bac_block2.PhyDN[i]);
    }
}

void TuneDialogA1::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.Uef_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphy", QString::number(ChA1->Bda_out.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreq", QString::number(ChA1->Bda_out.Frequency, 'f', 5));
    float Percents = qAbs(ChA1->Bda_out.Uef_filt[0]-ChA1->Bda_out.Uef_filt[1]) / CA1->Bci_block.K_DN / 1.732f;
    WDFunc::SetLBLText(this, "tunepercent", QString::number(Percents, 'f', 5));
}

void TuneDialogA1::FillBackBdOut()
{
    QString tmps;
    WDFunc::LBLText(this, "tunednu1", tmps);
    ChA1->Bda_out.Uef_filt[0] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednu2", tmps);
    ChA1->Bda_out.Uef_filt[1] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednphy", tmps);
    ChA1->Bda_out.Phy = tmps.toFloat();
    WDFunc::LBLText(this, "tunednfreq", tmps);
    ChA1->Bda_out.Frequency = tmps.toFloat();
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

void TuneDialogA1::SetDefCoefs2()
{
    Bac_block2.U1kDN[0] = Bac_block2.U2kDN[0] = 12;
    Bac_block2.U1kDN[1] = Bac_block2.U2kDN[1] = 24;
    Bac_block2.U1kDN[2] = Bac_block2.U2kDN[2] = 36;
    Bac_block2.U1kDN[3] = Bac_block2.U2kDN[3] = 48;
    Bac_block2.U1kDN[4] = Bac_block2.U2kDN[4] = 60;
    Bac_block2.U1kDN[5] = Bac_block2.U2kDN[5] = 71;
    Bac_block2.PhyDN[0] = Bac_block2.PhyDN[1] = Bac_block2.PhyDN[2] = Bac_block2.PhyDN[3] = Bac_block2.PhyDN[4] = Bac_block2.PhyDN[5] = 0;
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

void TuneDialogA1::CancelTune()
{
    Cancelled = true;
}

void TuneDialogA1::UpdateNSecondsWidget()
{
    QString tmps = "Подождите " + QString::number(--SecondsToEnd15SecondsInterval) + " с";
    emit SecondsRemaining(tmps);
}

void TuneDialogA1::MeasTimerTimeout()
{
    if (MeasurementEnabled)
    {
        WDFunc::SetEnabled(this, "GoodDN", true);
        WDFunc::SetEnabled(this, "NoGoodDN", true);
        cn->Send(CN_GBd, 4, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
        FillBdOut();
    }
    else
    {
        WDFunc::SetEnabled(this, "GoodDN", false);
        WDFunc::SetEnabled(this, "NoGoodDN", false);
    }
}

// ##################### PROTECTED ####################

void TuneDialogA1::closeEvent(QCloseEvent *e)
{
    emit stopall();
    e->accept();
}
