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
#include "tunedialoga1dn.h"
#include "../canal.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"

TuneDialogA1DN::TuneDialogA1DN(QWidget *parent) :
    AbstractTuneDialog(parent)
{
    CA1 = new ConfigA1(S2Config);
    ChA1 = new CheckA1;
    SetBac(&Bac_block, BT_MEZONIN, sizeof(Bac_block));
    SetupUI();
}

void TuneDialogA1DN::SetLbls()
{
    lbls.append("7.2.2. Ввод пароля...");
    lbls.append("7.2.1. Отображение диалога проверки схемы подключения...");
    lbls.append("7.2.2. Приём конфигурации от прибора...");
    lbls.append("7.2.3. Установка 20%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 50%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 80%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 100%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 120%, проверка и сохранение...");
    lbls.append("7.2.6. Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("7.2.8. Проверка аналоговых данных...");
    lbls.append("Настройка окончена!");
}

void TuneDialogA1DN::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &AbstractTuneDialog::CheckPassword; // 7.2.2. Ввод пароля
    int (AbstractTuneDialog::*func)() = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::ShowScheme); // 7.2.1. Отображение диалога проверки схемы подключения
    pf[lbls.at(count++)] = func; // 7.2.2. Приём конфигурации от прибора
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_1); // 7.2.3. Установка 20%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_2); // 7.2.5. Установка 50%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_3); // 7.2.5. Установка 80%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_4); // 7.2.5. Установка 100%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_5); // 7.2.5. Установка 120%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_67); // 7.2.6. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((AbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_8); // 7.2.8. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
}

void TuneDialogA1DN::SetupUI()
{
    int i;
    QWidget *cp1 = new QWidget;
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

    TuneTW->addTab(cp1,"Настройка штатного ДН");
    TuneTW->addTab(cp2,"Коэффициенты");
    TuneTW->addTab(cp3,"Данные измерений");

    // CP1 - НАСТРОЙКА ДН

    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Измерения в первичном масштабе");
    glyout->addWidget(WDFunc::NewLBL(this, "U1"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "ΔU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreq", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    glyout = new QGridLayout;
    gb = new QGroupBox("Измерения в масштабе, приведённом ко входу");
    glyout->addWidget(WDFunc::NewLBL(this, "U1"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1i", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2i", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "ΔU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercenti", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphyi", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreqi", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(5);
    QPushButton *pb = new QPushButton("Годится");
    pb->setObjectName("GoodDN");
    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Good()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    pb = new QPushButton("Отмена");
    pb->setObjectName("NoGoodDN");
    pb->setEnabled(false);
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(NoGood()));
    hlyout->addWidget(pb);
    hlyout->addStretch(5);
    lyout->addLayout(hlyout);

    lyout->addWidget(TuneUI());
    lyout->addStretch(1);
    cp1->setLayout(lyout);

    // CP2 - КОЭФФИЦИЕНТЫ МОДУЛЯ

    lyout = new QVBoxLayout;
    glyout = new QGridLayout;
    gb = new QGroupBox("Настроечные коэффициенты");
    for (i = 0; i < 6; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "U1kDN["+QString::number(i)+"]"),0,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+4), "", ValuesLEFormat),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "U2kDN["+QString::number(i)+"]"),2,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+10), "", ValuesLEFormat),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PhyDN["+QString::number(i)+"]"),4,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+16), "", ValuesLEFormat),5,i,1,1);
        if (i < 5)
        {
            glyout->addWidget(WDFunc::NewLBL(this, "δU["+QString::number(i)+"]"),6,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+22), "", ValuesLEFormat),7,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "Δφ["+QString::number(i)+"]"),8,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+27), "", ValuesLEFormat),9,i,1,1);
        }
    }
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
    gb = new QGroupBox("Напряжения в масштабе входных сигналов (Bda_in)");
    vlyout = new QVBoxLayout;
    vlyout->addWidget(ChA1->Bda_inW(this));
    gb->setLayout(vlyout);
    lyout->addWidget(gb);
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

void TuneDialogA1DN::FillBac()
{
    for (int i = 0; i < 6; ++i)
    {
        WDFunc::SetLEData(this, "tune"+QString::number(i+4), QString::number(Bac_block.U1kDN[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+10), QString::number(Bac_block.U2kDN[i], 'f', 5));
        WDFunc::SetLEData(this, "tune"+QString::number(i+16), QString::number(Bac_block.PhyDN[i], 'f', 5));
        if (i < 5)
        {
            WDFunc::SetLEData(this, "tune"+QString::number(i+22), QString::number(Bac_block.dU_cor[i], 'f', 5));
            WDFunc::SetLEData(this, "tune"+QString::number(i+27), QString::number(Bac_block.dPhy_cor[i], 'f', 5));
        }
    }
}

void TuneDialogA1DN::FillBackBac()
{
    for (int i = 0; i < 6; i++)
    {
        WDFunc::LENumber(this, "tune"+QString::number(i+4), Bac_block.U1kDN[i]);
        WDFunc::LENumber(this, "tune"+QString::number(i+11), Bac_block.U2kDN[i]);
        WDFunc::LENumber(this, "tune"+QString::number(i+18), Bac_block.PhyDN[i]);
        if (i < 5)
        {
            WDFunc::LENumber(this, "tune"+QString::number(i+22), Bac_block.dU_cor[i]);
            WDFunc::LENumber(this, "tune"+QString::number(i+27), Bac_block.dPhy_cor[i]);
        }
    }
}

void TuneDialogA1DN::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.Uef_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphy", QString::number(ChA1->Bda_out.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreq", QString::number(ChA1->Bda_out.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercent", QString::number(ChA1->Bda_out.dUrms, 'f', 5));
}

void TuneDialogA1DN::FillBackBdOut()
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

void TuneDialogA1DN::SetDefCoefs()
{
    Bac_block.U1kDN[0] = 0;
    Bac_block.U2kDN[0] = 0;
    Bac_block.PhyDN[0] = 0;
    Bac_block.U1kDN[1] = Bac_block.U2kDN[1] = 12;
    Bac_block.U1kDN[2] = Bac_block.U2kDN[2] = 30;
    Bac_block.U1kDN[3] = Bac_block.U2kDN[3] = 48;
    Bac_block.U1kDN[4] = Bac_block.U2kDN[4] = 60;
    Bac_block.U1kDN[5] = Bac_block.U2kDN[5] = 71;
    Bac_block.PhyDN[1] = Bac_block.PhyDN[2] = Bac_block.PhyDN[3] = \
            Bac_block.PhyDN[4] = Bac_block.PhyDN[5] = 0;
    Bac_block.dU_cor[0] = Bac_block.dPhy_cor[0] = 0;
    Bac_block.dU_cor[1] = Bac_block.dPhy_cor[1] = 0;
    Bac_block.dU_cor[2] = Bac_block.dPhy_cor[2] = 0;
    Bac_block.dU_cor[3] = Bac_block.dPhy_cor[3] = 0;
    Bac_block.dU_cor[4] = Bac_block.dPhy_cor[4] = 0;
    FillBac();
}

int TuneDialogA1DN::Start7_2_2()
{
    cn->Send(CN_GF, BT_NONE,NULL,0,1,&S2Config);
    if (cn->result == NOERROR)
    {
        Bac_block.U1kDN[0] = 0;
        Bac_block.U2kDN[0] = 0;
        Bac_block.PhyDN[0] = 0;
        return NOERROR;
    }
    return GENERALERROR;
}

int TuneDialogA1DN::Start7_2_3_1()
{
    QPushButton *pb = this->findChild<QPushButton *>("GoodDN");
    if (pb != 0)
        pb->setText("Годится");
    return Start7_2_345(0);
}

int TuneDialogA1DN::Start7_2_3_2()
{
    return Start7_2_345(1);
}

int TuneDialogA1DN::Start7_2_3_3()
{
    return Start7_2_345(2);
}

int TuneDialogA1DN::Start7_2_3_4()
{
    return Start7_2_345(3);
}

int TuneDialogA1DN::Start7_2_3_5()
{
    return Start7_2_345(4);
}

int TuneDialogA1DN::Start7_2_345(int counter)
{
    const int Percents[] = {20,50,80,100,120};
    if (counter > 4)
        return GENERALERROR;
    float VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * Percents[counter] / 1732;
    if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + QString::number(VoltageInkV, 'f', 1) + " кВ") == false)
    {
        Cancelled = true;
        return GENERALERROR;
    }
    if (StartMeasurement() != NOERROR)
        return GENERALERROR;
    FillBackBdOut();
    // теперь в ChA1->Bda_block лежат нужные нам значения
    Bac_block.U1kDN[counter+1] = ChA1->Bda_in.Uef_filt[0];
    Bac_block.U2kDN[counter+1] = ChA1->Bda_in.Uef_filt[1];
    Bac_block.PhyDN[counter+1] = ChA1->Bda_in.Phy;
    FillBac();
    return NOERROR;
}

int TuneDialogA1DN::Start7_2_67()
{
    WriteTuneCoefs();
    if (cn->result != NOERROR)
        return GENERALERROR;
    return NOERROR;
/*    if (cn->result == NOERROR)
    {
        MessageBox2::information(this, "Внимание", "Записано успешно!");
        return NOERROR;
    }
    return GENERALERROR; */
}

int TuneDialogA1DN::Start7_2_8()
{
    if (MessageBox2::question(this, "Подтверждение", "Теперь необходимо подтвердить погрешности установки") == false)
    {
        Cancelled = true;
        return GENERALERROR;
    }
    WaitNSeconds(5);
    QPushButton *pb = this->findChild<QPushButton *>("GoodDN");
    if (pb != 0)
        pb->setText("Запомнить погрешность");
    if (StartMeasurement() != NOERROR)
        return GENERALERROR;
    FillBackBdOut();
    Bac_block.dU_cor[0] = ChA1->Bda_out.dUrms;
    Bac_block.dPhy_cor[0] = ChA1->Bda_out.Phy;
    FillBac();
    return NOERROR;
}

int TuneDialogA1DN::Start7_2_9_1()
{
    return Start7_2_9(0);
}

int TuneDialogA1DN::Start7_2_9_2()
{
    return Start7_2_9(1);
}

int TuneDialogA1DN::Start7_2_9_3()
{
    return Start7_2_9(2);
}

int TuneDialogA1DN::Start7_2_9_4()
{
    if (Start7_2_9(3) != NOERROR)
        return GENERALERROR;
    SaveToFile();
    return Start7_2_67();
}

int TuneDialogA1DN::Start7_2_9(int counter)
{
    const int Percents[] = {100, 80, 50, 20};
    if (counter > 3)
        return GENERALERROR;
    float VoltageInkV = static_cast<float>(CA1->Bci_block.K_DN) * Percents[counter] / 1732;
    if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + QString::number(VoltageInkV, 'f', 1) + " кВ") == false)
    {
        Cancelled = true;
        return GENERALERROR;
    }
    if (StartMeasurement() != NOERROR)
        return GENERALERROR;
    FillBackBdOut();
    // теперь в ChA1->Bda_block лежат нужные нам значения
    Bac_block.dU_cor[counter+1] = ChA1->Bda_out.dUrms;
    Bac_block.dPhy_cor[counter+1] = ChA1->Bda_out.Phy;
    FillBac();
    return NOERROR;
}

int TuneDialogA1DN::ReadAnalogMeasurements()
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

int TuneDialogA1DN::ShowScheme()
{
    if (MessageBox2::question(this, "Подтверждение", "Подключите выход своего делителя напряжения ко входу U1 прибора\n"
                              "Выход эталонного делителя - ко входу U2") == false)
    {
        Cancelled = true;
        return GENERALERROR;
    }
    return NOERROR;
}

void TuneDialogA1DN::GetBdAndFillMTT()
{
    cn->Send(CN_GBd, 4, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
    if (cn->result == NOERROR)
        FillBdOut();
}
