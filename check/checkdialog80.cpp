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
#include "checkdialog80.h"
#include "../widgets/messagebox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"
#include "../config/config.h"

CheckDialog80::CheckDialog80(QWidget *parent) : AbstractCheckDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    C80 = new Check_80;
    BdNum = 8;
    SetBd(1, &C80->Bd_block1, sizeof(Check_80::Bd1));
    SetBd(2, &C80->Bd_block2, sizeof(Check_80::Bd2));
    SetBd(3, &C80->Bd_block3, sizeof(Check_80::Bd2));
    SetBd(4, &C80->Bd_block4, sizeof(Check_80::Bd4));
    SetBd(5, &C80->Bd_block5, sizeof(Check_80::Bd4));
    SetBd(6, &C80->Bd_block6, sizeof(Check_80::Bd6));
    SetBd(7, &C80->Bd_block7, sizeof(Check_80::Bd6));
    SetBd(8, &C80->Bd_block8, sizeof(Check_80::Bd8));
    SetupUI();
}

QWidget *CheckDialog80::AutoCheckUI()
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

void CheckDialog80::SetupUI()
{

}

QWidget *CheckDialog80::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case 1: // Блок #1
        return C80->Bd_1W(this);
    case 2:
        return C80->Bd_2W(this);
    case 3:
        return C80->Bd_3W(this);
    case 4:
        return C80->Bd_4W(this);
    case 5:
    case 6:
    case 7:
    case 8:
        return C80->Bd_5W(bdnum-5, this);
    case 9:
        return C80->Bd_6W(this);
    case 10:
    case 11:
    case 12:
    case 13:
//        return C80->Bd_7W(this);

//        return C80->Bd_8W(this);
    default:
        return new QWidget;
    }
}

void CheckDialog80::RefreshAnalogValues(int bdnum)
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

void CheckDialog80::PrepareHeadersForFile(int row)
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

void CheckDialog80::WriteToFile(int row, int bdnum)
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

void CheckDialog80::ChooseValuesToWrite()
{

}

void CheckDialog80::SetDefaultValuesToWrite()
{

}
