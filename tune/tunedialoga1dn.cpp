#include <QTime>
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
#include "tunedialoga1dn.h"
#include "../dialogs/keypressdialog.h"
#include "../widgets/emessagebox.h"
#include "../gen/error.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../gen/modulebsi.h"
#include "../widgets/wd_func.h"
#include "../gen/maindef.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif

TuneDialogA1DN::TuneDialogA1DN(QWidget *parent) :
    EAbstractTuneDialogA1DN(parent)
{
    CA1 = new ConfigA1(S2Config);
    ChA1 = new CheckA1;
    SetBac(&Bac_block, 2, sizeof(Bac));
    SetBac(&Bac_block3, 3, sizeof(Bac3));
    RepModel = new ReportModel;
    SetupUI();
    LoadSettings();
}

TuneDialogA1DN::~TuneDialogA1DN()
{
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialogA1DN::SetLbls()
{
    lbls.append("7.2.2. Ввод пароля...");
    lbls.append("7.2.1. Отображение диалога проверки схемы подключения...");
    lbls.append("7.2.3. Ввод данных по делителю и приём настроечных параметров...");
    lbls.append("7.2.5. Задание варианта включения ДН...");
    lbls.append("7.2.6. Запись варианта включения в прибор...");
    lbls.append("7.2.7. Установка 20%, проверка и сохранение...");
    lbls.append("7.2.7. Установка 50%, проверка и сохранение...");
    lbls.append("7.2.7. Установка 80%, проверка и сохранение...");
    lbls.append("7.2.7. Установка 100%, проверка и сохранение...");
    lbls.append("7.2.7. Установка 120%, проверка и сохранение...");
    lbls.append("7.2.11. Запись настроечных коэффициентов и переход на новую конфигурацию...");
    lbls.append("7.2.12. Проверка аналоговых данных...");
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
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_3); // 7.2.3. Ввод данных по делителю
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_5);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_6);
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_7_1); // 7.2.7. Установка 20%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_7_2); // 7.2.7. Установка 50%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_7_3); // 7.2.7. Установка 80%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_7_4); // 7.2.7. Установка 100%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_7_5); // 7.2.7. Установка 120%, проверка и сохранение
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_11); // 7.2.11. Запись настроечных коэффициентов и переход на новую конфигурацию
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_12); // 7.2.12. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;



    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_1); // 7.2.9.1. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_2); // 7.2.9.2. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_3); // 7.2.9.3. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_4); // 7.2.9.4. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_5); // 7.2.9.5. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_6); // 7.2.9.6. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
    func = reinterpret_cast<int ((EAbstractTuneDialog::*)())>(&TuneDialogA1DN::Start7_2_13_7); // 7.2.9.7. Проверка аналоговых данных
    pf[lbls.at(count++)] = func;
}
#endif

void TuneDialogA1DN::SetupUI()
{
    QWidget *cp1 = new QWidget;
    QWidget *cp2 = CoefUI(0);
    QWidget *cp3 = CoefUI(1);
    QWidget *cp4 = CoefUI(2);
    QWidget *cp5 = CoefUI3(0);
    QWidget *cp6 = CoefUI3(1);
    QWidget *cp7 = CoefUI3(2);
    QWidget *cp8 = new QWidget;
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
    TuneTW->addTab(cp2,"Коэф~ 1");
    TuneTW->addTab(cp3,"Коэф~ 2");
    TuneTW->addTab(cp4,"Коэф~ 3");
    TuneTW->addTab(cp5,"Коэф= 1");
    TuneTW->addTab(cp6,"Коэф= 2");
    TuneTW->addTab(cp7,"Коэф= 3");
    TuneTW->addTab(cp8,"Данные измерений");

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
    QPushButton *pb = new QPushButton("Продолжить");
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

    // CP8 - Данные измерений

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
    cp8->setLayout(lyout);

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
    hlyout->addWidget(WDFunc::NewSPB(this,"K_DNSPB."+Si,1,10000,0,UCONFWCLR));
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
    lyout->addWidget(BottomUI(1)); // блок Bac2 - первый по счёту в EAbstractTuneDialog
    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

void TuneDialogA1DN::FillBac(int bacnum)
{
    if (bacnum == BLOCK_ALTERNATIVE)
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
    else
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            QString Si = QString::number(j+3);
            WDFunc::SetSPBData(this, "K_DNSPB."+Si, Bac_block3.Bac_block3[j].K_DN);
            for (int i = 0; i < 6; ++i)
            {
                WDFunc::SetLEData(this, "tune"+QString::number(i+4)+"."+Si, QString::number(Bac_block3.Bac_block3[j].U1kDN[i], 'f', 5));
                WDFunc::SetLEData(this, "tune"+QString::number(i+10)+"."+Si, QString::number(Bac_block3.Bac_block3[j].U2kDN[i], 'f', 5));
                if (i < 5)
                {
                    WDFunc::SetLEData(this, "tune"+QString::number(i+22)+"."+Si, QString::number(Bac_block3.Bac_block3[j].dU_cor[i], 'f', 5));
                    WDFunc::SetLEData(this, "tune"+QString::number(i+32)+"."+Si, QString::number(Bac_block3.Bac_block3[j].ddU_cor[i], 'f', 5));
                }
            }
            WDFunc::SetLEData(this, "DividerSN"+Si, QString::number(Bac_block3.DNFNum));
        }
    }
}

void TuneDialogA1DN::FillBackBac(int bacnum)
{
    QString tmps;
    if (bacnum == BLOCK_ALTERNATIVE)
    {
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
    else
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            QString Si = QString::number(j+3);
            WDFunc::SPBData(this, "K_DNSPB."+Si, Bac_block3.Bac_block3[j].K_DN);
            for (int i = 0; i < 6; i++)
            {
                WDFunc::LENumber(this, "tune"+QString::number(i+4)+"."+Si, Bac_block3.Bac_block3[j].U1kDN[i]);
                WDFunc::LENumber(this, "tune"+QString::number(i+10)+"."+Si, Bac_block3.Bac_block3[j].U2kDN[i]);
                if (i < 5)
                {
                    WDFunc::LENumber(this, "tune"+QString::number(i+22)+"."+Si, Bac_block3.Bac_block3[j].dU_cor[i]);
                    WDFunc::LENumber(this, "tune"+QString::number(i+32)+"."+Si, Bac_block3.Bac_block3[j].ddU_cor[i]);
                }
            }
            WDFunc::LEData(this, "DividerSN"+Si, tmps);
            Bac_block3.DNFNum = tmps.toInt();
        }
    }
}

void TuneDialogA1DN::SetDefCoefs(int bacnum)
{
    if (bacnum == BLOCK_ALTERNATIVE)
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
        }
        Bac_block.Bac_block[0].K_DN = 350;
        Bac_block.Bac_block[1].K_DN = 1100;
        Bac_block.Bac_block[2].K_DN = 2200;
        Bac_block.DNFNum = 1;
    }
    else
    {
        for (int j=0; j<TUNEVARIANTSNUM; ++j)
        {
            Bac_block3.Bac_block3[j].U1kDN[0] = 0;
            Bac_block3.Bac_block3[j].U2kDN[0] = 0;
            Bac_block3.Bac_block3[j].U1kDN[1] = Bac_block3.Bac_block3[j].U2kDN[1] = 16;
            Bac_block3.Bac_block3[j].U1kDN[2] = Bac_block3.Bac_block3[j].U2kDN[2] = 40;
            Bac_block3.Bac_block3[j].U1kDN[3] = Bac_block3.Bac_block3[j].U2kDN[3] = 64;
            Bac_block3.Bac_block3[j].U1kDN[4] = Bac_block3.Bac_block3[j].U2kDN[4] = 80;
            Bac_block3.Bac_block3[j].U1kDN[5] = Bac_block3.Bac_block3[j].U2kDN[5] = 100;
            for (int i=0; i<5; ++i)
            {
                Bac_block3.Bac_block3[j].dU_cor[i] = 0;
                Bac_block3.Bac_block3[j].ddU_cor[i] = 0;
            }
        }
        Bac_block3.Bac_block3[0].K_DN = 350;
        Bac_block3.Bac_block3[1].K_DN = 1100;
        Bac_block3.Bac_block3[2].K_DN = 2200;
        Bac_block3.DNFNum = 1;
    }
    FillBac(bacnum);
}

QWidget *TuneDialogA1DN::CoefUI3(int bac3num)
{
    QString Si = QString::number(bac3num+3); // для различия настроечных коэффициентов по Bac_block даём смещение +3
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
    hlyout->addWidget(WDFunc::NewSPB(this,"K_DNSPB."+Si,1,10000,0,UCONFWCLR));
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
        if (i < 5)
        {
            glyout->addWidget(WDFunc::NewLBL(this, "δU["+QString::number(i)+"]"),6,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+22)+"."+Si, "", ValuesLEFormat),7,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "σU["+QString::number(i)+"]","","","СКО амплитудной погрешности"),10,i,1,1);
            glyout->addWidget(WDFunc::NewLE(this, "tune"+QString::number(i+32)+"."+Si, "", ValuesLEFormat),11,i,1,1);
        }
    }
    lyout->addLayout(glyout);
    lyout->addWidget(BottomUI(2));
    lyout->addStretch(1);
    w->setLayout(lyout);
    return w;
}

#if PROGSIZE != PROGSIZE_EMUL
void TuneDialogA1DN::AcceptDNData()
{
    WDFunc::CBIndex(this, "dnutype", Mode);
    if (Mode > 0)
        Commands::SetMode(Mode); // устанавливаем род напряжения (0 - переменный, 1 - постоянный)
    if (Mode == MODE_ALTERNATIVE)
        WDFunc::LENumber(this, "dnfnumle", Bac_block.DNFNum);
    else
        WDFunc::LENumber(this, "dnfnumle", Bac_block3.DNFNum);
    WriteBacBlock();
    emit Finished();
}

int TuneDialogA1DN::Start7_2_3()
{
    Commands::GetBac(2, &Bac_block, sizeof(Bac));
    Commands::GetBac(3, &Bac_block3, sizeof(Bac3)); // 7.2.4
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Данные на ТН(ДН)"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер ТН(ДН)"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "dnfnumle", QString::number(Bac_block.DNFNum)), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Род напряжения"), row, 0, 1, 1, Qt::AlignRight);
    QStringList sl = QStringList() << "Переменный" << "Постоянный";
    glyout->addWidget(WDFunc::NewCB(this, "dnutype", sl), row++, 1, 1, 1, Qt::AlignLeft);

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
    connect(this,SIGNAL(Finished()),dlg,SLOT(close()));
    Accepted = false;
    dlg->show();
    while (!Accepted && !StdFunc::IsCancelled())
        TimeFunc::Wait();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
}

int TuneDialogA1DN::Start7_2_5()
{
    InputTuneVariant(TUNEVARIANTSNUM);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    WriteBacBlock();
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
/*    if (Commands::GetFile(1,&S2Config) == Error::ER_NOERROR)
    { */
        Bac_block.Bac_block[TuneVariant].U1kDN[0] = 0;
        Bac_block.Bac_block[TuneVariant].U2kDN[0] = 0;
        Bac_block.Bac_block[TuneVariant].PhyDN[0] = 0;
        Bac_block3.Bac_block3[TuneVariant].U1kDN[0] = 0;
        Bac_block3.Bac_block3[TuneVariant].U2kDN[0] = 0;
/*        return Error::ER_NOERROR;
    }
    return Error::ER_GENERALERROR; */
    return Error::ER_NOERROR;
}

int TuneDialogA1DN::Start7_2_6()
{
    if (Commands::SetUsingVariant(TuneVariant+1) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    WDFunc::SetLBLText(this, "tune00", QString::number(TuneVariant+1));
    return Error::ER_NOERROR;
}

int TuneDialogA1DN::Start7_2_7_1()
{
    QPushButton *pb = this->findChild<QPushButton *>("GoodDN");
    if (pb != nullptr)
        pb->setText("Продолжить");
    return Start7_2_78910(0);
}

int TuneDialogA1DN::Start7_2_7_2()
{
    return Start7_2_78910(1);
}

int TuneDialogA1DN::Start7_2_7_3()
{
    return Start7_2_78910(2);
}

int TuneDialogA1DN::Start7_2_7_4()
{
    return Start7_2_78910(3);
}

int TuneDialogA1DN::Start7_2_7_5()
{
    return Start7_2_78910(4);
}

int TuneDialogA1DN::Start7_2_78910(int counter)
{
    CheckA1::A1_Bd1 tmpst2;
    const int Percents[] = {20,50,80,100,120};

    if (counter > 4)
        return Error::ER_GENERALERROR;
    if (GetAndAverage(Percents[counter], GAAT_BDA_IN, &tmpst2) == Error::ER_GENERALERROR)
    {
        EMessageBox::error(this, "Ошибка", "Ошибка при проведении процедуры усреднения");
        return Error::ER_GENERALERROR;
    }
/*    float VoltageInV = (Mode == MODE_ALTERNATIVE) ? (100 / qSqrt(3)) : (100 * qSqrt(2) / qSqrt(3));
    VoltageInV *= Percents[counter] / 100;
    float VoltageInkV = (Mode == MODE_ALTERNATIVE) ? Bac_block.Bac_block[TuneVariant].K_DN : Bac_block3.Bac_block3[TuneVariant].K_DN;
    VoltageInkV *= VoltageInV / 1000;
    if (EMessageBox::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                              QString::number(VoltageInkV, 'f', 1) + " кВ (" + QString::number(VoltageInV, 'f', 3) + " В) \n"
                              "и нажмите кнопку \"Годится\", когда напряжения установятся") == false)
    {
        StdFunc::Cancel();
        return Error::ER_GENERALERROR;
    }
//    WaitNSeconds(WAITFORCONST);
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    // проверка, что установлены напряжения правильно
    if (StartMeasurement() != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    // накопление измерений
    CheckA1::A1_Bd1 tmpst, tmpst2;
    tmpst2.Phy = tmpst2.Uef_filt[0] = tmpst2.Uef_filt[1] = 0;
    int count = 0;
    emit StartPercents(PovNumPoints);
    while ((count < PovNumPoints) && !StdFunc::IsCancelled())
    {
        if (Commands::GetBd(A1_BDA_IN_BN, &tmpst, sizeof(CheckA1::A1_Bd1)) == Error::ER_NOERROR)
            FillBdIn();
        else
        {
            EMessageBox::information(this, "Внимание", "Ошибка при приёме блока Bda_in");
            return Error::ER_GENERALERROR;
        }
        tmpst2.Phy += tmpst.Phy;
        tmpst2.UefNat_filt[0] += tmpst.UefNat_filt[0];
        tmpst2.UefNat_filt[1] += tmpst.UefNat_filt[1];
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    if (StdFunc::IsCancelled())
        return Error::ER_GENERALERROR;
    // усреднение
    tmpst2.Phy /= count;
    tmpst2.Uef_filt[0] /= count;
    tmpst2.Uef_filt[1] /= count; */

    // теперь в tmpst2 лежат нужные нам значения
    if (Mode == MODE_ALTERNATIVE)
    {
        Bac_block.Bac_block[TuneVariant].U1kDN[counter+1] = tmpst2.UefNat_filt[0];
        Bac_block.Bac_block[TuneVariant].U2kDN[counter+1] = tmpst2.UefNat_filt[1];
        Bac_block.Bac_block[TuneVariant].PhyDN[counter+1] = tmpst2.Phy;
        FillBac(BLOCK_ALTERNATIVE);
    }
    else
    {
        Bac_block3.Bac_block3[TuneVariant].U1kDN[counter+1] = tmpst2.UefNat_filt[0];
        Bac_block3.Bac_block3[TuneVariant].U2kDN[counter+1] = tmpst2.UefNat_filt[1];
        FillBac(BLOCK_DIRECT);
    }
    return Error::ER_NOERROR;
}

int TuneDialogA1DN::Start7_2_11()
{
    if (!WriteTuneCoefs(Mode + 1)) // MODE = BLOCK + 1
        return Error::ER_GENERALERROR;
    return Error::ER_NOERROR;
/*    if (cn->result == Error::NOERROR)
    {
        EMessageBox::information(this, "Внимание", "Записано успешно!");
        return Error::NOERROR;
    }
    return Error::GENERALERROR; */
}

int TuneDialogA1DN::Start7_2_12()
{
    if (EMessageBox::question(this, "Подтверждение", "Теперь необходимо подтвердить погрешности установки") == false)
    {
        StdFunc::Cancel();
        return Error::ER_GENERALERROR;
    }
    QPushButton *pb = this->findChild<QPushButton *>("GoodDN");
    if (pb != nullptr)
        pb->setText("Продолжить");
    RepModel->SetModel(GOST23625ROWCOUNT, GOST23625COLCOUNT);
    return Start7_2_13(0);
}

int TuneDialogA1DN::Start7_2_13_1()
{
    return Start7_2_13(1);
}

int TuneDialogA1DN::Start7_2_13_2()
{
    return Start7_2_13(2);
}

int TuneDialogA1DN::Start7_2_13_3()
{
    return Start7_2_13(3);
}

int TuneDialogA1DN::Start7_2_13_4()
{
    return Start7_2_13(4);
}

int TuneDialogA1DN::Start7_2_13_5()
{
    return Start7_2_13(5);
}

int TuneDialogA1DN::Start7_2_13_6()
{
    return Start7_2_13(6);
}

int TuneDialogA1DN::Start7_2_13_7()
{
    return Start7_2_13(7);
}

int TuneDialogA1DN::Start7_2_13_8()
{
    if (Start7_2_13(8) != Error::ER_NOERROR)
        return Error::ER_GENERALERROR;
    // теперь считаем средние погрешности и СКО
    if (Mode == MODE_ALTERNATIVE)
    {
        Bac_block.Bac_block[TuneVariant].dPhy_cor[0] = Dd_Block[4].Phy;
        Bac_block.Bac_block[TuneVariant].dU_cor[0] = Dd_Block[4].dUrms;
        Bac_block.Bac_block[TuneVariant].ddPhy_cor[0] = Dd_Block[4].sPhy;
        Bac_block.Bac_block[TuneVariant].ddU_cor[0] = Dd_Block[4].sU;
        for (int i=0; i<4; ++i)
        {
            Bac_block.Bac_block[TuneVariant].dPhy_cor[4-i] = (Dd_Block[i].Phy + Dd_Block[TUNEA1LEVELS-i-1].Phy) / 2;
            Bac_block.Bac_block[TuneVariant].dU_cor[4-i] = (Dd_Block[i].dUrms + Dd_Block[TUNEA1LEVELS-i-1].dUrms) / 2;
            Bac_block.Bac_block[TuneVariant].ddPhy_cor[4-i] = (Dd_Block[i].sPhy + Dd_Block[TUNEA1LEVELS-i-1].sPhy) / 2;
            Bac_block.Bac_block[TuneVariant].ddU_cor[4-i] = (Dd_Block[i].sU + Dd_Block[TUNEA1LEVELS-i-1].sU) / 2;
        }
        FillBac(BLOCK_ALTERNATIVE);
        SaveToFile(BLOCK_ALTERNATIVE);
    }
    else
    {
        Bac_block3.Bac_block3[TuneVariant].dU_cor[0] = Dd_Block[4].dUrms;
        Bac_block3.Bac_block3[TuneVariant].ddU_cor[0] = Dd_Block[4].sU;
        for (int i=0; i<4; ++i)
        {
            Bac_block3.Bac_block3[TuneVariant].dU_cor[4-i] = (Dd_Block[i].dUrms + Dd_Block[TUNEA1LEVELS-i-1].dUrms) / 2;
            Bac_block3.Bac_block3[TuneVariant].ddU_cor[4-i] = (Dd_Block[i].sU + Dd_Block[TUNEA1LEVELS-i-1].sU) / 2;
        }
        FillBac(BLOCK_DIRECT);
        SaveToFile(BLOCK_DIRECT);
    }
    return Start7_2_11();
}

int TuneDialogA1DN::Start7_2_13(int counter)
{
    const int Percents[] = {120, 100, 80, 50, 20, 50, 80, 100, 120};
    if (counter >= TUNEA1LEVELS)
        return Error::ER_GENERALERROR;
    return GetAndAverage(Percents[counter], GAAT_BDA_OUT, &Dd_Block[counter]);
}

int TuneDialogA1DN::Start7_2_14()
{
    if (!WriteTuneCoefs(Mode + 1)) // MODE = BLOCK + 1
        return Error::ER_GENERALERROR;
    FillHeaders();
    // запись файла протокола
    ReportHeader.PovDateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
    Commands::GetBd(A1_BDA_OUT_AN_BN, &ChA1->Bda_out_an, sizeof(CheckA1::A1_Bd4));
    ReportHeader.Freq = QString::number(ChA1->Bda_out.Frequency, 'g', 4);
    ReportHeader.Humidity = QString::number(ChA1->Bda_out_an.Hamb, 'g', 3);
    ReportHeader.Temp = QString::number(ChA1->Bda_out_an.Tamb, 'g', 2);
    if (DNDialog(PovDev)) // вводим данные по делителю
    {
        EMessageBox::information(this, "Отменено", "Операция отменена");
        return Error::ER_GENERALERROR;
    }
    if (ConditionDataDialog()) // задаём условия поверки
    {
        EMessageBox::information(this, "Отменено", "Операция отменена");
        return Error::ER_GENERALERROR;
    }
    GenerateReport();
    return Error::ER_NOERROR;
}

void TuneDialogA1DN::FillHeaders()
{
    QStringList sl = QStringList() << "Проц" << "S/Sном" <<  "dUrms(u)" <<  "Phy(u)";
    sl << "dUrms(d)" << "Phy(d)" << "dUrms(ud)" << "Phy(ud)" << "dUrms(md)" << "Phy(md)" << \
          "dUrms(u-d)" << "Phy(u-d)" << "dUrms" << "Phy" << "sUrms(d)" << "sPhy(d)";
    sl << "sUrms(u)" << "sPhy(u)";
    RepModel->SetHeader(sl);
}

bool TuneDialogA1DN::ConditionDataDialog()
{
    Cancelled = false;
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Условия поверки"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Дата поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDateTime", ReportHeader.PovDateTime), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Температура окружающей среды, °С"), row, 0, 1, 1, Qt::AlignRight);
    QString tmps;
    if (StdFunc::FloatInRange(ChA1->Bda_out_an.Tamb, FLT_MAX, TH1))
        tmps = "";
    else
        tmps = ReportHeader.Temp;
    glyout->addWidget(WDFunc::NewLE(this, "Temp", tmps), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Влажность воздуха, %"), row, 0, 1, 1, Qt::AlignRight);
    if (StdFunc::FloatInRange(ChA1->Bda_out_an.Hamb, FLT_MAX, TH1))
        tmps = "";
    else
        tmps = ReportHeader.Humidity;
    glyout->addWidget(WDFunc::NewLE(this, "Humidity", tmps), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Атмосферное давление, кПа"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Pressure", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания сети, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Voltage", ""), row++, 1, 1, 1, Qt::AlignLeft);
/*    glyout->addWidget(WDFunc::NewLBL(this, "Частота питания сети, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "Frequency", ReportHeader.Freq), row++, 1, 1, 1, Qt::AlignLeft); */
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetConditionData()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(CancelTune()));
    lyout->addWidget(pb);
    connect(this,SIGNAL(Finished()),dlg,SLOT(close()));
    dlg->setLayout(lyout);
    dlg->exec();
    return Cancelled;
}

bool TuneDialogA1DN::DNDialog(PovDevStruct &PovDev)
{
    Cancelled = false;
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Данные ТН(ДН)"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Организация, проводившая поверку"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "UKDNOrganization", StdFunc::OrganizationString()), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип ТН(ДН)"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNType", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Обозначение по схеме, фаза"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNNamePhase", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNSerialNum", ReportHeader.DNSerNum), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности, %"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNTolerance", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное первичное напряжение, кВ"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNU1", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNU2", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная мощность нагрузки, ВА"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNP", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная частота, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNFreq", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Предприятие-изготовитель"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNOrganization", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Место установки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNPlace", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Наименование средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDev", PovDev.DevName), row++, 1, 1, 1, Qt::AlignLeft);
    WDFunc::SetEnabled(this, "PovDev", false);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDevSN", PovDev.DevSN), row++, 1, 1, 1, Qt::AlignLeft);
    WDFunc::SetEnabled(this, "PovDevSN", false);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "PovDevPrecision", PovDev.DevPrecision), row++, 1, 1, 1, Qt::AlignLeft);
    WDFunc::SetEnabled(this, "PovDevPrecision", false);
    glyout->addWidget(WDFunc::NewLBL(this, "Результаты внешнего осмотра"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNInspection", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Результаты проверки правильности обозначения\nвыводов и групп соединений обмоток"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLE(this, "DNWindingInspection", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetDNData()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    lyout->addWidget(pb);
    connect(this,SIGNAL(CloseDialog()),dlg,SLOT(close()));
    dlg->setLayout(lyout);
    dlg->exec();
    return Cancelled;
}

int TuneDialogA1DN::ReadAnalogMeasurements()
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

int TuneDialogA1DN::ShowScheme()
{
    if (EMessageBox::question(this, "Подтверждение", "Подключите выход своего делителя напряжения ко входу U1 прибора\n"
                              "Выход эталонного делителя - ко входу U2") == false)
    {
        StdFunc::Cancel();
        return Error::ER_GENERALERROR;
    }
    return Error::ER_NOERROR;
}

void TuneDialogA1DN::WriteBacBlock()
{
    if (Mode == MODE_ALTERNATIVE)
    {
        if (Commands::WriteBac(2, &Bac_block, sizeof(Bac)) != Error::ER_NOERROR)
        {
            EMessageBox::error(this, "Ошибка", "Ошибка при записи коэффициентов");
            StdFunc::Cancel();
        }
        else
        {
            EMessageBox::information(this, "Успешно", "Записано успешно!");
            FillBac(BLOCK_ALTERNATIVE);
            Accepted = true;
        }
    }
    else
    {
        if (Commands::WriteBac(3, &Bac_block3, sizeof(Bac3)) != Error::ER_NOERROR)
        {
            EMessageBox::error(this, "Ошибка", "Ошибка при записи коэффициентов");
            StdFunc::Cancel();
        }
        else
        {
            EMessageBox::information(this, "Успешно", "Записано успешно!");
            FillBac(BLOCK_DIRECT);
            Accepted = true;
        }
    }
}

#endif

void TuneDialogA1DN::LoadSettings()
{
    QSettings *sets = new QSettings ("EvelSoft",PROGNAME);
    PovNumPoints = sets->value("PovNumPoints", "60").toInt();
}

void TuneDialogA1DN::GenerateReport()
{
    // данные в таблицу уже получены или из файла, или в процессе работы
    // отобразим таблицу
    ShowTable();
    Report *report = new Report("23625", this);
    report->AddModel("maindata", RepModel);
    // запрос блока Bda_h, чтобы выдать KNI в протокол
#if PROGSIZE != PROGSIZE_EMUL
    if (Commands::GetBd(A1_BDA_H_BN, &ChA1->Bda_h, sizeof(CheckA1::A1_Bd2)) == Error::ER_NOERROR)
        report->SetVar("KNI", ChA1->Bda_h.HarmBuf[0][0], 5);
#endif
    report->SetVar("Organization", StdFunc::OrganizationString());
    QString day = QDateTime::currentDateTime().toString("dd");
    QString month = QDateTime::currentDateTime().toString("MM");
    QString yr = QDateTime::currentDateTime().toString("yy");
    report->SetVar("Day", day);
    report->SetVar("Month", month);
    report->SetVar("Yr", yr);
    report->SetVar("DNNamePhase", ReportHeader.DNNamePhase);
    report->SetVar("DNType", ReportHeader.DNType);
    report->SetVar("DNSerNum", ReportHeader.DNSerNum);
    report->SetVar("DNTol", ReportHeader.DNTol);
    report->SetVar("DNU1", ReportHeader.DNU1);
    report->SetVar("DNU2", ReportHeader.DNU2);
    report->SetVar("DNP", ReportHeader.DNP);
    report->SetVar("DNF", ReportHeader.DNF);
    report->SetVar("DNOrganization", ReportHeader.DNOrganization);
    report->SetVar("DNPlace", ReportHeader.DNPlace);
    report->SetVar("DNDevices", ReportHeader.DNDevices);
    report->SetVar("Temp", ReportHeader.Temp);
    report->SetVar("Humidity", ReportHeader.Humidity);
    report->SetVar("Pressure", ReportHeader.Pressure);
    report->SetVar("Voltage", ReportHeader.Voltage);
    if (Mode == MODE_ALTERNATIVE)
        report->SetVar("Freq", ReportHeader.Freq);
    report->SetVar("OuterInsp", ReportHeader.OuterInsp);
    report->SetVar("WindingsInsp", ReportHeader.WindingsInsp);
    report->SetVar("PovDateTime", ReportHeader.PovDateTime);
    QString filename = Files::ChooseFileForSave(this, "*.pdf", "pdf");
    if (!filename.isEmpty())
    {
        report->Generate(filename);
        EMessageBox::information(this, "Успешно!", "Записано успешно!");
    }
    else
        EMessageBox::information(this, "Отменено", "Действие отменено");
    delete report;
}
