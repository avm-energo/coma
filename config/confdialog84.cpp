#include <QGroupBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>
#include "../widgets/emessagebox.h"
#include "../widgets/ecombobox.h"
#include "../widgets/wd_func.h"
#include "../gen/colors.h"
#include "../gen/modulebsi.h"
#include "confdialog84.h"
#include "../gen/error.h"
#include "../gen/mainwindow.h"


ConfDialog84::ConfDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent) :
    AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);
    this->S2Config = &S2Config;
    C84 = new Config84(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    TheEnd = 0;
    SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    PrereadConf();
#endif
}

ConfDialog84::~ConfDialog84()
{
}

void ConfDialog84::Fill()
{
    int i;

    WDFunc::SetSPBData(this, "spb.1", C84->MainBlk.Abs_104);
    WDFunc::SetSPBData(this, "spb.2", C84->MainBlk.Cycle_104);
    WDFunc::SetSPBData(this, "spb.3", C84->MainBlk.T1_104);
    WDFunc::SetSPBData(this, "spb.4", C84->MainBlk.T2_104);
    WDFunc::SetSPBData(this, "spb.5", C84->MainBlk.T3_104);
    WDFunc::SetSPBData(this, "spb.6", C84->MainBlk.k_104);
    WDFunc::SetSPBData(this, "spb.7", C84->MainBlk.w_104);
    //WDFunc::SetSPBData(this, "spb.8", C84->MainBlk.Ctype);
    int cbidx = ((C84->MainBlk.Ctype & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "spb.8", cbidx);

    WDFunc::SetSPBData(this, "Unom", C84->Bci_block.Unom);
    WDFunc::SetSPBData(this, "Umin", C84->Bci_block.Umin);
    WDFunc::SetSPBData(this, "Imin", C84->Bci_block.Imin);

    for (i = 0; i < 3; i++)
    {
      //WDFunc::SetSPBData(this, "Imax."+QString::number(i), C84->Bci_block.Imax[i]);
      //WDFunc::SetSPBData(this, "C_init."+QString::number(i), C84->Bci_block.C_init[i]);
      //WDFunc::SetSPBData(this, "Tg_init."+QString::number(i), C84->Bci_block.Tg_init[i]);
      //WDFunc::SetSPBData(this, "corTg."+QString::number(i), C84->Bci_block.corTg[i]);
        WDFunc::SetSPBData(this, "Tg_pasp."+QString::number(i), C84->Bci_block.Tg_pasp[i]);
        WDFunc::SetSPBData(this, "C_pasp."+QString::number(i), C84->Bci_block.C_pasp[i]);
    }

    WDFunc::SetSPBData(this, "dС_pred", C84->Bci_block.dC_pred);
    WDFunc::SetSPBData(this, "dС_alarm", C84->Bci_block.dC_alarm);
    WDFunc::SetSPBData(this, "dTg_pred", C84->Bci_block.dTg_pred);
    WDFunc::SetSPBData(this, "dTg_alarm", C84->Bci_block.dTg_alarm);
    WDFunc::SetSPBData(this, "dIunb_pred", C84->Bci_block.dIunb_pred);
    WDFunc::SetSPBData(this, "dIunb_alarm", C84->Bci_block.dIunb_alarm);
    WDFunc::SetSPBData(this, "GdС", C84->Bci_block.GdC);
    WDFunc::SetSPBData(this, "GdTg", C84->Bci_block.GdTg);
    WDFunc::SetSPBData(this, "GdIunb", C84->Bci_block.GdIunb);
    WDFunc::SetSPBData(this, "NFiltr", C84->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "T_Data_Rec", C84->Bci_block.T_Data_Rec);
    WDFunc::SetSPBData(this, "Unom_1", C84->Bci_block.Unom_1);

    //if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    //{
        WDFunc::SetSPBData(this, "RTerm", C84->Bci_block.RTerm);
        WDFunc::SetSPBData(this, "W100", C84->Bci_block.W100);

        QString StrIP, StrMask, StrSNTP, StrGate;
        QLocale german(QLocale::German);

        for (i = 0; i < 4; i++)
        {
         /*WDFunc::SetSPBData(this, "IP"+QString::number(i), C84->Com_param.IP[i]);
         WDFunc::SetSPBData(this, "Mask"+QString::number(i), C84->Com_param.Mask[i]);
         WDFunc::SetSPBData(this, "GateWay"+QString::number(i), C84->Com_param.GateWay[i]);
         WDFunc::SetSPBData(this, "Port"+QString::number(i), C84->Com_param.Port[i]);
         WDFunc::SetSPBData(this, "SNTP"+QString::number(i), C84->Com_param.SNTP[i]);*/

         if(i==3)
         {
           StrIP.append(german.toString(C84->Com_param.IP[i]));
           StrMask.append(german.toString(C84->Com_param.Mask[i]));
           StrGate.append(german.toString(C84->Com_param.GateWay[i]));
           StrSNTP.append(german.toString(C84->Com_param.SNTP[i]));
         }
         else
         {
           StrIP.append(german.toString(C84->Com_param.IP[i])+".");
           StrMask.append(german.toString(C84->Com_param.Mask[i])+".");
           StrGate.append(german.toString(C84->Com_param.GateWay[i])+".");
           StrSNTP.append(german.toString(C84->Com_param.SNTP[i])+".");
         }
        }

        WDFunc::SetSPBData(this, "Port"+QString::number(0), C84->Com_param.Port[0]);

        WDFunc::LE_write_data(this, StrIP, "IP");
        WDFunc::LE_write_data(this, StrSNTP, "SNTP");
        WDFunc::LE_write_data(this, StrGate, "GateWay");
        WDFunc::LE_write_data(this, StrMask, "Mask");

        WDFunc::SetSPBData(this, "Baud", C84->Com_param.baud);
        for(int i= 0; i<8; i++)
        {
            if(C84->Com_param.baud == Rates.at(i).toUInt())
            cbidx = i;
        }
        WDFunc::SetCBIndex(this, "Baud", cbidx);
        cbidx = (C84->Com_param.parity & 0x04) ? 2 : ((C84->Com_param.parity & 0x02) ? 1 : 0);
        WDFunc::SetCBIndex(this, "Parity", cbidx);
        cbidx = ((C84->Com_param.stopbit & 0x02) ? 1 : 0);
        WDFunc::SetCBIndex(this, "StopBit", cbidx);
        //WDFunc::SetSPBData(this, "StopBit", C84->Com_param.stopbit);
        WDFunc::SetSPBData(this, "adrMB", C84->Com_param.adrMB);

        if(C84->Com_param.isNTP)
        WDFunc::SetChBData(this, "ISNTP", true);
        else
        WDFunc::SetChBData(this, "ISNTP", false);

        WDFunc::SetSPBData(this, "Ulow", C84->Bci_block.LowU);
        WDFunc::SetSPBData(this, "Tevent_pred", C84->Bci_block.Tevent_pred);
        WDFunc::SetSPBData(this, "Tevent_alarm", C84->Bci_block.Tevent_alarm);
        WDFunc::SetSPBData(this, "Trele_pred", C84->Bci_block.Trele_pred);
        WDFunc::SetSPBData(this, "Trele_alarm", C84->Bci_block.Trele_alarm);

        if(C84->Bci_block.IsU)
        WDFunc::SetChBData(this, "IsU", true);
        else
        WDFunc::SetChBData(this, "IsU", false);

        if(C84->Bci_block.IsIunb)
        WDFunc::SetChBData(this, "IsIunb", true);
        else
        WDFunc::SetChBData(this, "IsIunb", false);

}

void ConfDialog84::FillBack()
{
    int i;
    int cbidx;
    QString tmps;

    WDFunc::SPBData(this, "spb.1", C84->MainBlk.Abs_104);
    WDFunc::SPBData(this, "spb.2", C84->MainBlk.Cycle_104);
    WDFunc::SPBData(this, "spb.3", C84->MainBlk.T1_104);
    WDFunc::SPBData(this, "spb.4", C84->MainBlk.T2_104);
    WDFunc::SPBData(this, "spb.5", C84->MainBlk.T3_104);
    WDFunc::SPBData(this, "spb.6", C84->MainBlk.k_104);
    WDFunc::SPBData(this, "spb.7", C84->MainBlk.w_104);
    cbidx = WDFunc::CBIndex(this, "spb.8");
    C84->MainBlk.Ctype = (0x00000001 << cbidx) - 1;

    WDFunc::SPBData(this, "Unom", C84->Bci_block.Unom);
    WDFunc::SPBData(this, "Umin", C84->Bci_block.Umin);
    WDFunc::SPBData(this, "Imin", C84->Bci_block.Imin);

    for (i = 0; i < 3; i++)
    {
      //WDFunc::SPBData(this, "Imax."+QString::number(i), C84->Bci_block.Imax[i]);
      //WDFunc::SPBData(this, "C_init."+QString::number(i), C84->Bci_block.C_init[i]);
      //WDFunc::SPBData(this, "Tg_init."+QString::number(i), C84->Bci_block.Tg_init[i]);
      //WDFunc::SPBData(this, "corTg."+QString::number(i), C84->Bci_block.corTg[i]);
      WDFunc::SPBData(this, "Tg_pasp."+QString::number(i), C84->Bci_block.Tg_pasp[i]);
      WDFunc::SPBData(this, "C_pasp."+QString::number(i), C84->Bci_block.C_pasp[i]);
    }

    WDFunc::SPBData(this, "dС_pred", C84->Bci_block.dC_pred);
    WDFunc::SPBData(this, "dС_alarm", C84->Bci_block.dC_alarm);
    WDFunc::SPBData(this, "dTg_pred", C84->Bci_block.dTg_pred);
    WDFunc::SPBData(this, "dTg_alarm", C84->Bci_block.dTg_alarm);
    WDFunc::SPBData(this, "dIunb_pred", C84->Bci_block.dIunb_pred);
    WDFunc::SPBData(this, "dIunb_alarm", C84->Bci_block.dIunb_alarm);
    WDFunc::SPBData(this, "GdС", C84->Bci_block.GdC);
    WDFunc::SPBData(this, "GdTg", C84->Bci_block.GdTg);
    WDFunc::SPBData(this, "GdIunb", C84->Bci_block.GdIunb);
    WDFunc::SPBData(this, "NFiltr", C84->Bci_block.NFiltr);
    WDFunc::SPBData(this, "T_Data_Rec", C84->Bci_block.T_Data_Rec);
    WDFunc::SPBData(this, "Unom_1", C84->Bci_block.Unom_1);

    //if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    //{
        WDFunc::SPBData(this, "RTerm", C84->Bci_block.RTerm);
        WDFunc::SPBData(this, "W100", C84->Bci_block.W100);

        /*for (i = 0; i < 4; i++)
        {
         WDFunc::SPBData(this, "IP"+QString::number(i), C84->Com_param.IP[i]);
         WDFunc::SPBData(this, "Mask"+QString::number(i), C84->Com_param.Mask[i]);
         WDFunc::SPBData(this, "GateWay"+QString::number(i), C84->Com_param.GateWay[i]);
         WDFunc::SPBData(this, "Port"+QString::number(i), C84->Com_param.Port[i]);
         WDFunc::SPBData(this, "SNTP"+QString::number(i), C84->Com_param.SNTP[i]);
        }*/

        QString StrIP, StrMask, StrSNTP, StrGate;
        QString NameIP = "IP", NameMask = "Mask", NameSNTP = "SNTP", NameGate = "GateWay";
        QStringList inIP, inMask, inSNTP, inGate;
        QLocale german(QLocale::German);

        WDFunc::LE_read_data(this, NameIP, StrIP);
        WDFunc::LE_read_data(this, NameSNTP, StrSNTP);
        WDFunc::LE_read_data(this, NameGate, StrGate);
        WDFunc::LE_read_data(this, NameMask, StrMask);

        inIP.append(StrIP.split("."));
        inMask.append(StrMask.split("."));
        inSNTP.append(StrSNTP.split("."));
        inGate.append(StrGate.split("."));

        for (i = 0; i < 4; i++)
        {

         /*WDFunc::SetSPBData(this, "IP"+QString::number(i), C84->Com_param.IP[i]);
         WDFunc::SetSPBData(this, "Mask"+QString::number(i), C84->Com_param.Mask[i]);
         WDFunc::SetSPBData(this, "GateWay"+QString::number(i), C84->Com_param.GateWay[i]);
         WDFunc::SetSPBData(this, "Port"+QString::number(i), C84->Com_param.Port[i]);
         WDFunc::SetSPBData(this, "SNTP"+QString::number(i), C84->Com_param.SNTP[i]);*/

         C84->Com_param.IP[i] = inIP.at(i).toInt();
         C84->Com_param.Mask[i] = inMask.at(i).toInt();
         C84->Com_param.GateWay[i] = inGate.at(i).toInt();
         C84->Com_param.SNTP[i] = inSNTP.at(i).toInt();
        }

        WDFunc::SPBData(this, "Port"+QString::number(0), C84->Com_param.Port[0]);

        cbidx = WDFunc::CBIndex(this, "Baud");
        C84->Com_param.baud = (Rates.at(cbidx).toInt());
        cbidx = WDFunc::CBIndex(this, "Parity");
        C84->Com_param.parity = (0x00000001 << cbidx) - 1;
        cbidx = WDFunc::CBIndex(this, "StopBit");
        C84->Com_param.stopbit = 0x00000001 << cbidx;

        WDFunc::SPBData(this, "adrMB", C84->Com_param.adrMB);
        /*if(C84->Com_param.isNTP)
        WDFunc::SetChBData(this, "ISNTP", true);
        else
        WDFunc::SetChBData(this, "ISNTP", false);*/
        WDFunc::ChBData(this, "ISNTP", IsNtp);
        if(IsNtp)
        C84->Com_param.isNTP = 1;
        else
        C84->Com_param.isNTP = 0;

        WDFunc::SPBData(this, "Ulow", C84->Bci_block.LowU);
        WDFunc::SPBData(this, "Tevent_pred", C84->Bci_block.Tevent_pred);
        WDFunc::SPBData(this, "Tevent_alarm", C84->Bci_block.Tevent_alarm);
        WDFunc::SPBData(this, "Trele_pred", C84->Bci_block.Trele_pred);
        WDFunc::SPBData(this, "Trele_alarm", C84->Bci_block.Trele_alarm);

        WDFunc::ChBData(this, "IsU", IsNtp);
        if(IsNtp)
        C84->Bci_block.IsU = 1;
        else
        C84->Bci_block.IsU = 0;

        WDFunc::ChBData(this, "IsIunb", IsNtp);
        if(IsNtp)
        C84->Bci_block.IsIunb= 1;
        else
        C84->Bci_block.IsIunb = 0;


}

void ConfDialog84::SetupUI()
{
    QString phase[3] = {"Фаза A:","Фаза B:","Фаза C:"};
    //QString S;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QScrollArea *area = new QScrollArea;
    QScrollArea *area2 = new QScrollArea;
    QWidget *analog1 = new QWidget;
    QWidget *analog2 = new QWidget;
    QWidget *extraconf = new QWidget;
    QWidget *MEKconf = new QWidget;
    QWidget *Leftconf = new QWidget;
    QWidget *time = new QWidget;
    QString tmps = "QWidget {background-color: "+QString(ACONFWCLR)+";}";
    analog1->setStyleSheet(tmps);
    analog2->setStyleSheet(tmps);
    extraconf->setStyleSheet(tmps);
    time->setStyleSheet(tmps);
    MEKconf->setStyleSheet(tmps);
    Leftconf->setStyleSheet(tmps);

    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    uint32_t unixtimestamp = 1423062000;

    QDateTime myDateTime;

    myDateTime.setTime_t(unixtimestamp);

    QString paramcolor = MAINWINCLR;
    QFont font;

    QGroupBox *gb = new QGroupBox;

   // gb->setTitle("Настройка времени");
    /*glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl,0,0,1,1,Qt::AlignRight);
    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "spb.1", 0, 65535, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса, с:");
    glyout->addWidget(lbl,0,2,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.2", 0, 255, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 3, 1, 1, Qt::AlignLeft);
    //lbl=new QLabel("c");
    //glyout->addWidget(lbl,0,4,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl,1,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.6", 0, 255, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    //lbl=new QLabel("c");
    //glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl,1,2,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.7", 0, 255, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 3, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t1, с:");
    glyout->addWidget(lbl,2,0,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.3", 0, 255, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    //lbl=new QLabel("c");
    //glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t2, с:");
    glyout->addWidget(lbl,2,2,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.4", 0, 255, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 3, 1, 1, Qt::AlignLeft);
    //lbl=new QLabel("c");
    //glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    lbl = new QLabel("Тайм-аут t3, с:");
    glyout->addWidget(lbl,2,4,1,1,Qt::AlignRight);
    dspbls = WDFunc::NewSPB(this, "spb.5", 0, 255, 0, paramcolor);
    connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 5, 1, 1, Qt::AlignLeft);
    //lbl=new QLabel("c");
    //glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);

    //lbl=new QLabel("c");
    //glyout->addWidget(lbl,6,2,1,1);
    lbl = new QLabel("Тип синхр. времени:");
    glyout->addWidget(lbl,3,0,1,1,Qt::AlignRight);
    QStringList cbl = QStringList() << "SNTP+PPS" << "SNTP";
    EComboBox *cb = WDFunc::NewCB(this, "spb.8", cbl, paramcolor);
    cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, 3, 1, 1, 2);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);*/
    //MEKconf->setLayout(vlyout1);

    /*vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);*/

    int row = 0;
    gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    //setFont(font);
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное линейное первичное напряжение, кВ:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom", 0, 10000, 0, paramcolor), row,2,1,3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение первой тройки, В:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom_1", 0, 10000, 1, paramcolor), row,2,1,3);
    row++;

    /*for(int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
    }
    row++;*/

    for (int i = 0; i < 3; i++)
    {
     //glyout->addWidget(WDFunc::NewLBL(this, " "), row, i,1,1,Qt::AlignLeft);
     glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1, Qt::AlignTop);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Паспортные значения ёмкости вводов, пФ:"), row,1,1,1, Qt::AlignTop);
    for (int i = 0; i < 3; i++)
    {
     //glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
     glyout->addWidget(WDFunc::NewSPB(this, "C_pasp."+QString::number(i), 0, 20000, 1, paramcolor), row,2+i,1,1, Qt::AlignTop);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Паспортные значения tg вводов, %:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     //glyout->addWidget(WDFunc::NewLBL(this, phase[i]), row,2+i,1,1,Qt::AlignLeft);
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_pasp."+QString::number(i), 0, 10, 2, paramcolor), row,2+i,1,1);
    }
    row++;

    for(int i=0; i<10; i++)
    {
        glyout->addWidget(WDFunc::NewLBL(this, ""), row,1,1,1);
        row++;
    }

    /*glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов, %:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Tg_init."+QString::number(i), 0, 10000, 1, paramcolor), row,2+i,1,1);
    }
    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов:"), row,1,1,1);

    for (int i = 0; i < 3; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "corTg."+QString::number(i), 0, 10000, 1, paramcolor), row, 2+i, 1, 1);
    }
    row++; */


    /*row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iunb_init", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Phy_unb_init", 0, 10000, 1, paramcolor), row,2,1,3);*/

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    /*gb = new QGroupBox("Осциллограммы");
    vlyout2 = new QVBoxLayout;
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewLBL(this, "Запуск осциллограммы:"));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.0", "по команде Ц", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.1", "по дискр. входу PD1", ACONFWCLR));
    hlyout->addWidget(WDFunc::NewChB(this, "oscchb.2", "по резкому изменению", ACONFWCLR));
    vlyout2->addLayout(hlyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);*/
    analog1->setLayout(vlyout1);


    gb = new QGroupBox("Уставки сигнализации");
    gb->setFont(font);
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению емкости, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dС_pred", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению емкости, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dС_alarm", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению tg δ, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTg_pred", 0, 1000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению tg δ, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTg_alarm", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по изменению небаланса токов, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dIunb_pred", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка аварийной сигнализации по изменению небаланса токов, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "dIunb_alarm", 0, 10000, 1, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Уставки контроля минимума тока и напряжения");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума напряжения (в % от номинального):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Umin", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума тока (в % от Imax):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row,2,1,3);


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Гистерезис");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dC, % от уставки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdС", 0, 10000, 1, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по dTg, % от уставки:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdTg", 0, 10000, 3, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис на отключение сигнализации по небалансу токов, %:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "GdIunb", 0, 10000, 1, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Сигнализации событий");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Низкое напряжение для сигнализации:"), row,0,1,1,Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(this, "Ulow", 0, 10000, 1, paramcolor), row,1,1,1);

    //row++;
    glyout->addWidget(WDFunc::NewChB(this, "IsU", "Сигнализация по наличию входного напряжения"), row,2,1,1);

    //row++;


    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Задержка на формирование предупредительных событий:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tevent_pred", 0, 10000, 1, paramcolor), row,1,1,1);

     glyout->addWidget(WDFunc::NewChB(this, "IsIunb", "Сигнализация по небалансу токов"), row,2,1,1);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Задержка на формирование аварийных событий:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tevent_alarm", 0, 10000, 1, paramcolor), row,1,1,1);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Задержка на срабатывание реле предупредительной сигнализации:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Trele_pred", 0, 10000, 1, paramcolor), row,1,1,1);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Задержка на срабатывание реле аварийной сигнализации:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Trele_alarm", 0, 10000, 1, paramcolor), row,1,1,1);


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    analog2->setLayout(vlyout1);
    area2->setWidget(analog2);

    gb = new QGroupBox("Конфигурация 104");
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

        gb->setTitle("Настройки протокола МЭК-60870-5-104");
        gb->setFont(font);
        glyout = new QGridLayout;
        glyout->setColumnStretch(2, 50);
        QLabel *lbl = new QLabel("Адрес базовой станции:");
        glyout->addWidget(lbl,0,0,1,1,Qt::AlignLeft);
        QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "spb.1", 0, 65535, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
        lbl = new QLabel("Интервал циклического опроса, с:");
        glyout->addWidget(lbl,1,0,1,1,Qt::AlignLeft);
        dspbls = WDFunc::NewSPB(this, "spb.2", 0, 255, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
        //lbl=new QLabel("c");
        //glyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
        lbl = new QLabel("Тайм-аут t1, с:");
        glyout->addWidget(lbl,2,0,1,1,Qt::AlignLeft);
        dspbls = WDFunc::NewSPB(this, "spb.3", 0, 255, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
        //lbl=new QLabel("c");
        //glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
        lbl = new QLabel("Тайм-аут t2, с:");
        glyout->addWidget(lbl,3,0,1,1,Qt::AlignLeft);
        dspbls = WDFunc::NewSPB(this, "spb.4", 0, 255, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
        //lbl=new QLabel("c");
        //glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
        lbl = new QLabel("Тайм-аут t3, с:");
        glyout->addWidget(lbl,4,0,1,1,Qt::AlignLeft);
        dspbls = WDFunc::NewSPB(this, "spb.5", 0, 255, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
        //lbl=new QLabel("c");
        //glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
        lbl = new QLabel("Макс. число неподтв. APDU (k):");
        glyout->addWidget(lbl,5,0,1,1,Qt::AlignLeft);
        dspbls = WDFunc::NewSPB(this, "spb.6", 0, 255, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
        //lbl=new QLabel("c");
        //glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
        lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
        glyout->addWidget(lbl,6,0,1,1,Qt::AlignLeft);
        dspbls = WDFunc::NewSPB(this, "spb.7", 0, 255, 0, paramcolor);
        connect(dspbls,SIGNAL(valueChanged(double)),this,SLOT(Set104(double)));
        glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
        //lbl=new QLabel("c");
        //glyout->addWidget(lbl,6,2,1,1);


    row += 7;

    QString Str;
    QLocale german(QLocale::German);
    int i;
    for(i = 0; i<4; i++)
    {
      C84->Com_param.IP[i]=0;
      if(i==3)
      Str.append(german.toString(C84->Com_param.IP[i]));
      else
      Str.append(german.toString(C84->Com_param.IP[i])+".");
    }


    glyout->addWidget(WDFunc::NewLBL(this, "IP адрес устройства:"), row,0,1,1);

    glyout->addWidget(WDFunc::NewLE(this, "IP", Str, paramcolor), row,1,1,1, Qt::AlignLeft);

    /*for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "IP"+QString::number(i), 0, 10000, 0, paramcolor), row,1+i,1, 1, Qt::AlignLeft);
    }*/

    row++;
    Str.clear();
    for(i = 0; i<4; i++)
    {
      C84->Com_param.Mask[i]=0;
      if(i==3)
      Str.append(german.toString(C84->Com_param.Mask[i]));
      else
      Str.append(german.toString(C84->Com_param.Mask[i])+".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Маска:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewLE(this, "Mask", Str, paramcolor), row,1,1,1, Qt::AlignLeft);
    /*for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Mask"+QString::number(i), 0, 10000, 0, paramcolor) , row,1+i,1,1, Qt::AlignLeft);
    }*/

    row++;
    Str.clear();
    for(i = 0; i<4; i++)
    {
      C84->Com_param.GateWay[i]=0;
      if(i==3)
      Str.append(german.toString(C84->Com_param.GateWay[i]));
      else
      Str.append(german.toString(C84->Com_param.GateWay[i])+".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Шлюз:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewLE(this, "GateWay", Str, paramcolor), row,1,1,1, Qt::AlignLeft);
    /*for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "GateWay"+QString::number(i), 0, 10000, 0, paramcolor), row,1+i,1,1, Qt::AlignLeft);
    }*/

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Порт протокола 104:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "Port0", 0, 10000, 0, paramcolor), row,1,1,1);
    /*for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "Port"+QString::number(i), 0, 10000, 0, paramcolor), row,2+i,1,1, Qt::AlignLeft);
    }*/

    row++;
    glyout->addWidget(WDFunc::NewChB(this, "ISNTP", "Использование SNTP сервера"), row,0,1,1);

    row++;
    Str.clear();
    for(i = 0; i<4; i++)
    {
      C84->Com_param.SNTP[i]=0;
      if(i==3)
      Str.append(german.toString(C84->Com_param.SNTP[i]));
      else
      Str.append(german.toString(C84->Com_param.SNTP[i])+".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес SNTP сервера:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewLE(this, "SNTP", Str, paramcolor), row,1,1,1, Qt::AlignLeft);
    /*for (int i = 0; i < 4; i++)
    {
     glyout->addWidget(WDFunc::NewSPB(this, "SNTP"+QString::number(i), 0, 10000, 0, paramcolor), row,1+i,1,1, Qt::AlignLeft);
    }*/


    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройка времени");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    row++;
    lbl = new QLabel("Тип синхронизации времени:");
    glyout->addWidget(lbl,row,0,1,1,Qt::AlignLeft);
    QStringList cbl = QStringList() << "SNTP+PPS" << "SNTP";
    EComboBox *cb = WDFunc::NewCB(this, "spb.8", cbl, paramcolor);
    //cb->setMinimumWidth(70);
    connect(cb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройки ModBus");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость RS485 интерфейса:"), row,0,1,1);
    cbl = QStringList() << "1200" << "2400"<< "4800" << "9600" << "19200" << "38400" << "57600" << "115200";
    cb = WDFunc::NewCB(this, "Baud", cbl, paramcolor);
    //cb->setMinimumWidth(80);
    //cb->setMinimumHeight(15);
    glyout->addWidget(cb,row,1,1,1);


    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Чётность:"), row,0,1,1);
    cbl = QStringList() << "NoParity" << "EvenParity" << "OddParity";
    cb = WDFunc::NewCB(this, "Parity", cbl, paramcolor);
    //cb->setMinimumWidth(80);
    //cb->setMinimumHeight(15);
    glyout->addWidget(cb,row,1,1,1);
    //glyout->addWidget(WDFunc::NewSPB(this, "Parity", 0, 10000, 0, paramcolor), row,2,1,4);


    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество стоповых битов:"), row,0,1,1);
    cbl = QStringList() << "Stop_Bit_1" << "Stop_Bit_2";
    cb = WDFunc::NewCB(this, "StopBit", cbl, paramcolor);
    //cb->setMinimumWidth(80);
    //cb->setMinimumHeight(20);
    glyout->addWidget(cb,row,1,1,1);
    //glyout->addWidget(WDFunc::NewSPB(this, "StopBit", 0, 10000, 0, paramcolor), row,2,1,4);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес устройства для Modbus:"), row,0,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "adrMB", 0, 10000, 0, paramcolor), row,1,1,1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    extraconf->setLayout(vlyout1);
    area->setWidget(extraconf);


    gb = new QGroupBox("Параметры записи");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;

    row=0;
    glyout->addWidget(WDFunc::NewLBL(this, "Интервал усреднения данных  (в периодах основной частоты):"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "NFiltr", 0, 10000, 0, paramcolor), row,2,1,3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), в секундах:"), row,1,1,1);
    glyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row,2,1,3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    if((ModuleBSI::GetMType(BoardTypes::BT_BASE) << 8) == Config::MTB_A2)
    {
        gb = new QGroupBox("Температура");
        gb->setFont(font);
        vlyout2 = new QVBoxLayout;
        glyout = new QGridLayout;

        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Сопротивление термометра при 0°С, Ом (только для АВМ):"), row,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "RTerm", 0, 10000, 1, paramcolor), row,2,1,3);

        row++;
        glyout->addWidget(WDFunc::NewLBL(this, "Температурный коэффициент термометра (только для АВМ):"), row,1,1,1);
        glyout->addWidget(WDFunc::NewSPB(this, "W100", 0, 10000, 3, paramcolor), row,2,1,3);

        vlyout2->addLayout(glyout);
        gb->setLayout(vlyout2);
        vlyout1->addWidget(gb);
    }



    Leftconf->setLayout(vlyout1);
    //area->setWidget(Leftconf);


    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    //ConfTW->addTab(MEKconf,"Конфигурирование МЭК-60870-5-104");
    ConfTW->addTab(analog1,"Аналоговые");

    if(MainWindow::MTypeB == Config::MTB_A2)
    {
      ConfTW->addTab(area2,"Уставки");
      area2->verticalScrollBar()->setValue(area2->verticalScrollBar()->maximum());
      //area2->setSizeIncrement(QSize(1000,1000));
      ConfTW->addTab(area,"Связь");
      area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    }
    else
    ConfTW->addTab(analog2,"Уставки");

    ConfTW->addTab(Leftconf,"Остальное");
    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialog84::CheckConf()
{
}




void ConfDialog84::SetDefConf()
{
    C84->SetDefConf();
    Fill();
}

void ConfDialog84::slot_timeOut()
{
   // WDFunc::LBLText(this, "adrMB", currentTime().toString("hh:mm:ss"));
    SysTime->setText(QDateTime::currentDateTimeUtc().toString("dd-MM-yyyy HH:mm:ss"));
    //SysTime->update();
}

void ConfDialog84::slot2_timeOut()
{
/*    uint unixtimestamp = 0;
    QDateTime myDateTime;

    if (Commands::GetTimeMNK(unixtimestamp) == NOERROR)
    {
      myDateTime = QDateTime::fromTime_t(unixtimestamp, Qt::UTC);
      //myDateTime.setTime_t(unixtimestamp);
      SysTime2->setText(myDateTime.toString("dd-MM-yyyy HH:mm:ss"));

      //EMessageBox::information(this, "INFO", "Прочитано успешно");
    }
    //else
    //EMessageBox::information(this, "INFO", "Ошибка");*/

}

void ConfDialog84::Start_Timer()
{
   timerRead->start(1000);
}

void ConfDialog84::Stop_Timer()
{
   timerRead->stop();
}

void ConfDialog84::Write_PCDate()
{
    QDateTime myDateTime;
    uint time ;
    myDateTime = QDateTime::currentDateTimeUtc();
    time = myDateTime.toTime_t();

    /*if (Commands::WriteTimeMNK(&time, sizeof(uint)) == NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");*/

}

void ConfDialog84::Write_Date()
{
    QDateTime myDateTime;
    uint *time = new uint;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr,"dd-MM-yyyy HH:mm:ss");
    myDateTime.setOffsetFromUtc(0);
    *time = myDateTime.toTime_t();

   /* if (Commands::WriteTimeMNK(time, sizeof(uint)) == NOERROR)
    EMessageBox::information(this, "INFO", "Записано успешно");
    else
    EMessageBox::information(this, "INFO", "Ошибка");*/

}

void ConfDialog84::Set104(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 1)
    {
        DBGMSG;
        return;
    }
    bool ok;
    int wnum = sl.at(1).toInt(&ok);
    if (!ok)
    {
        DBGMSG;
        return;
    }
    switch (wnum)
    {
    case 1:
    {
        C84->MainBlk.Abs_104=dbl;
        break;
    }
    case 2:
    {
        C84->MainBlk.Cycle_104=dbl;
        break;
    }
    case 3:
    {
        C84->MainBlk.T1_104=dbl;
        break;
    }
    case 4:
    {
        C84->MainBlk.T2_104=dbl;
        break;
    }
    case 5:
    {
        C84->MainBlk.T3_104=dbl;
        break;
    }
    case 6:
    {
        C84->MainBlk.k_104=dbl;
        break;
    }
    case 7:
    {
        C84->MainBlk.w_104=dbl;
        break;
    }
    default:
        break;
    }
}

void ConfDialog84::SetCType(int num)
{
    C84->MainBlk.Ctype = num;
}



