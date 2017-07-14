#include <QCoreApplication>
#include <QtMath>
#include <QTime>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include "checkdialoga1.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"
#include "../config/config.h"
#include "../canal.h"

CheckDialogA1::CheckDialogA1(QWidget *parent) : AbstractCheckDialog(parent)
{
    BdNum = 8;
    SetupUI();
}

QWidget *CheckDialogA1::AutoCheckUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;

    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl=new QLabel("Проверка каналов измерения...");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("ainl");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl);
    lyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl=new QLabel("Проверка настроечных параметров...");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("asetl");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl);
    lyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl=new QLabel("Проверка контактов IP...");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("aipl");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl);
    lyout->addLayout(hlyout);
    hlyout=new QHBoxLayout;
    lbl=new QLabel("Проверка приёма сигнала 1PPS...");
    hlyout->addWidget(lbl);
    lbl=new QLabel("");
    lbl->setObjectName("appsl");
    lbl->setStyleSheet(ValuesFormat);
    hlyout->addWidget(lbl);
    lyout->addLayout(hlyout);
    lyout->addStretch(99);
    w->setLayout(lyout);
    return w;
}

QWidget *CheckDialogA1::BdUI(int bdnum)
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    int i;
    switch (bdnum)
    {
    case 0: // Блок #1
    {
        hlyout->addWidget(WDFunc::NewLBL(this, "0.Температура:"));
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "value0", ValuesFormat, ""));
        hlyout->addWidget(WDFunc::NewLBL(this, "1.Батарея:"));
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "value1", ValuesFormat, ""));
        hlyout->addWidget(WDFunc::NewLBL(this, "2.Частота:"));
        hlyout->addWidget(WDFunc::NewLBLT(this, "", "value2", ValuesFormat, ""));
        lyout->addLayout(hlyout);
        for (i = 0; i < 6; ++i)
        {
            QString IndexStr = "[" + QString::number(i) + "]";
            glyout->addWidget(WDFunc::NewLBL(this, "IUNF"+IndexStr),0,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+3), ValuesFormat, \
                                              QString::number(i+3)+".Истинные действующие значения сигналов"),1,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "IUF"+IndexStr),2,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+9), ValuesFormat, \
                                              QString::number(i+9)+".Действующие значения сигналов по 1-й гармонике\nотносительно ф. А 1-й группы"),3,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "PHF"+IndexStr),4,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+15), ValuesFormat, \
                                              QString::number(i+15)+".Угол сдвига между сигналами по первой гармонике\nотносительно ф. А 1-й группы"),5,i,1,1);
        }
        for (i = 0; i < 3; ++i)
        {
            QString IndexStr = "[" + QString::number(i) + "]";
            glyout->addWidget(WDFunc::NewLBL(this, "PNF"+IndexStr),6,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+21), ValuesFormat, \
                                              QString::number(i+21)+".Истинная активная мощность"),7,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "SNF"+IndexStr),6,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+24), ValuesFormat, \
                                              QString::number(i+24)+".Кажущаяся полная мощность"),7,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "QNF"+IndexStr),8,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+27), ValuesFormat, \
                                              QString::number(i+27)+".Реактивная мощность"),9,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "Cos"+IndexStr),8,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+30), ValuesFormat, \
                                              QString::number(i+30)+".Cos phi по истинной активной мощности"),9,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "PF"+IndexStr),10,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+33), ValuesFormat, \
                                              QString::number(i+33)+".Активная мощность по 1-й гармонике"),11,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "QF"+IndexStr),10,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+36), ValuesFormat, \
                                              QString::number(i+36)+".Реактивная мощность по 1-й гармонике"),11,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "SF"+IndexStr),12,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+39), ValuesFormat, \
                                              QString::number(i+39)+".Полная мощность по 1-й гармонике"),13,i,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "CosPhi"+IndexStr),12,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+42), ValuesFormat, \
                                              QString::number(i+42)+".Cos phi по 1-й гармонике"),13,i+3,1,1);
            glyout->addWidget(WDFunc::NewLBL(this, "PHI"+IndexStr),14,i,1,1);
            glyout->addWidget(WDFunc::NewLBLT(this, "", "value"+QString::number(i+45), ValuesFormat, \
                                              QString::number(i+45)+".Угол между током и напряжением"),15,i,1,1);
        }
        lyout->addLayout(glyout);
        break;
    }
    case 1:
    {

    }
    }
    w->setLayout(lyout);
    return w;
}

void CheckDialogA1::RefreshAnalogValues(int bdnum)
{
/*    QLabel *lbl;
    WDFunc::SetLBLText(this, "value0", QString::number(Bda_block.Tmk, 'f', 1));
    WDFunc::SetLBLText(this, "value1", QString::number(Bda_block.Vbat, 'f', 1));
    WDFunc::SetLBLText(this, "value2", QString::number(Bda_block.Frequency, 'f', 1));
    for (int i = 0; i < 3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+3));
        if (lbl == 0)
            return;
        int Precision = (pc.ModuleBsi.MTypeB != MTE_2T0N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+6));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUefNat_filt[i+3], 'f', Precision));

        lbl = this->findChild<QLabel *>("value"+QString::number(i+9));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_2T0N) ? 3 : 4;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+12));
        if (lbl == 0)
            return;
        Precision = (pc.ModuleBsi.MType1 != MTE_0T2N) ? 4 : 3;
        lbl->setText(QString::number(Bda_block.IUeff_filtered[i+3], 'f', Precision));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+15));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.phi_next_f[i], 'f', 3));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+21));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.PNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+24));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.SNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+27));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.QNatf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+30));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhiNat[i], 'f', 4));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+33));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Pf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+36));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Qf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+39));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.Sf[i], 'f', 3));
        lbl = this->findChild<QLabel *>("value"+QString::number(i+42));
        if (lbl == 0)
            return;
        lbl->setText(QString::number(Bda_block.CosPhi[i], 'f', 4));
    }
    for (int i=0; i<3; i++)
    {
        lbl = this->findChild<QLabel *>("value"+QString::number(i+45));
        if (lbl == 0)
            return;
        float PHI = (180*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        lbl->setText(QString::number(PHI, 'f', 3));
    } */
}

void CheckDialogA1::PrepareHeadersForFile(int row)
{
    for (int i=0; i<3; i++)
    {
        if (pc.ModuleBsi.MTypeM != MTM_81)
            xlsx->write(row,i+2,QVariant(("U1 ф")+QString::number(i+10, 36)+", В"));
        else
            xlsx->write(row,i+2,QVariant("I1 ф"+QString::number(i+10, 36)+", А"));
        if (pc.ModuleBsi.MTypeM != MTM_83)
            xlsx->write(row,i+5,QVariant("I2 ф"+QString::number(i+10, 36)+", А"));
        else
            xlsx->write(row,i+5,QVariant("U2 ф"+QString::number(i+10, 36)+", В"));
        xlsx->write(row,i+8,QVariant("Phi ф"+QString::number(i+10, 36)+", град"));
        xlsx->write(row,i+11,QVariant("Pf ф"+QString::number(i+10, 36)+", Вт"));
        xlsx->write(row,i+14,QVariant("Qf ф"+QString::number(i+10, 36)+", ВА"));
        xlsx->write(row,i+17,QVariant("Sf ф"+QString::number(i+10, 36)+", ВА"));
    }
    xlsx->write(row,20,QVariant("f, Гц"));
    xlsx->write(row,21,QVariant("t, град"));
}

void CheckDialogA1::WriteToFile(int row, int bdnum)
{
/*    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    for (int i=0; i<3; i++)
    {
        QString Precision = (pc.ModuleBsi.MTypeM != MTM_81) ? "0.000" : "0.0000";
        format.setNumberFormat(Precision);
        xlsx->write(WRow,i+2,Bda_block.IUeff_filtered[i],format);

        Precision = (pc.ModuleBsi.MTypeM != MTM_83) ? "0.0000" : "0.000";
        format.setNumberFormat(Precision);
        xlsx->write(WRow,i+5,Bda_block.IUeff_filtered[i+3],format);

        format.setNumberFormat("0.000");
        float Phi = (static_cast<float>(180)*qAsin(Bda_block.Qf[i]/Bda_block.Sf[i])/M_PI);
        xlsx->write(WRow,i+8,Phi,format);
        xlsx->write(WRow,i+11,Bda_block.Pf[i],format);
        xlsx->write(WRow,i+14,Bda_block.Qf[i],format);
        xlsx->write(WRow,i+17,Bda_block.Sf[i],format);
    }
    format.setNumberFormat("0.0000");
    xlsx->write(WRow,20,Bda_block.Frequency,format);
    format.setNumberFormat("0.0");
    xlsx->write(WRow,21,Bda_block.Tmk,format); */
}
