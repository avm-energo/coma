#include <QTime>
#include <QtMath>
#include <QSettings>
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
#include "../gen/maindef.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../dialogs/keypressdialog.h"
#include "../widgets/emessagebox.h"
#include "../gen/stdfunc.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"

TuneDialogA1::TuneDialogA1(QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    CA1 = new ConfigA1(S2Config);
    ChA1 = new CheckA1;
    SetBac(&Bac_block, 1, sizeof(Bac_block));
    SetupUI();
    LoadSettings();
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialogA1::SetLbls()
{
    lbls.append("1 Ввод пароля...");
    lbls.append("2 Отображение схемы подключения...");
    lbls.append("6.3.2 Получение настроечных коэффициентов...");
    lbls.append("6.3.3.1 КПТ: выдача предупреждения...");
    lbls.append("6.3.3.2 КПТ: опрос постоянных напряжений...");
    lbls.append("6.3.3.3 КПТ: вычисление смещения...");
    lbls.append("6.3.3.4 КПТ: выдача предупреждения...");
    lbls.append("6.3.3.5 КПТ: получение блока данных и усреднение...");
    lbls.append("6.3.3.6 КПТ: ввод данных от энергомонитора...");
    lbls.append("6.3.3.7 КПТ: расчёт регулировочных коэффициентов...");
    lbls.append("6.3.4.1 КТС: подтверждение установки 80 Ом...");
    lbls.append("6.3.4.3 КТС: подтверждение установки 120 Ом...");
    lbls.append("6.3.4.4 КТС: получение блока данных и расчёт регулировочных коэффициентов...");
    lbls.append("6.3.5 КМТ2: подтверждение установки 4 мА...");
    lbls.append("6.3.6.1 КМТ2: получение блока данных...");
    lbls.append("6.3.6.2 КМТ2: подтверждение установки 20 мА...");
    lbls.append("6.3.6.3 КМТ2: получение блока данных и расчёт регулировочных коэффициентов...");
    lbls.append("6.3.7 КМТ1: подтверждение установки 4 мА...");
    lbls.append("6.3.8.1 КМТ1: получение блока данных...");
    lbls.append("6.3.8.2 КМТ1: подтверждение установки 20 мА...");
    lbls.append("6.3.8.3 КМТ1: получение блока данных и расчёт регулировочных коэффициентов...");
    lbls.append("6.3.9 Проверка данных блока Bda_out_an...");
    lbls.append("6.3.10.1 Настройка температурной коррекции по нормальной температуре...");
    lbls.append("6.3.10.2 Настройка температурной коррекции по повышенной температуре...");
    lbls.append("6.3.10.3 Настройка температурной коррекции по пониженной температуре...");
    lbls.append("6.3.11 Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("6.3.12 Проверка аналоговых данных...");
}

void TuneDialogA1::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // 1. Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::ShowScheme);
    pf[lbls.at(count++)] = func; // 2. Отображение схемы подключения
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_2); // 6.3.2. Получение настроечных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_1); // 6.3.3.1. КПТ: выдача предупреждения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_2); // 6.3.3.2. КПТ: опрос постоянных напряжений
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_3); // 6.3.3.3. КПТ: вычисление смещения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_4); // 6.3.3.4. КПТ: выдача предупреждения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_5); // 6.3.3.5. КПТ: получение блока данных и усреднение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_6); // 6.3.3.6. КПТ: ввод данных от энергомонитора
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_3_7); // 6.3.3.7. КПТ: расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_4_1); // 6.3.4.1. КТС: подтверждение установки 80 Ом
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_4_3); // 6.3.4.3. КТС: подтверждение установки 120 Ом
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_4_4); // 6.3.4.4. КТС: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_5); // 6.3.5. КМТ2: подтверждение установки 4 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_6_1); // 6.3.6.1. КМТ2: получение блока данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_6_2); // 6.3.6.2. КМТ2: подтверждение установки 20 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_6_3); // 6.3.6.3. КМТ2: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_7); // 6.3.7. КМТ1: подтверждение установки 4 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_8_1); // 6.3.8.1. КМТ1: получение блока данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_8_2); // 6.3.8.2. КМТ1: подтверждение установки 20 мА
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_8_3); // 6.3.8.3. КМТ1: получение блока данных и расчёт регулировочных коэффициентов
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_9); // 6.3.9. Проверка данных блока Bda_out_an
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_10_1); // 6.3.10.1 Настройка температурной коррекции по 20 градусам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_10_2); // 6.3.10.2 Настройка температурной коррекции по 50 градусам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_10_3); // 6.3.10.3 Настройка температурной коррекции по 0 градусам
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_11); // 6.3.11. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int (EAbstractTuneDialog::*)()>(&TuneDialogA1::Start6_3_12); // 6.3.12. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
}
#endif

void TuneDialogA1::SetupUI()
{
    QWidget *cp1 = TuneUI();
    QWidget *cp2 = new QWidget;
    QWidget *cp3 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *TuneTW = new QTabWidget;
    TuneTW->setObjectName("tunetw");

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
    glyout->addWidget(WDFunc::NewLE(this, "tune0", "", ValuesLEFormat), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "KmU[1]"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune1", "", ValuesLEFormat), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "K_freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune2", "", ValuesLEFormat), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "DPhy"), 1, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune3", "", ValuesLEFormat), 1, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Art"), 2, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune25", "", ValuesLEFormat), 2, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Brt"), 2, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune26", "", ValuesLEFormat), 2, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama1"), 3, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune27", "", ValuesLEFormat), 3, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma1"), 3, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune28", "", ValuesLEFormat), 3, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Ama2"), 4, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune29", "", ValuesLEFormat), 4, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Bma2"), 4, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune30", "", ValuesLEFormat), 4, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "TKUa[0]"), 5, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune31", "", ValuesLEFormat), 5, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "TKUa[1]"), 5, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune32", "", ValuesLEFormat), 5, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "TKUb[0]"), 6, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune33", "", ValuesLEFormat), 6, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "TKUb[1]"), 6, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune34", "", ValuesLEFormat), 6, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Tmk0"), 7, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune35", "", ValuesLEFormat), 7, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Offset0"), 8, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune36", "", ValuesLEFormat), 8, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Offset1"), 8, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "tune37", "", ValuesLEFormat), 8, 3, 1, 1);
    glyout->setColumnStretch(0, 0);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(2, 0);
    glyout->setColumnStretch(3, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);
    lyout->addWidget(BottomUI(1));
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
    lyout->addWidget(BdaBottomUI());

    lyout->addStretch(1);
    cp3->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
}

QWidget *TuneDialogA1::BdaBottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;

    QPushButton *pb = new QPushButton("Запустить чтение сигналов");
    pb->setObjectName("pbmeasurements");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(StartMeasurement()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение сигналов");
#if PROGSIZE != PROGSIZE_EMUL
    connect(pb,SIGNAL(clicked()),this,SLOT(Good()));
#endif
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    lyout->addWidget(pb);
    w->setLayout(lyout);
    return w;
}

#if PROGSIZE != PROGSIZE_EMUL
int TuneDialogA1::Start6_3_2()
{
    if (Commands::GetBac(BT_BASE, &Bac_block_old, sizeof(Bac)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bac");
        return Error::ER_GENERALERROR;
    }
    FillBac(1);
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_3_1()
{
    Skipped = false;
    if (EMessageBox::question(this, "Вопрос", "Будет проведена регулировка по напряжениям, выполнить?") == false)
    {
        Bac_block.KmU[0] = Bac_block_old.KmU[0];
        Bac_block.KmU[1] = Bac_block_old.KmU[1];
        Bac_block.K_freq = Bac_block_old.K_freq;
        Bac_block.DPhy = Bac_block_old.DPhy;
        Bac_block.ch_offset[0] = Bac_block_old.ch_offset[0];
        Bac_block.ch_offset[1] = Bac_block_old.ch_offset[1];
        Skipped = true;
        return Error::ER_RESEMPTY;
    }
    EMessageBox::information(this, "Требование", "отключите РЕТОМ и закоротите входы U1 и U2");
    WaitNSeconds(2);
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_3_2()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    CheckA1::Bda tmpbda;
    if (Start60PointsMeasurements(TDA1_BDA, &tmpbda) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    memcpy(&ChA1->Bda_block, &tmpbda, sizeof(CheckA1::Bda));
    ChA1->FillBda(this);
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_3_3()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    Bac_block.ch_offset[0] = Bac_block_old.ch_offset[0] + ChA1->Bda_block.Middle_ADC[0];
    Bac_block.ch_offset[1] = Bac_block_old.ch_offset[1] + ChA1->Bda_block.Middle_ADC[1];
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_3_4()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    EMessageBox::information(this, "Требование", "Снимите закоротку с РЕТОМ, подключите РЕТОМ ко входам U1 и U2 \n "
                             "и задайте на выходе напряжения фазы А напряжение 100,0 В при частоте 51 Гц");
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_3_5()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    if (Commands::SetMode(MODE_ALTERNATIVE) != Error::ER_NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка установки рода напряжения");
        return Error::ER_GENERALERROR;
    }
//    EMessageBox::information(this, "Требование", "Установите на РЕТОМ значение напряжения 100 В");
    // получение текущих аналоговых сигналов от модуля
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (ReadAnalogMeasurements() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    if (CheckBdaValues(CHECK_VOLT) == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    CheckA1::A1_Bd1 tmpbd1;
    if (Start60PointsMeasurements(TDA1_BD1, &tmpbd1) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    memcpy(&ChA1->Bda_in, &tmpbd1, sizeof(CheckA1::A1_Bd1));
    ChA1->FillBda_in(this);
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_3_6()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
/*    if (GetExternalData() != Error::NOERROR)
        return Error::GENERALERROR;
    return CheckAnalogValues(false); */
    return GetExternalData();
}

int TuneDialogA1::Start6_3_3_7()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    Bac_block.KmU[0] = Bac_block_old.KmU[0] * RealData.u / ChA1->Bda_in.UefNat_filt[0];
    Bac_block.KmU[1] = Bac_block_old.KmU[1] * RealData.u / ChA1->Bda_in.UefNat_filt[1];
    Bac_block.K_freq = Bac_block_old.K_freq * RealData.freq / ChA1->Bda_in.Frequency;
    Bac_block.DPhy = Bac_block_old.DPhy - ChA1->Bda_in.Phy;
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_4_1()
{
    Skipped = false;
    if (EMessageBox::question(this, "Вопрос", "Будет проведена регулировка по входу Pt100, выполнить?") == false)
    {
        Bac_block.Art = Bac_block_old.Art;
        Bac_block.Brt = Bac_block_old.Brt;
        Skipped = true;
        return Error::Error::ER_RESEMPTY;
    }
    EMessageBox::information(this, "Требование", "Установите на магазине сопротивлений значение 80 Ом");
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (ReadAnalogMeasurements() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    RegData = ChA1->Bda_block.Pt100;
//    return CheckBdaValues(CHECK_PT100);
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_4_3()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    EMessageBox::information(this, "Требование", "Установите на магазине сопротивлений значение 120 Ом");
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_4_4()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    int res = ReadAnalogMeasurements();
    if (res == Error::ER_NOERROR)
    {
        Bac_block.Art = (ChA1->Bda_block.Pt100 - RegData) / 40;
        Bac_block.Brt = 2*ChA1->Bda_block.Pt100 - 3*RegData;
    }
    return res;
}

int TuneDialogA1::Start6_3_5()
{
    Skipped = false;
    if (EMessageBox::question(this, "Вопрос", "Будет проведена регулировка по входу EXTmA2, выполнить?") == false)
    {
        Bac_block.Ama2 = Bac_block_old.Ama2;
        Bac_block.Bma2 = Bac_block_old.Bma2;
        Skipped = true;
        return Error::ER_RESEMPTY;
    }
    EMessageBox::information(this, "Требование", "Задайте ток 20,000мА в канале EXTmA2");
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (ReadAnalogMeasurements() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    return CheckBdaValues(CHECK_MA2);
}

int TuneDialogA1::Start6_3_6_1()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    int res = ReadAnalogMeasurements();
    if (res == Error::ER_NOERROR)
        RegData = ChA1->Bda_block.EXTmA2;
    return res;
}

int TuneDialogA1::Start6_3_6_2()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    EMessageBox::information(this, "Требование", "Задайте ток 4,000мА в канале EXTmA2");
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_6_3()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    int res = ReadAnalogMeasurements();
    if (res == Error::ER_NOERROR)
    {
        Bac_block.Ama2 = (RegData - ChA1->Bda_block.EXTmA2) / 16;
        Bac_block.Bma2 = (4*RegData - 20*ChA1->Bda_block.EXTmA2) / 16;
    }
    return res;
}

int TuneDialogA1::Start6_3_7()
{
    Skipped = false;
    if (EMessageBox::question(this, "Вопрос", "Будет проведена регулировка по входу EXTmA1, выполнить?") == false)
    {
        Bac_block.Ama1 = Bac_block_old.Ama1;
        Bac_block.Bma1 = Bac_block_old.Bma1;
        Skipped = true;
        return Error::ER_RESEMPTY;
    }
    EMessageBox::information(this, "Требование", "Задайте ток 4,000мА в канале EXTmA1");
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    if (ReadAnalogMeasurements() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    return CheckBdaValues(CHECK_MA1);
}

int TuneDialogA1::Start6_3_8_1()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    int res = ReadAnalogMeasurements();
    if (res == Error::ER_NOERROR)
        RegData = ChA1->Bda_block.EXTmA1;
    return res;
}

int TuneDialogA1::Start6_3_8_2()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    EMessageBox::information(this, "Требование", "Задайте ток 20,000мА в канале EXTmA1");
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_8_3()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    int res = ReadAnalogMeasurements();
    if (res == Error::ER_NOERROR)
    {
        Bac_block.Ama1 = (ChA1->Bda_block.EXTmA1 - RegData) / 16;
        Bac_block.Bma1 = (4*ChA1->Bda_block.EXTmA1 - 20*RegData) / 16;
    }
    return res;
}

int TuneDialogA1::Start6_3_9()
{
    CheckA1::A1_Bd4 tmpst;
    if (Skipped)
        return Error::ER_RESEMPTY;
    if (Commands::GetBd(A1_BDA_OUT_AN_BN, &tmpst, sizeof(CheckA1::A1_Bd4)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_out_an");
        return Error::ER_GENERALERROR;
    }
    if ((tmpst.Tmk < 15) || (tmpst.Tmk > 40)) // температура на 5...12 градусов выше температуры в камере (20 град)
    {
        EMessageBox::information(this, "Внимание", "Температура Tmk находится вне пределов (15...40) град. С");
        return Error::ER_GENERALERROR;
    }
    if ((tmpst.Vbat < 1.7f) || (tmpst.Vbat > 3.4f)) // напряжение батареи
    {
        EMessageBox::information(this, "Внимание", "Напряжение батарейки находится вне пределов (1,7...3,4) В");
        return Error::ER_GENERALERROR;
    }
/*    if ((tmpst.Tamb < 51.85f) || (tmpst.Tamb > 52.05f)) // Температура окр воздуха 51,95+/-0,1 град С при установленном 120 Ом
    {
        EMessageBox::information(this, "Внимание", "Температура окр воздуха вне пределов (51,85...52,02) град С");
        return Error::ER_GENERALERROR;
    }
    if ((tmpst.Hamb < 0) || (tmpst.Vbat > 0.1f)) // Влажность
    {
        EMessageBox::information(this, "Внимание", "Влажность находится вне пределов (0...0,1) %");
        return Error::ER_GENERALERROR;
    } */
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_10_1()
{
    Skipped = false;
    if (EMessageBox::question(this, "Вопрос", "Будет проведена температурная регулировка, выполнить?") == false)
    {
        Bac_block.TKUa[0] = Bac_block_old.TKUa[0];
        Bac_block.TKUa[1] = Bac_block_old.TKUa[1];
        Bac_block.TKUb[0] = Bac_block_old.TKUb[0];
        Bac_block.TKUb[1] = Bac_block_old.TKUb[1];
        Bac_block.Tmk0 = Bac_block_old.Tmk0;
        Skipped = true;
        return Error::ER_RESEMPTY;
    }
    ShowScheme();
    Bac_block.TKUa[0] = Bac_block.TKUb[0] = Bac_block.TKUa[1] = Bac_block.TKUb[1] = 0;
    if (Commands::WriteBac(BT_BASE, &Bac_block, sizeof(Bac_block)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при записи регулировочных коэффициентов");
        return Error::ER_GENERALERROR;
    }
    return Start6_3_10_60(0);
/*    if (Start60PointsMeasurements(TDA1_BD1 | TDA1_BD4, &TKUSourceData.Bda_in[0], &TKUSourceData.Bda_out_an[0]) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    if (GetExternalTemp() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    TKUSourceData.Tmk[0] = RealData.t;
    memcpy(&ChA1->Bda_in, &TKUSourceData.Bda_in[0], sizeof(CheckA1::A1_Bd1));
    ChA1->FillBda_in(this);
    memcpy(&ChA1->Bda_out_an, &TKUSourceData.Bda_out_an[0], sizeof(CheckA1::A1_Bd4));
    ChA1->FillBda_out_an(this);
    return Error::ER_NOERROR; */
}

int TuneDialogA1::Start6_3_10_2()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    if (EMessageBox::question(this, "Требование", "Задайте повышенную температуру в термокамере (+50) °С") == false)
    {
        Skipped = true;
        return Error::ER_RESEMPTY;
    }
    WaitNSeconds(30*60, true);
    StdFunc::ClearCancel(); // если прервали 30-минутный отсчёт, это дело пользователя
    return Start6_3_10_60(1);
/*    if (Start60PointsMeasurements(TDA1_BD1 | TDA1_BD4, &TKUSourceData.Bda_in[1], &TKUSourceData.Bda_out_an[1]) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    if (GetExternalTemp() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    TKUSourceData.Tmk[1] = RealData.t;
    memcpy(&ChA1->Bda_in, &TKUSourceData.Bda_in[1], sizeof(CheckA1::A1_Bd1));
    ChA1->FillBda_in(this);
    memcpy(&ChA1->Bda_out_an, &TKUSourceData.Bda_in[1], sizeof(CheckA1::A1_Bd4));
    ChA1->FillBda_out_an(this);
    return Error::ER_NOERROR; */
}

int TuneDialogA1::Start6_3_10_3()
{
    if (Skipped)
        return Error::ER_RESEMPTY;
    if (EMessageBox::question(this, "Требование", "Задайте пониженную температуру в термокамере (0) °С") == false)
    {
        Skipped = true;
        return Error::ER_RESEMPTY;
    }
    WaitNSeconds(30*60, true);
    StdFunc::ClearCancel(); // если прервали 30-минутный отсчёт, это дело пользователя
    Start6_3_10_60(2);
/*    if (Start60PointsMeasurements(TDA1_BD1 | TDA1_BD4, &TKUSourceData.Bda_in[2], &TKUSourceData.Bda_out_an[2]) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    if (GetExternalTemp() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
    TKUSourceData.Tmk[2] = RealData.t;
    memcpy(&ChA1->Bda_in, &TKUSourceData.Bda_in[2], sizeof(CheckA1::A1_Bd1));
    ChA1->FillBda_in(this);
    memcpy(&ChA1->Bda_out_an, &TKUSourceData.Bda_in[2], sizeof(CheckA1::A1_Bd4));
    ChA1->FillBda_out_an(this); */
/*    float Um[2], Up[2], Uo[2], Tmkm, Tmkp, Tmko;
    Tmkm = TKUSourceData.Tmk[2];
    Tmkp = TKUSourceData.Tmk[1];
    Tmko = TKUSourceData.Tmk[0];
    Uo[0] = TKUSourceData.Bda_in[0].UefNat_filt[0];
    Uo[1] = TKUSourceData.Bda_in[0].UefNat_filt[1];
    Up[0] = TKUSourceData.Bda_in[1].UefNat_filt[0];
    Up[1] = TKUSourceData.Bda_in[1].UefNat_filt[1];
    Um[0] = TKUSourceData.Bda_in[2].UefNat_filt[0];
    Um[1] = TKUSourceData.Bda_in[2].UefNat_filt[1]; */
    double dUm0 = TKUSourceData.Bda_in[2].UefNat_filt[0] / TKUSourceData.Bda_in[0].UefNat_filt[0] - 1;
    double dUp0 = TKUSourceData.Bda_in[1].UefNat_filt[0] / TKUSourceData.Bda_in[0].UefNat_filt[0] - 1;
    double dUm1 = TKUSourceData.Bda_in[2].UefNat_filt[1] / TKUSourceData.Bda_in[0].UefNat_filt[1] - 1;
    double dUp1 = TKUSourceData.Bda_in[1].UefNat_filt[1] / TKUSourceData.Bda_in[0].UefNat_filt[1] - 1;
    double dTm = TKUSourceData.Bda_out_an[2].Tmk - TKUSourceData.Bda_out_an[0].Tmk;
    double dTp = TKUSourceData.Bda_out_an[1].Tmk - TKUSourceData.Bda_out_an[0].Tmk;
    if ((qAbs(dTm) < 1.0f) || (qAbs(dTp) < 1.0f))
    {
        EMessageBox::error(this, "Ошибка", "Разница измеренных температур в опытах \n"
                                           "повышенной и пониженной температур слишком маленькая");
        return Error::ER_GENERALERROR;
    }
    Bac_block.TKUa[0] = (dUm0 * dTp * dTp - dUp0 * dTm * dTm) / (dTp * dTm * (dTp - dTm));
    Bac_block.TKUa[1] = (dUm1 * dTp * dTp - dUp1 * dTm * dTm) / (dTp * dTm * (dTp - dTm));
    Bac_block.TKUb[0] = (dUp0 * dTm - dUm0 * dTp) / (dTp * dTm * (dTp - dTm));
    Bac_block.TKUb[1] = (dUp1 * dTm - dUm1 * dTp) / (dTp * dTm * (dTp - dTm));
    Bac_block.Tmk0 = TKUSourceData.Bda_out_an[0].Tmk;
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_10_60(int index)
{
    if (Start60PointsMeasurements(TDA1_BD1 | TDA1_BD4, &TKUSourceData.Bda_in[index], &TKUSourceData.Bda_out_an[index]) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    if (GetExternalTemp() == Error::ER_GENERALERROR)
        return Error::ER_GENERALERROR;
//    TKUSourceData.Tmk[index] = RealData.t;
    memcpy(&ChA1->Bda_in, &TKUSourceData.Bda_in[index], sizeof(CheckA1::A1_Bd1));
    ChA1->FillBda_in(this);
    memcpy(&ChA1->Bda_out_an, &TKUSourceData.Bda_out_an[index], sizeof(CheckA1::A1_Bd4));
    ChA1->FillBda_out_an(this);
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_11()
{
    FillBac(1);
    if (EMessageBox::question(this, "Вопрос", "Сохранить регулировочные коэффициенты?") == false)
        return Error::ER_GENERALERROR;
    SaveToFileEx(1);
    if (!WriteTuneCoefs(1))
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialogA1::Start6_3_12()
{
    QString tmps = "Пожалуйста, после нажатия \"ОК\" просмотрите текущие данные после регулировки в соответствующих окнах";
    if (TuneFileSaved)
        tmps += "\nЕсли в процессе регулировки произошла ошибка, сохранённые коэффициенты\n"
                "Вы можете загрузить из файла "+StdFunc::GetSystemHomeDir()+"temptune.tn1";
    tmps += "\nПосле окончания проверки нажмите Enter для завершения процедуры регулировки";
    EMessageBox::information(this, "Завершение регулировки", tmps);
    QTabWidget *TuneTW = this->findChild<QTabWidget *>("tunetw");
    if (TuneTW != nullptr)
        TuneTW->setCurrentIndex(2); // показ данных измерений
    connect(this,SIGNAL(Finished()),this,SLOT(Good()));
    StartMeasurement();
    if (TuneTW != nullptr)
        TuneTW->setCurrentIndex(0);
    return Error::ER_NOERROR;
}

// принимаем в течение PovNumPoints точек по TUNE_POINTSPER секунд данные по Bda_in и усредняем их

int TuneDialogA1::Start60PointsMeasurements(int whichtomeasure, void *dst1, void *dst2, void *dst3)
{
    CheckA1::A1_Bd1 *out1;
    CheckA1::A1_Bd4 *out2;
    CheckA1::Bda *out3;
    bool out1busy, out2busy;

    out1busy = out2busy = false;

    if (whichtomeasure & TDA1_BD1)
    {
        out1 = static_cast<CheckA1::A1_Bd1 *>(dst1);
        out1->Frequency = out1->Phy = out1->UefNat_filt[0] = out1->UefNat_filt[1] = \
                out1->Uef_filt[0] = out1->Uef_filt[1] = out1->dU = out1->dUrms = 0;
        out1busy = true;
    }
    if (whichtomeasure & TDA1_BD4)
    {
        if (out1busy)
        {
            out2 = static_cast<CheckA1::A1_Bd4 *>(dst2);
            out2busy = true;
        }
        else
        {
            out2 = static_cast<CheckA1::A1_Bd4 *>(dst1);
            out1busy = true;
        }
        out2->Hamb = out2->Tamb = out2->Tmk = out2->Vbat = 0;
    }
    if (whichtomeasure & TDA1_BDA)
    {
        if (out1busy)
        {
            if (out2busy)
                out3 = static_cast<CheckA1::Bda *>(dst3);
            else
                out3 = static_cast<CheckA1::Bda *>(dst2);
        }
        else
            out3 = static_cast<CheckA1::Bda *>(dst1);
        out3->EXTmA1 = out3->EXTmA2 = out3->Frequency = out3->Middle_ADC[0] = out3->Middle_ADC[1] = \
                out3->Pt100 = out3->Ueff_ADC[0] = out3->Ueff_ADC[1] = 0;
    }
    int count = 0;
    int res = Error::ER_NOERROR;
    emit StartPercents(PovNumPoints);
    WaitWidget *w = new WaitWidget;
    w->SetMessage("Пожалуйста, подождите...");
    WaitWidget::ww_struct ww;
    ww.isincrement = true;
    ww.isallowedtostop = false;
    ww.format = WaitWidget::WW_SIMPLE;
    ww.initialseconds = 0;
    w->Init(ww);
    w->Start();
    while ((count < PovNumPoints) && !StdFunc::IsCancelled())
    {
        if (whichtomeasure & TDA1_BD1)
            res += MeasureBd1(out1);
        if (whichtomeasure & TDA1_BD4)
            res += MeasureBd4(out2);
        if (whichtomeasure & TDA1_BDA)
            res += MeasureBda(out3);
        if (res != Error::ER_NOERROR)
        {
            w->Stop();
            return Error::ER_GENERALERROR;
        }
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    if (StdFunc::IsCancelled())
    {
        w->Stop();
        return Error::ER_GENERALERROR;
    }
    if (whichtomeasure & TDA1_BD1)
        Bd1Mean(count, out1);
    if (whichtomeasure & TDA1_BD4)
        Bd4Mean(count, out2);
    if (whichtomeasure & TDA1_BDA)
        BdaMean(count, out3);
    w->Stop();
    return Error::ER_NOERROR;
}

int TuneDialogA1::MeasureBd1(CheckA1::A1_Bd1 *result)
{
    CheckA1::A1_Bd1 tmpbd1;

    if (Commands::GetBd(A1_BDA_IN_BN, &tmpbd1, sizeof(CheckA1::A1_Bd1)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
        return Error::ER_GENERALERROR;
    }
    result->Frequency += tmpbd1.Frequency;
    result->Phy += tmpbd1.Phy;
    result->UefNat_filt[0] += tmpbd1.UefNat_filt[0];
    result->UefNat_filt[1] += tmpbd1.UefNat_filt[1];
    result->Uef_filt[0] += tmpbd1.Uef_filt[0];
    result->Uef_filt[1] += tmpbd1.Uef_filt[1];
    result->dU += tmpbd1.dU;
    result->dUrms += tmpbd1.dUrms;
    return 0;
}

int TuneDialogA1::MeasureBd4(CheckA1::A1_Bd4 *result)
{
    CheckA1::A1_Bd4 tmpbd4;

    if (Commands::GetBd(A1_BDA_OUT_AN_BN, &tmpbd4, sizeof(CheckA1::A1_Bd4)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_out_an");
        return Error::ER_GENERALERROR;
    }
    result->Hamb += tmpbd4.Hamb;
    result->Tamb += tmpbd4.Tamb;
    result->Tmk += tmpbd4.Tmk;
    result->Vbat += tmpbd4.Vbat;
    return 0;
}

int TuneDialogA1::MeasureBda(CheckA1::Bda *result)
{
    CheckA1::Bda tmpbda;

    if (Commands::GetBda(BT_BASE, &tmpbda, sizeof(CheckA1::Bda)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda");
        return Error::ER_GENERALERROR;
    }
    result->EXTmA1 += tmpbda.EXTmA1;
    result->EXTmA2 += tmpbda.EXTmA2;
    result->Frequency += tmpbda.Frequency;
    result->Pt100 += tmpbda.Pt100;
    result->Middle_ADC[0] += tmpbda.Middle_ADC[0]; result->Middle_ADC[1] += tmpbda.Middle_ADC[1];
    result->Ueff_ADC[0] += tmpbda.Ueff_ADC[0]; result->Ueff_ADC[1] += tmpbda.Ueff_ADC[1];
    return 0;
}

void TuneDialogA1::Bd1Mean(int count, CheckA1::A1_Bd1 *result)
{
    result->Frequency /= count;
    result->Phy /= count;
    result->UefNat_filt[0] /= count;
    result->UefNat_filt[1] /= count;
    result->Uef_filt[0] /= count;
    result->Uef_filt[1] /= count;
    result->dU /= count;
    result->dUrms /= count;
}

void TuneDialogA1::Bd4Mean(int count, CheckA1::A1_Bd4 *result)
{
    result->Hamb /= count;
    result->Tamb /= count;
    result->Tmk /= count;
    result->Vbat /= count;
}

void TuneDialogA1::BdaMean(int count, CheckA1::Bda *result)
{
    result->EXTmA1 /= count;
    result->EXTmA2 /= count;
    result->Frequency /= count;
    result->Pt100 /= count;
    result->Middle_ADC[0] /= count;
    result->Middle_ADC[1] /= count;
    result->Ueff_ADC[0] /= count;
    result->Ueff_ADC[1] /= count;
}

int TuneDialogA1::ShowScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/tunea1.png")));
    lyout->addWidget(WDFunc::NewLBL(this, "1. На выходах РЕТОМ задайте частоту 51,0 Гц, уровень напряжения фазы А 100 В с фазой 0 градусов, включите режим однофазного выхода;"));
    lyout->addWidget(WDFunc::NewLBL(this, "2. Включите питание прибора Энергомонитор-3.1КМ и настройте его следующим образом:"));
    lyout->addWidget(WDFunc::NewLBL(this, "  2.1. Схема подключения: однофазная двухпроводная;"));
    lyout->addWidget(WDFunc::NewLBL(this, "  2.2. Режим измерения: диапазон 0...240 В;"));
    lyout->addWidget(WDFunc::NewLBL(this, "3. Перейдите в окно Энергомонитора \"Измерения - Переменного тока - Гармоники\";"));
    lyout->addWidget(WDFunc::NewLBL(this, "4. На магазине сопротивлений установите значение сопротивления 100,0 Ом (при необходимости регулировки канала Pt100);"));
    lyout->addWidget(WDFunc::NewLBL(this, "5. Подключите калибратор токовой петли на вход 2 и установите выходной ток 20 мА (при необходимости регулировки каналов 4..20 мА);"));
    lyout->addWidget(WDFunc::NewLBL(this, "6. Включите выходы РЕТОМ;"));
    lyout->addWidget(WDFunc::NewLBL(this, "7. Задайте нормальную температуру в термокамере +20±5 °C."));
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    if (StdFunc::IsCancelled() == true)
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialogA1::CheckBdaValues(int checktype)
{
    if (checktype == CHECK_VOLT)
    {
        if (!IsWithinLimits(ChA1->Bda_block.Ueff_ADC[0], 3900000.0, 400000.0))
            return Error::ER_GENERALERROR;
        if (!IsWithinLimits(ChA1->Bda_block.Ueff_ADC[1], 3900000.0, 400000.0))
            return Error::ER_GENERALERROR;
        if (!IsWithinLimits(ChA1->Bda_block.Frequency, 51.0, 0.05))
            return Error::ER_GENERALERROR;
    }
    if (checktype == CHECK_PT100)
    {
        if (!IsWithinLimits(ChA1->Bda_block.Pt100, 492, 150.0))
            return Error::ER_GENERALERROR;
    }
    if (checktype == CHECK_MA1)
    {
        if (!IsWithinLimits(ChA1->Bda_block.EXTmA1, 655.0, 25.0))
            return Error::ER_GENERALERROR;
    }
    if (checktype == CHECK_MA2)
    {
        if (!IsWithinLimits(ChA1->Bda_block.EXTmA2, 3275.0, 75.0))
            return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

int TuneDialogA1::CheckAnalogValues(bool isPrecise)
{
    double Tols[6] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.15};
    double PrecTols[6] = {0.03, 0.03, 0.03, 0.03, 0.025, 0.05};
    double *T;
    T = (isPrecise) ? &PrecTols[0] : &Tols[0];
    if (!IsWithinLimits(ChA1->Bda_in.UefNat_filt[0], RealData.u, T[0]))
        return Error::ER_GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.UefNat_filt[1], RealData.u, T[1]))
        return Error::ER_GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Uef_filt[0], RealData.u, T[2]))
        return Error::ER_GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Uef_filt[0], RealData.u, T[3]))
        return Error::ER_GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Frequency, RealData.freq, T[4]))
        return Error::ER_GENERALERROR;
    if (!IsWithinLimits(ChA1->Bda_in.Phy, 0, T[5]))
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialogA1::GetExternalData()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("extdatad");
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(dlg, "Введите значения сигналов по приборам"),0,0,1,4);
    glyout->addWidget(WDFunc::NewLBL(dlg, "Значение напряжения, В"), 1,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "u", -125, 125, 5), 1,1,1,1);
    glyout->addWidget(WDFunc::NewLBL(dlg, "Частота, Гц"), 2,2,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "freq", -100, 100, 5), 2,3,1,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetExtData()));
    glyout->addWidget(pb,4,0,1,3);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelExtData()));
    glyout->addWidget(pb,4,3,1,3);
    dlg->setLayout(glyout);
    dlg->exec();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialogA1::GetExternalTemp()
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("extdatat");
    QGridLayout *glyout = new QGridLayout;
    glyout->addWidget(WDFunc::NewLBL(dlg, "Введите текущее значение температуры в камере:"),0,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(dlg, "t", -125, 125, 3),0,1,1,1);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(SetExtTemp()));
    glyout->addWidget(pb,4,0,1,3);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked()),this,SLOT(CancelExtData()));
    glyout->addWidget(pb,4,3,1,3);
    dlg->setLayout(glyout);
    dlg->exec();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

void TuneDialogA1::GetBdAndFillMTT()
{
    if (Commands::GetBd(A1_BDA_IN_BN, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
        ChA1->FillBda_in(this);
    if (Commands::GetBd(A1_BDA_IN_AN_BN, &ChA1->Bda_in_an, sizeof(CheckA1::A1_Bd3)) == Error::ER_NOERROR)
        ChA1->FillBda_in_an(this);
    if (Commands::GetBda(BT_BASE, &ChA1->Bda_block, sizeof(CheckA1::Bda)) == Error::ER_NOERROR)
        ChA1->FillBda(this);
}

// ####################### SLOTS #############################

int TuneDialogA1::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    if (Commands::GetBda(BT_BASE, &ChA1->Bda_block, sizeof(CheckA1::Bda)) != Error::ER_NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda");
        return Error::ER_GENERALERROR;
    }
    ChA1->FillBda(this);
    return Error::ER_NOERROR;
}
#endif

void TuneDialogA1::LoadSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    PovNumPoints = sets->value("PovNumPoints", "60").toInt();
}

void TuneDialogA1::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
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
    WDFunc::SetLEData(this, "tune31", QString::number(Bac_block.TKUa[0], 'e', 5));
    WDFunc::SetLEData(this, "tune32", QString::number(Bac_block.TKUa[1], 'e', 5));
    WDFunc::SetLEData(this, "tune33", QString::number(Bac_block.TKUb[0], 'e', 5));
    WDFunc::SetLEData(this, "tune34", QString::number(Bac_block.TKUb[1], 'e', 5));
    WDFunc::SetLEData(this, "tune35", QString::number(Bac_block.Tmk0, 'f', 5));
    WDFunc::SetLEData(this, "tune36", QString::number(Bac_block.ch_offset[0], 'f', 5));
    WDFunc::SetLEData(this, "tune37", QString::number(Bac_block.ch_offset[1], 'f', 5));
}

void TuneDialogA1::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum);
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
    WDFunc::LENumber(this, "tune31", Bac_block.TKUa[0]);
    WDFunc::LENumber(this, "tune32", Bac_block.TKUa[1]);
    WDFunc::LENumber(this, "tune33", Bac_block.TKUb[0]);
    WDFunc::LENumber(this, "tune34", Bac_block.TKUb[1]);
    WDFunc::LENumber(this, "tune35", Bac_block.Tmk0);
    WDFunc::LENumber(this, "tune36", Bac_block.ch_offset[0]);
    WDFunc::LENumber(this, "tune37", Bac_block.ch_offset[1]);
}

// для своей калибровки блок Bac только один, и bacnum = 1
void TuneDialogA1::SetDefCoefs()
{
    Bac_block.KmU[0] = static_cast<float>(0.97330302);
    Bac_block.KmU[1] = static_cast<float>(0.98191289);
    Bac_block.K_freq = static_cast<float>(0.999962);
    Bac_block.DPhy = static_cast<float>(-0.009118);
    Bac_block.Art = static_cast<float>(82.00767);
    Bac_block.Brt = static_cast<float>(6010.39014);
    Bac_block.Ama1 = static_cast<float>(163.76575);
    Bac_block.Bma1 = static_cast<float>(-0.58002);
    Bac_block.Ama2 = static_cast<float>(163.2626);
    Bac_block.Bma2 = static_cast<float>(-0.8425);
    Bac_block.TKUa[0] = Bac_block.TKUa[1] = Bac_block.TKUb[0] = Bac_block.TKUb[1] = 0;
    Bac_block.Tmk0 = 31;
    Bac_block.ch_offset[0] = 2649.0;
    Bac_block.ch_offset[1] = 1835.0;
    FillBac(1);
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialogA1::SetExtData()
{
    QDialog *dlg = this->findChild<QDialog *>("extdatad");
    if (dlg == nullptr)
        return;
    WDFunc::SPBData(dlg, "u", RealData.u);
    WDFunc::SPBData(dlg, "freq", RealData.freq);
    StdFunc::ClearCancel();
    dlg->close();
}

void TuneDialogA1::CancelExtData()
{
    CancelExt("extdatad");
}

void TuneDialogA1::SetExtTemp()
{
    QDialog *dlg = this->findChild<QDialog *>("extdatat");
    if (dlg == nullptr)
        return;
    WDFunc::SPBData(dlg, "t", RealData.t);
    StdFunc::ClearCancel();
    dlg->close();
}

void TuneDialogA1::CancelExtTemp()
{
    CancelExt("extdatat");
}

void TuneDialogA1::CancelExt(const QString &dlgname)
{
    QDialog *dlg = this->findChild<QDialog *>(dlgname);
    if (dlg == nullptr)
        return;
    StdFunc::Cancel();
    dlg->close();
}
#endif
