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
#include "../gen/commands.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/waitwidget.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

TuneDialogA1DN::TuneDialogA1DN(QWidget *parent) :
    EAbstractTuneDialog(parent)
{
    CA1 = new ConfigA1(S2Config);
    ChA1 = new CheckA1;
    SetBac(&Bac_block, BT_MEZONIN, sizeof(Bac));
    SetupUI();
}

TuneDialogA1DN::~TuneDialogA1DN()
{
}

void TuneDialogA1DN::SetLbls()
{
    lbls.append("7.2.2. Ввод пароля...");
    lbls.append("7.2.1. Отображение диалога проверки схемы подключения...");
    lbls.append("Ввод данных по делителю...");
    lbls.append("7.2.2. Приём конфигурации от прибора...");
    lbls.append("7.2.3. Установка 20%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 50%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 80%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 100%, проверка и сохранение...");
    lbls.append("7.2.5. Установка 120%, проверка и сохранение...");
    lbls.append("7.2.6. Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("7.2.8. Проверка аналоговых данных...");
    lbls.append("7.2.9.1. Проверка аналоговых данных...");
    lbls.append("7.2.9.2. Проверка аналоговых данных...");
    lbls.append("7.2.9.3. Проверка аналоговых данных...");
    lbls.append("7.2.9.4. Проверка аналоговых данных...");
    lbls.append("7.2.9.5. Проверка аналоговых данных...");
    lbls.append("7.2.9.6. Проверка аналоговых данных...");
    lbls.append("7.2.9.7. Проверка аналоговых данных...");
}

void TuneDialogA1DN::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword; // 7.2.2. Ввод пароля
    int (EAbstractTuneDialog::*func)() = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::ShowScheme); // 7.2.1. Отображение диалога проверки схемы подключения
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_2); // 7.2.2. Приём конфигурации от прибора
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::InputDNData); // Ввод данных по делителю
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_1); // 7.2.3. Установка 20%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_2); // 7.2.5. Установка 50%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_3); // 7.2.5. Установка 80%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_4); // 7.2.5. Установка 100%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3_5); // 7.2.5. Установка 120%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_67); // 7.2.6. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_8); // 7.2.8. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_1); // 7.2.9.1. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_2); // 7.2.9.2. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_3); // 7.2.9.3. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_4); // 7.2.9.4. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_5); // 7.2.9.5. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_6); // 7.2.9.6. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_9_7); // 7.2.9.7. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
}

void TuneDialogA1DN::SetupUI()
{
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = CoefUI(0);
    QWidget *cp3 = CoefUI(1);
    QWidget *cp4 = CoefUI(2);
    QWidget *cp5 = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *TuneTW = new QTabWidget;

    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    tmps = "QWidget {background-color: "+QString(UCONFWCLR)+";}";
    cp1->setStyleSheet(tmps);
    cp2->setStyleSheet(tmps);
    cp3->setStyleSheet(tmps);
    cp4->setStyleSheet(tmps);
    cp5->setStyleSheet(tmps);

    TuneTW->addTab(cp1,"Настройка штатного ДН");
    TuneTW->addTab(cp2,"Коэф. 1");
    TuneTW->addTab(cp3,"Коэф. 2");
    TuneTW->addTab(cp4,"Коэф. 3");
    TuneTW->addTab(cp5,"Данные измерений");

    // CP1 - НАСТРОЙКА ДН

    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Вариант использования: "));
    hlyout->addWidget(WDFunc::NewLBLT(this, "", "tune00", ValuesFormat));
    lyout->addLayout(hlyout);
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Измерения в первичном масштабе");
    glyout->addWidget(WDFunc::NewLBL(this, "U1, кВ"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2, кВ"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "ΔU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq, Гц"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreq", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    glyout = new QGridLayout;
    gb = new QGroupBox("Измерения в масштабе, приведённом ко входу");
    glyout->addWidget(WDFunc::NewLBL(this, "U1, В"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1i", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2, В"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2i", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "ΔU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercenti", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphyi", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq, Гц"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreqi", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    hlyout = new QHBoxLayout;
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

    glyout = new QGridLayout;
    gb = new QGroupBox("СКО");
    glyout->addWidget(WDFunc::NewLBL(this, "dUrms(m)"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunedurmsm", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "φ(m)"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunephym", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σUrms"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesurms", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σφ"), 1, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesphy", ValuesFormat, ""), 1, 3, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    lyout->addWidget(TuneUI());
//    lyout->addStretch(1);
    cp1->setLayout(lyout);

    // CP5 - Данные измерений

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
    cp5->setLayout(lyout);

    lyout = new QVBoxLayout;
    lyout->addWidget(TuneTW);
    setLayout(lyout);
    for (int i=0; i<TUNEVARIANTSNUM; ++i)
        WDFunc::SetLEData(this, "DividerSN"+QString::number(i), "00000000", "^\\d{8}$");
}

QWidget *TuneDialogA1DN::CoefUI(int bac2num)
{
    QString Si = QString::number(bac2num);
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString tmps = "QWidget {background-color: "+QString(UCONFWCLR)+";}";
    QString ValuesLEFormat = "QLineEdit {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(UCONFWCLR)+"; font: bold 10px;}";
    hlyout->addWidget(WDFunc::NewLBL(this,"Заводской номер делителя:"), 0);
    hlyout->addWidget(WDFunc::NewLE(this, "DividerSN"+Si, "", tmps),10);
    hlyout->addWidget(WDFunc::NewLBL(this,"Номинальный коэффициент деления ДН:"), 0);
    hlyout->addWidget(WDFunc::NewSPB(this,"K_DNSPB."+Si,1,10000,1,0,UCONFWCLR));
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);

    glyout = new QGridLayout;
//    gb = new QGroupBox("Настроечные коэффициенты");
    for (int i = 0; i < 6; ++i)
    {
        glyout->addWidget(WDFunc::NewLBL(this, "U1kDN["+QString::number(i)+"]"),0,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+4)+"."+Si, "", ValuesLEFormat),1,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "U2kDN["+QString::number(i)+"]"),2,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+10)+"."+Si, "", ValuesLEFormat),3,i,1,1);
        glyout->addWidget(WDFunc::NewLBL(this, "PhyDN["+QString::number(i)+"]"),4,i,1,1);
        glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+16)+"."+Si, "", ValuesLEFormat),5,i,1,1);
        if (i < 5)
        {
            glyout->addWidget(WDFunc::NewLBL(this, "δU["+QString::number(i)+"]"),6,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+22)+"."+Si, "", ValuesLEFormat),7,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "Δφ["+QString::number(i)+"]"),8,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+27)+"."+Si, "", ValuesLEFormat),9,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "σU["+QString::number(i)+"]","","","СКО амплитудной погрешности"),10,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+32)+"."+Si, "", ValuesLEFormat),11,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "σφ["+QString::number(i)+"]","","","СКО фазовой погрешности"),12,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+37)+"."+Si, "", ValuesLEFormat),13,i,1,1);
        }
    }
    lyout->addLayout(glyout);
    lyout->addWidget(BottomUI());
    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

int TuneDialogA1DN::InputDNData()
{
    Commands::GetBac(BT_MEZONIN, &Bac_block, sizeof(Bac));
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Данные на ТН(ДН)"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент деления ТН(ДН)"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "kdnle", QString::number(Bac_block.Bac_block[TuneVariant].K_DN)), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер ТН(ДН)"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "dnfnumle", QString::number(Bac_block.DNFNum)), row++, 1, 1, 1, Qt::AlignLeft);

    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(AcceptDNData()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    connect(this,SIGNAL(DNDataIsSet()),dlg,SLOT(close()));
    Accepted = false;
    dlg->show();
    while (!Accepted && !pc.Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    if (pc.Cancelled)
        return GENERALERROR;
    return NOERROR;
}

void TuneDialogA1DN::FillBac()
{
    for (int j=0; j<TUNEVARIANTSNUM; ++j)
    {
        QString Si = QString::number(j);
        WDFunc::SetSPBData(this, "K_DNSPB."+Si, Bac_block.Bac_block[j].K_DN);
        for (int i = 0; i < 6; ++i)
        {
            WDFunc::SetLEData(this, "tune"+QString::number(i+4)+"."+Si, QString::number(Bac_block.Bac_block[j].U1kDN[i], 'f', 5));
            WDFunc::SetLEData(this, "tune"+QString::number(i+10)+"."+Si, QString::number(Bac_block.Bac_block[j].U2kDN[i], 'f', 5));
            WDFunc::SetLEData(this, "tune"+QString::number(i+16)+"."+Si, QString::number(Bac_block.Bac_block[j].PhyDN[i], 'f', 5));
            if (i < 5)
            {
                WDFunc::SetLEData(this, "tune"+QString::number(i+22)+"."+Si, QString::number(Bac_block.Bac_block[j].dU_cor[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+27)+"."+Si, QString::number(Bac_block.Bac_block[j].dPhy_cor[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+32)+"."+Si, QString::number(Bac_block.Bac_block[j].ddU_cor[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+37)+"."+Si, QString::number(Bac_block.Bac_block[j].ddPhy_cor[i], 'f', 5));
            }
        }
        WDFunc::SetLEData(this, "DividerSN"+Si, QString::number(Bac_block.DNFNum));
    }
}

void TuneDialogA1DN::FillBackBac()
{
    QString tmps;
    for (int j=0; j<TUNEVARIANTSNUM; ++j)
    {
        QString Si = QString::number(j);
        WDFunc::SPBData(this, "K_DNSPB."+Si, Bac_block.Bac_block[j].K_DN);
        for (int i = 0; i < 6; i++)
        {
            WDFunc::LENumber(this, "tune"+QString::number(i+4)+"."+Si, Bac_block.Bac_block[j].U1kDN[i]);
            WDFunc::LENumber(this, "tune"+QString::number(i+10)+"."+Si, Bac_block.Bac_block[j].U2kDN[i]);
            WDFunc::LENumber(this, "tune"+QString::number(i+16)+"."+Si, Bac_block.Bac_block[j].PhyDN[i]);
            if (i < 5)
            {
                WDFunc::LENumber(this, "tune"+QString::number(i+22)+"."+Si, Bac_block.Bac_block[j].dU_cor[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+27)+"."+Si, Bac_block.Bac_block[j].dPhy_cor[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+32)+"."+Si, Bac_block.Bac_block[j].ddU_cor[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+37)+"."+Si, Bac_block.Bac_block[j].ddPhy_cor[i]);
            }
        }
        WDFunc::LEData(this, "DividerSN"+Si, tmps);
        Bac_block.DNFNum = tmps.toInt();
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
    WDFunc::LBLText(this, "tunepercent", tmps);
    ChA1->Bda_out.dUrms = tmps.toFloat();
}

void TuneDialogA1DN::FillBdIn()
{
    WDFunc::SetLBLText(this, "tunednu1i", QString::number(ChA1->Bda_in.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2i", QString::number(ChA1->Bda_in.Uef_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphyi", QString::number(ChA1->Bda_in.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreqi", QString::number(ChA1->Bda_in.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercenti", QString::number(ChA1->Bda_in.dUrms, 'f', 5));
}

void TuneDialogA1DN::FillBackBdIn()
{
    QString tmps;
    WDFunc::LBLText(this, "tunednu1i", tmps);
    ChA1->Bda_in.Uef_filt[0] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednu2i", tmps);
    ChA1->Bda_in.Uef_filt[1] = tmps.toFloat();
    WDFunc::LBLText(this, "tunednphyi", tmps);
    ChA1->Bda_in.Phy = tmps.toFloat();
    WDFunc::LBLText(this, "tunednfreqi", tmps);
    ChA1->Bda_in.Frequency = tmps.toFloat();
    WDFunc::LBLText(this, "tunepercenti", tmps);
    ChA1->Bda_in.dUrms = tmps.toFloat();
}

void TuneDialogA1DN::FillMedian(int index)
{
    if (index < TUNEA1LEVELS)
    {
        WDFunc::SetLBLText(this, "tunedurmsm", QString::number(Dd_Block[index].dUrms, 'f', 5));
        WDFunc::SetLBLText(this, "tunephym", QString::number(Dd_Block[index].Phy, 'f', 5));
        WDFunc::SetLBLText(this, "tunesurms", QString::number(Dd_Block[index].sU, 'f', 5));
        WDFunc::SetLBLText(this, "tunesphy", QString::number(Dd_Block[index].sPhy, 'f', 5));
    }
}

void TuneDialogA1DN::SetDefCoefs()
{
    for (int j=0; j<TUNEVARIANTSNUM; ++j)
    {
        Bac_block.Bac_block[j].U1kDN[0] = 0;
        Bac_block.Bac_block[j].U2kDN[0] = 0;
        Bac_block.Bac_block[j].U1kDN[1] = Bac_block.Bac_block[j].U2kDN[1] = 12;
        Bac_block.Bac_block[j].U1kDN[2] = Bac_block.Bac_block[j].U2kDN[2] = 30;
        Bac_block.Bac_block[j].U1kDN[3] = Bac_block.Bac_block[j].U2kDN[3] = 48;
        Bac_block.Bac_block[j].U1kDN[4] = Bac_block.Bac_block[j].U2kDN[4] = 60;
        Bac_block.Bac_block[j].U1kDN[5] = Bac_block.Bac_block[j].U2kDN[5] = 71;
        for (int i=0; i<5; ++i)
        {
            Bac_block.Bac_block[j].PhyDN[i] = 0;
            Bac_block.Bac_block[j].dU_cor[i] = 0;
            Bac_block.Bac_block[j].dPhy_cor[i] = 0;
            Bac_block.Bac_block[j].ddU_cor[i] = 0;
            Bac_block.Bac_block[j].ddPhy_cor[i] = 0;
        }
        if (j == 0)
            Bac_block.Bac_block[j].K_DN = 1100;
        else if (j == 1)
            Bac_block.Bac_block[j].K_DN = 2200;
        else
            Bac_block.Bac_block[j].K_DN = 3300;
    }
    Bac_block.DNFNum = 0;
    FillBac();
}

void TuneDialogA1DN::AcceptDNData()
{
    WDFunc::LENumber(this, "kdnle", Bac_block.Bac_block[TuneVariant].K_DN);
    WDFunc::LENumber(this, "dnfnumle", Bac_block.DNFNum);
    if (Commands::WriteBac(BT_MEZONIN, &Bac_block, sizeof(Bac)) != NOERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при записи коэффициентов");
        pc.Cancelled = true;
    }
    else
    {
        EMessageBox::information(this, "Успешно", "Записано успешно!");
        FillBac();
        Accepted = true;
    }
    emit DNDataIsSet();
}

int TuneDialogA1DN::Start7_2_2()
{
    InputTuneVariant(TUNEVARIANTSNUM);
    if (pc.Cancelled)
        return GENERALERROR;
    if (Commands::SetUsingVariant(TuneVariant+1) != NOERROR)
        return GENERALERROR;
    WDFunc::SetLBLText(this, "tune00", QString::number(TuneVariant+1));
    if (Commands::GetFile(1,&S2Config) == NOERROR)
    {
        Bac_block.Bac_block[TuneVariant].U1kDN[0] = 0;
        Bac_block.Bac_block[TuneVariant].U2kDN[0] = 0;
        Bac_block.Bac_block[TuneVariant].PhyDN[0] = 0;
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
    float VoltageInkV = static_cast<float>(Bac_block.Bac_block[TuneVariant].K_DN) * Percents[counter] / 1732;
    float VoltageInV = 57.735 * Percents[counter] / 100;
    if (EMessageBox::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                              QString::number(VoltageInkV, 'f', 1) + " кВ (" + QString::number(VoltageInV, 'f', 3) + " В)") == false)
    {
        pc.Cancelled = true;
        return GENERALERROR;
    }
    WaitNSeconds(10);
    if (pc.Cancelled)
        return GENERALERROR;
    // проверка, что установлены напряжения правильно
    if (StartMeasurement() != NOERROR)
        return GENERALERROR;
    // накопление измерений
    CheckA1::A1_Bd1 tmpst, tmpst2;
    tmpst2.Phy = tmpst2.Uef_filt[0] = tmpst2.Uef_filt[1] = 0;
    int count = 0;
    emit StartPercents(pc.PovNumPoints);
    while ((count < pc.PovNumPoints) && !pc.Cancelled)
    {
        if (Commands::GetBd(A1_BDA_IN_BN, &tmpst, sizeof(CheckA1::A1_Bd1)) == NOERROR)
            FillBdIn();
        else
        {
            EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
            return GENERALERROR;
        }
        tmpst2.Phy += tmpst.Phy;
        tmpst2.Uef_filt[0] += tmpst.Uef_filt[0];
        tmpst2.Uef_filt[1] += tmpst.Uef_filt[1];
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    if (pc.Cancelled)
        return GENERALERROR;
    // усреднение
    tmpst2.Phy /= count;
    tmpst2.Uef_filt[0] /= count;
    tmpst2.Uef_filt[1] /= count;
    // теперь в ChA1->Bda_block лежат нужные нам значения
    Bac_block.Bac_block[TuneVariant].U1kDN[counter+1] = tmpst2.Uef_filt[0];
    Bac_block.Bac_block[TuneVariant].U2kDN[counter+1] = tmpst2.Uef_filt[1];
    Bac_block.Bac_block[TuneVariant].PhyDN[counter+1] = tmpst2.Phy;
    FillBac();
    return NOERROR;
}

int TuneDialogA1DN::Start7_2_67()
{
    if (!WriteTuneCoefs())
        return GENERALERROR;
    return NOERROR;
/*    if (cn->result == NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Записано успешно!");
        return NOERROR;
    }
    return GENERALERROR; */
}

int TuneDialogA1DN::Start7_2_8()
{
    if (EMessageBox::question(this, "Подтверждение", "Теперь необходимо подтвердить погрешности установки") == false)
    {
        pc.Cancelled = true;
        return GENERALERROR;
    }
    QPushButton *pb = this->findChild<QPushButton *>("GoodDN");
    if (pb != 0)
        pb->setText("Запомнить погрешность");
    return Start7_2_9(0);
}

int TuneDialogA1DN::Start7_2_9_1()
{
    return Start7_2_9(1);
}

int TuneDialogA1DN::Start7_2_9_2()
{
    return Start7_2_9(2);
}

int TuneDialogA1DN::Start7_2_9_3()
{
    return Start7_2_9(3);
}

int TuneDialogA1DN::Start7_2_9_4()
{
    return Start7_2_9(4);
}

int TuneDialogA1DN::Start7_2_9_5()
{
    return Start7_2_9(5);
}

int TuneDialogA1DN::Start7_2_9_6()
{
    return Start7_2_9(6);
}

int TuneDialogA1DN::Start7_2_9_7()
{
    if (Start7_2_9(7) != NOERROR)
        return GENERALERROR;
    // теперь считаем средние погрешности и СКО
    Bac_block.Bac_block[TuneVariant].dPhy_cor[0] = Dd_Block[0].Phy;
    Bac_block.Bac_block[TuneVariant].dU_cor[0] = Dd_Block[0].dUrms;
    Bac_block.Bac_block[TuneVariant].ddPhy_cor[0] = Dd_Block[0].sPhy;
    Bac_block.Bac_block[TuneVariant].ddU_cor[0] = Dd_Block[0].sU;
    Bac_block.Bac_block[TuneVariant].dPhy_cor[4] = Dd_Block[4].Phy;
    Bac_block.Bac_block[TuneVariant].dU_cor[4] = Dd_Block[4].dUrms;
    Bac_block.Bac_block[TuneVariant].ddPhy_cor[4] = Dd_Block[4].sPhy;
    Bac_block.Bac_block[TuneVariant].ddU_cor[4] = Dd_Block[4].sU;
    for (int i=1; i<4; ++i)
    {
        Bac_block.Bac_block[TuneVariant].dPhy_cor[i] = (Dd_Block[i].Phy + Dd_Block[TUNEA1LEVELS-i].Phy) / 2;
        Bac_block.Bac_block[TuneVariant].dU_cor[i] = (Dd_Block[i].dUrms + Dd_Block[TUNEA1LEVELS-i].dUrms) / 2;
        Bac_block.Bac_block[TuneVariant].ddPhy_cor[i] = (Dd_Block[i].sPhy + Dd_Block[TUNEA1LEVELS-i].sPhy) / 2;
        Bac_block.Bac_block[TuneVariant].ddU_cor[i] = (Dd_Block[i].sU + Dd_Block[TUNEA1LEVELS-i].sU) / 2;
    }
    FillBac();
    SaveToFile();
    return Start7_2_67();
}

int TuneDialogA1DN::Start7_2_9(int counter)
{
    const int Percents[] = {120, 100, 80, 50, 20, 50, 80, 100};
    if (counter > 7)
        return GENERALERROR;
    float VoltageInkV = static_cast<float>(Bac_block.Bac_block[TuneVariant].K_DN) * Percents[counter] / 1732;
    float VoltageInV = 57.735 * Percents[counter] / 100;
    if (EMessageBox::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                              QString::number(VoltageInkV, 'f', 1) + " кВ (" + QString::number(VoltageInV, 'f', 3) + " В)") == false)
    {
        pc.Cancelled = true;
        return GENERALERROR;
    }
    WaitNSeconds(10);
    if (StartMeasurement() != NOERROR)
        return GENERALERROR;
    FillBackBdOut();

    // накопление измерений
    DdStruct tmpst2;
    tmpst2.dUrms = tmpst2.Phy = tmpst2.sPhy = tmpst2.sU = 0;
    QList<float> sPhy, sU;
    int count = 0;
    emit StartPercents(pc.PovNumPoints);
    while ((count < pc.PovNumPoints) && !pc.Cancelled)
    {
        if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == NOERROR)
            FillBdOut();
        else
        {
            EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_out");
            return GENERALERROR;
        }
        tmpst2.dUrms += ChA1->Bda_out.dUrms;
        tmpst2.Phy += ChA1->Bda_out.Phy;
        sU.append(ChA1->Bda_out.dUrms);
        sPhy.append(ChA1->Bda_out.Phy);
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    if (pc.Cancelled)
        return GENERALERROR;
    // усреднение
    float Um = tmpst2.dUrms / count; // среднее значение погрешности по напряжению
    float Phym = tmpst2.Phy / count; // среднее значение погрешности по углу
    Dd_Block[counter].dUrms = Um;
    Dd_Block[counter].Phy = Phym;
    float sUo, sPhyo; // временные накопительные СКО
    sUo = sPhyo = 0;
    for (int i=0; i<count; ++i)
    {
        if ((i < sPhy.size()) && (i < sU.size()))
        {
            sUo += qPow((sU.at(i) - Um), 2);
            sPhyo += qPow((sPhy.at(i) - Phym), 2);
        }
    }
    sUo = qSqrt(sUo/count);
    sPhyo = qSqrt(sPhyo/count);
    Dd_Block[counter].sPhy = sPhyo;
    Dd_Block[counter].sU = sUo;
    FillMedian(counter);
    return NOERROR;
}

int TuneDialogA1DN::ReadAnalogMeasurements()
{
    // получение текущих аналоговых сигналов от модуля
    if (Commands::GetBda(BT_BASE, &ChA1->Bda_block, sizeof(CheckA1::Bda)) != NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda");
        return GENERALERROR;
    }
    ChA1->FillBda(this);
    return NOERROR;
}

int TuneDialogA1DN::ShowScheme()
{
    if (EMessageBox::question(this, "Подтверждение", "Подключите выход своего делителя напряжения ко входу U1 прибора\n"
                              "Выход эталонного делителя - ко входу U2") == false)
    {
        pc.Cancelled = true;
        return GENERALERROR;
    }
    return NOERROR;
}

void TuneDialogA1DN::GetBdAndFillMTT()
{
    if (Commands::GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == NOERROR)
        FillBdOut();
    if (Commands::GetBd(A1_BDA_IN_BN, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1)) == NOERROR)
        FillBdIn();
}
