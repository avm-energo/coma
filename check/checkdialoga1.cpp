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
    SetBd(&ChA1.Bda_in, sizeof(ChA1.Bda_in));
    SetBd(&ChA1.Bda_out, sizeof(ChA1.Bda_out));
    SetBd(&ChA1.Bda_h, sizeof(ChA1.Bda_h));
    SetBd(&ChA1.Bda_in_an, sizeof(ChA1.Bda_in_an));
    SetBd(&ChA1.Bda_out_an, sizeof(ChA1.Bda_out_an));
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
        return ChA1.Bda_h0W(this);
    case 4:
        return ChA1.Bda_h1W(this);
    case 5:
        return ChA1.Bda_h2W(this);
    case 6:
        return ChA1.Bda_h3W(this);
    case 7:
        return ChA1.Bda_in_anW(this);
    case 8:
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
    xlsx->write(row,2,QVariant("UefNat_filt1, В"));
    xlsx->write(row,3,QVariant("UefNat_filt2, В"));
    xlsx->write(row,4,QVariant("Uef_filt1, В"));
    xlsx->write(row,5,QVariant("Uef_filt2, В"));
    xlsx->write(row,6,QVariant("Phy, град."));
    xlsx->write(row,7,QVariant("Freq, Гц"));
    xlsx->write(row,8,QVariant("Pt100R, Ом"));
    xlsx->write(row,9,QVariant("EXTmA1, мА"));
    xlsx->write(row,10,QVariant("EXTmA2, мА"));
    xlsx->write(row,11,QVariant("Tmk, град. С"));
    xlsx->write(row,12,QVariant("Vbat, В"));
    xlsx->write(row,13,QVariant("Tamb, град. С"));
    xlsx->write(row,14,QVariant("Hamb, %"));
}

void CheckDialogA1::WriteToFile(int row, int bdnum)
{
    QXlsx::Format format;
    QString Precision = "0.0000";
    format.setNumberFormat(Precision);
    switch (bdnum)
    {
    case 1: // Блок #1
        xlsx->write(row,2,ChA1.Bda_in.UefNat_filt[0],format);
        xlsx->write(row,3,ChA1.Bda_in.UefNat_filt[1],format);
        xlsx->write(row,4,ChA1.Bda_in.Uef_filt[0],format);
        xlsx->write(row,5,ChA1.Bda_in.Uef_filt[1],format);
        xlsx->write(row,6,ChA1.Bda_in.Phy, format);
        xlsx->write(row,7,ChA1.Bda_in.Frequency, format);
        break;
    case 2:
        xlsx->write(row,8,ChA1.Bda_in_an.Pt100_R, format);
        xlsx->write(row,9,ChA1.Bda_in_an.EXTmA1_I, format);
        xlsx->write(row,10,ChA1.Bda_in_an.EXTmA2_I, format);
        break;
    case 3:
        xlsx->write(row,11,ChA1.Bda_out_an.Tmk, format);
        xlsx->write(row,12,ChA1.Bda_out_an.Vbat, format);
        xlsx->write(row,13,ChA1.Bda_out_an.Tamb, format);
        xlsx->write(row,14,ChA1.Bda_out_an.Hamb, format);
        break;
    default:
        break;
    }
}

void CheckDialogA1::ChooseValuesToWrite()
{

}

void CheckDialogA1::SetDefaultValuesToWrite()
{

}
