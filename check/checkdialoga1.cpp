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
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    BdNum = 5; // количество блоков данных 6
    BdUINum = 8; // количество вкладок - 8 (блок Bda_h разделён ввиду его огромности на четыре вкладки)
    ChA1 = new CheckA1;
    SetBd(1, &ChA1->Bda_in, sizeof(CheckA1::A1_Bd1));
    SetBd(4, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
    SetBd(3, &ChA1->Bda_h, sizeof(CheckA1::A1_Bd2));
    SetBd(2, &ChA1->Bda_in_an, sizeof(CheckA1::A1_Bd3));
    SetBd(5, &ChA1->Bda_out_an, sizeof(CheckA1::A1_Bd4));
//    SetBd(6, &ChA1->Bd_com, sizeof(CheckA1::A1_Bd6));
    SetupUI();
    timer->setInterval(ANMEASINT);
}

QWidget *CheckDialogA1::AutoCheckUI()
{
    QWidget *w = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(UCONFCLR)+";}";
    w->setStyleSheet(tmps);
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
    case 0: // Блок #1
        return ChA1->Bda_inW(this);
    case 1:
        return ChA1->Bda_in_anW(this);
    case 2:
        return ChA1->Bda_h0W(this);
    case 3:
        return ChA1->Bda_h1W(this);
    case 4:
        return ChA1->Bda_h2W(this);
    case 5:
        return ChA1->Bda_h3W(this);
    case 6:
        return ChA1->Bda_outW(this);
    case 7:
        return ChA1->Bda_out_anW(this);
    default:
        return new QWidget;
    }
}

void CheckDialogA1::SetupUI()
{
    QStringList sl = QStringList() << "Uin" << "Ain" << "Harm1" << "Harm2" << "Harm3" << \
                                      "Harm4" << "Uout" << "Aout";
    if (sl.size() < BdUINum)
    {
        ERMSG("Wrong BdTab size");
        return;
    }
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    QString ConfTWss = "QTabBar::tab {margin-right: 0px; margin-left: 0px; padding: 5px;}"
                       "QTabBar::tab:selected {background-color: "+QString(TABCOLORA1)+";"
                        "border: 1px solid #000000;"
                        "border-top-left-radius: 4px;"
                        "border-top-right-radius: 4px;"
                        "padding: 2px;"
                        "margin-left: -4px; margin-right: -4px;}" \
                       "QTabBar::tab:first:selected {margin-left: 0;}" \
                       "QTabBar::tab:last:selected {margin-right: 0;}" \
                       "QTabBar::tab:only-one {margin: 0;}";
    CheckTW->tabBar()->setStyleSheet(ConfTWss);
//    CheckTW->addTab(AutoCheckUI(),"  Автоматическая проверка  ");
    for (int i=0; i<BdUINum; ++i)
        CheckTW->addTab(BdUI(i),"  "+sl.at(i)+"  ");
    lyout = new QVBoxLayout;
    lyout->addWidget(CheckTW);
    lyout->addWidget(BottomUI());
    setLayout(lyout);
}

void CheckDialogA1::RefreshAnalogValues(int bdnum)
{
    switch (bdnum)
    {
    case 1: // Блок #1
        ChA1->FillBda_in(this);
    case 2:
        ChA1->FillBda_in_an(this);
    case 3:
        ChA1->FillBda_h(this);
    case 4:
        ChA1->FillBda_out(this);
    case 5:
        ChA1->FillBda_out_an(this);
    default:
        return;
    }
}

void CheckDialogA1::PrepareHeadersForFile(int row)
{
    xlsx->write(row,2,QVariant("UefNat_filt1, В"));
    xlsx->write(row,3,QVariant("UefNat_filt2, В"));
    xlsx->write(row,4,QVariant("δUrms, В"));
    xlsx->write(row,5,QVariant("Uef_filt1, В"));
    xlsx->write(row,6,QVariant("Uef_filt2, В"));
    xlsx->write(row,7,QVariant("δU, В"));
    xlsx->write(row,8,QVariant("Phy, град."));
    xlsx->write(row,9,QVariant("Freq, Гц"));
    xlsx->write(row,10,QVariant("UefNat_filt1, кВ"));
    xlsx->write(row,11,QVariant("UefNat_filt2, кВ"));
    xlsx->write(row,12,QVariant("Uef_filt1, кВ"));
    xlsx->write(row,13,QVariant("Uef_filt2, кВ"));
    xlsx->write(row,14,QVariant("Phy, град."));
    xlsx->write(row,15,QVariant("Freq, Гц"));
    xlsx->write(row,16,QVariant("Tmk, град. С"));
    xlsx->write(row,17,QVariant("Vbat, В"));
    xlsx->write(row,18,QVariant("Tamb, град. С"));
    xlsx->write(row,19,QVariant("Hamb, %"));
}

void CheckDialogA1::WriteToFile(int row, int bdnum)
{
    QXlsx::Format format;
    QString Precision = "0.0000";
    format.setNumberFormat(Precision);
    switch (bdnum)
    {
    case 1:
        xlsx->write(row,2,WDFunc::StringValueWithCheck(ChA1->Bda_in.UefNat_filt[0]),format);
        xlsx->write(row,3,WDFunc::StringValueWithCheck(ChA1->Bda_in.UefNat_filt[1]),format);
        xlsx->write(row,4,WDFunc::StringValueWithCheck(ChA1->Bda_in.dUrms),format);
        xlsx->write(row,5,WDFunc::StringValueWithCheck(ChA1->Bda_in.Uef_filt[0]),format);
        xlsx->write(row,6,WDFunc::StringValueWithCheck(ChA1->Bda_in.Uef_filt[1]),format);
        xlsx->write(row,7,WDFunc::StringValueWithCheck(ChA1->Bda_in.dU),format);
        xlsx->write(row,8,WDFunc::StringValueWithCheck(ChA1->Bda_in.Phy), format);
        xlsx->write(row,9,WDFunc::StringValueWithCheck(ChA1->Bda_in.Frequency), format);
        break;
    case 4:
        xlsx->write(row,10,WDFunc::StringValueWithCheck(ChA1->Bda_out.UefNat_filt[0]), format);
        xlsx->write(row,11,WDFunc::StringValueWithCheck(ChA1->Bda_out.UefNat_filt[1]),format);
        xlsx->write(row,12,WDFunc::StringValueWithCheck(ChA1->Bda_out.Uef_filt[0]),format);
        xlsx->write(row,13,WDFunc::StringValueWithCheck(ChA1->Bda_out.Uef_filt[1]),format);
        xlsx->write(row,14,WDFunc::StringValueWithCheck(ChA1->Bda_out.Phy), format);
        xlsx->write(row,15,WDFunc::StringValueWithCheck(ChA1->Bda_out.Frequency), format);
        break;
    case 5:
        xlsx->write(row,16,WDFunc::StringValueWithCheck(ChA1->Bda_out_an.Tmk), format);
        xlsx->write(row,17,WDFunc::StringValueWithCheck(ChA1->Bda_out_an.Vbat), format);
        xlsx->write(row,18,WDFunc::StringValueWithCheck(ChA1->Bda_out_an.Tamb), format);
        xlsx->write(row,19,WDFunc::StringValueWithCheck(ChA1->Bda_out_an.Hamb), format);
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
