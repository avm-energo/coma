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
    BdNum = 5;
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
    switch (bdnum)
    {
    case 1: // Блок #1
        return ChA1.Bda_inW(this);
    case 2:
        return ChA1.Bda_outW(this);
    case 3:
        return ChA1.Bda_hW(this);
    case 4:
        return ChA1.Bda_in_anW(this);
    case 5:
        return ChA1.Bda_out_anW(this);
    default:
        return new QWidget;
    }
}

void CheckDialogA1::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case 1: // Блок #1
        ChA1.FillBda_in(this);
    case 2:
        ChA1.FillBda_out(this);
    case 3:
        ChA1.FillBda_h(this);
    case 4:
        ChA1.FillBda_in_an(this);
    case 5:
        ChA1.FillBda_out_an(this);
    default:
        return;
    }
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
