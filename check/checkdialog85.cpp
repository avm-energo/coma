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
#include "checkdialog85.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"
#include "../gen/publicclass.h"
#include "../config/config.h"

CheckDialog85::CheckDialog85(int board, QWidget *parent) : EAbstractCheckDialog(board, parent)
{
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    Ch85 = new Check_85;
    Ch = new Check;
    BdUINum = 2;
    SetBd(0, &Ch->Bd_block0, sizeof(Check::Bd0));
    SetBd(1, &Ch85->Bd_block1, sizeof(Check_85::Bd1));
    QStringList sl = QStringList() << "Общ" << "Все";
    SetupUI(sl);
}

QWidget *CheckDialog85::AutoCheckUI()
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

QWidget *CheckDialog85::BdUI(int bdnum)
{
    switch (bdnum)
    {
    case 0:
        return Ch->Bd0W(this);
    case 1: // Блок #1
        return Ch85->Bd1W(this);
    default:
        return new QWidget;
    }
}

void CheckDialog85::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case BD_COMMON:
        Ch->FillBd0(this);
    case 1: // Блок #1
        Ch85->FillBd1(this);
    default:
        return;
    }
}

void CheckDialog85::PrepareHeadersForFile(int row)
{
    for (int i=0; i<3; i++)
    {
        xlsx->write(row,i+2,QVariant(("U1 ф")+QString::number(i+10, 36)+", В"));
        xlsx->write(row,i+5,QVariant(("I1 ф")+QString::number(i+10, 36)+", A"));
        xlsx->write(row,i+8,QVariant(("U2 ф")+QString::number(i+10, 36)+", В"));
        xlsx->write(row,i+11,QVariant("CosPhi ф"+QString::number(i+10, 36)));
        xlsx->write(row,i+14,QVariant("Pf ф"+QString::number(i+10, 36)+", Вт"));
        xlsx->write(row,i+17,QVariant("Qf ф"+QString::number(i+10, 36)+", ВА"));
        xlsx->write(row,i+20,QVariant("Sf ф"+QString::number(i+10, 36)+", ВА"));
    }
    xlsx->write(row,23,QVariant("f, Гц"));
    xlsx->write(row,24,QVariant("t, град"));
    xlsx->write(row, 25, QVariant("Ubat, В"));
}

void CheckDialog85::WriteToFile(int row, int bdnum)
{
    // получение текущих аналоговых сигналов от модуля
    QXlsx::Format format;
    QString Precision = "0.0000";
    format.setNumberFormat(Precision);
    switch (bdnum)
    {
    case 0:
        xlsx->write(row,24,WDFunc::FloatValueWithCheck(Ch->Bd_block0.Tmk), format);
        xlsx->write(row,25,WDFunc::FloatValueWithCheck(Ch->Bd_block0.Vbat), format);
    case 1:
    {
        for (int i=0; i<3; i++)
        {
            xlsx->write(row,i+2,WDFunc::FloatValueWithCheck(Ch85->Bd_block1.IUefNat_filt[i]), format);
            xlsx->write(row,i+5,WDFunc::FloatValueWithCheck(Ch85->Bd_block1.IUefNat_filt[i+3]), format);
            xlsx->write(row,i+8,WDFunc::FloatValueWithCheck(Ch85->Bd_block1.IUefNat_filt[i+6]), format);
            xlsx->write(row,i+11,WDFunc::StringValueWithCheck(Ch85->Bd_block1.PNatf[i], 3));
            xlsx->write(row,i+14,WDFunc::StringValueWithCheck(Ch85->Bd_block1.QNatf[i], 3));
            xlsx->write(row,i+17,WDFunc::StringValueWithCheck(Ch85->Bd_block1.SNatf[i], 3));
            xlsx->write(row,i+20,WDFunc::StringValueWithCheck(Ch85->Bd_block1.CosPhiNat[i], 5));
        }
        xlsx->write(WRow,23,Ch85->Bd_block1.Frequency,format);
    }
    }
}

void CheckDialog85::ChooseValuesToWrite()
{

}

void CheckDialog85::SetDefaultValuesToWrite()
{

}

void CheckDialog85::PrepareAnalogMeasurements()
{

}

QWidget *CheckDialog85::CustomTab()
{
    return 0;
}
