#include "confdialog.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../widgets/ecombobox.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

ConfDialog::ConfDialog(QVector<S2::DataRec> *S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent)
    : QWidget(parent)
{
    ConfigMain = new Config(S2Config, MTypeB, MTypeM); // добавляем к переданному S2Config общую часть
                                                       // SetupUI();
    ConfKxx = new ConfDialogKxx(S2Config);

    Kxx = new ConfigKxx(S2Config);

    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
    // QString tmps = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
}

QWidget *ConfDialog::SetupMainBlk(QWidget *parent)
{
    // QWidget *w = new QWidget(parent);
    QString paramcolor = MAINWINCLR;
    // QVBoxLayout *vlyout = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;
    QScrollArea *scrArea = new QScrollArea(parent);
    QWidget *w = new QWidget;

    scrArea->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    scrArea->setFrameShape(QFrame::NoFrame);
    scrArea->setWidgetResizable(true);

    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);
    int i = 0;
    int row = 0;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    gb->setFont(font);
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl;
    QDoubleSpinBox *dspbls;

    lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl, 0, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "Abs_104", 0, 65535, 0, paramcolor);
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Интервал циклического опроса, с:");
    glyout->addWidget(lbl, 1, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "Cycle_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t1, с:");
    glyout->addWidget(lbl, 2, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T1_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t2, с:");
    glyout->addWidget(lbl, 3, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T2_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t3, с:");
    glyout->addWidget(lbl, 4, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T3_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl, 5, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "k_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl, 6, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "w_104", 0, 255, 0, paramcolor);
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    vlyout2->addWidget(ConfKxx->SetupComParam(this));
    gb->setLayout(vlyout2);

    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройка времени");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    //    row++;

    lbl = new QLabel("Тип синхронизации времени:");

    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);

    QStringList cbl = QStringList() << "SNTP+PPS"
                                    << "SNTP";
    EComboBox *cb = WDFunc::NewCB(this, "Ctype", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    scrArea->setWidget(w);

    return scrArea;
}

void ConfDialog::FillMainBlk()
{
    WDFunc::SetSPBData(this, "Abs_104", ConfigMain->MainBlk.Abs_104);
    WDFunc::SetSPBData(this, "Cycle_104", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SetSPBData(this, "T1_104", ConfigMain->MainBlk.T1_104);
    WDFunc::SetSPBData(this, "T2_104", ConfigMain->MainBlk.T2_104);
    WDFunc::SetSPBData(this, "T3_104", ConfigMain->MainBlk.T3_104);
    WDFunc::SetSPBData(this, "k_104", ConfigMain->MainBlk.k_104);
    WDFunc::SetSPBData(this, "w_104", ConfigMain->MainBlk.w_104);

    int cbidx = ((ConfigMain->MainBlk.Ctype & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Ctype", cbidx);
}

void ConfDialog::Fill()
{
    int i, cbidx;
    //    WDFunc::SetSPBData(this, "Abs_104", ConfigMain->MainBlk.Abs_104);
    //    WDFunc::SetSPBData(this, "Cycle_104", ConfigMain->MainBlk.Cycle_104);
    //    WDFunc::SetSPBData(this, "T1_104", ConfigMain->MainBlk.T1_104);
    //    WDFunc::SetSPBData(this, "T2_104", ConfigMain->MainBlk.T2_104);
    //    WDFunc::SetSPBData(this, "T3_104", ConfigMain->MainBlk.T3_104);
    //    WDFunc::SetSPBData(this, "k_104", ConfigMain->MainBlk.k_104);
    //    WDFunc::SetSPBData(this, "w_104", ConfigMain->MainBlk.w_104);

    //    int cbidx = ((ConfigMain->MainBlk.Ctype & 0x01) ? 1 : 0);
    //    WDFunc::SetCBIndex(this, "Ctype", cbidx);

    //....................................................
    WDFunc::SetSPBData(this, "RTerm", Kxx->TempConf.RTerm);
    WDFunc::SetSPBData(this, "W100", Kxx->TempConf.W100);
    WDFunc::SetSPBData(this, "Trele_pred", Kxx->StrTrele.Trele_pred);
    WDFunc::SetSPBData(this, "Trele_alarm", Kxx->StrTrele.Trele_alarm);

    //.................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QLocale german(QLocale::German);

    for (i = 0; i < 4; i++)
    {

        if (i == 3)
        {
            StrIP.append(german.toString(Kxx->Com_param.IP[i]));
            StrMask.append(german.toString(Kxx->Com_param.Mask[i]));
            StrGate.append(german.toString(Kxx->Com_param.GateWay[i]));
            StrSNTP.append(german.toString(Kxx->Com_param.SNTP[i]));
        }
        else
        {
            StrIP.append(german.toString(Kxx->Com_param.IP[i]) + ".");
            StrMask.append(german.toString(Kxx->Com_param.Mask[i]) + ".");
            StrGate.append(german.toString(Kxx->Com_param.GateWay[i]) + ".");
            StrSNTP.append(german.toString(Kxx->Com_param.SNTP[i]) + ".");
        }
    }

    WDFunc::SetSPBData(this, "Port_ID", Kxx->Com_param.Port[0]);

    WDFunc::LE_write_data(this, StrIP, "IP_ID");
    WDFunc::LE_write_data(this, StrSNTP, "SNTP_ID");
    WDFunc::LE_write_data(this, StrGate, "GW_ID");
    WDFunc::LE_write_data(this, StrMask, "Mask_ID");

    for (int i = 0; i < 8; i++)
    {
        if (Kxx->Com_param.Baud == Sbaud.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(this, "Baud_ID", cbidx);

    if (Kxx->Com_param.Parity > 2)
        cbidx = 0;
    else
        cbidx = Kxx->Com_param.Parity;

    WDFunc::SetCBIndex(this, "Parity_ID", cbidx);
    cbidx = ((Kxx->Com_param.Stopbit & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Stopbit_ID", cbidx);

    WDFunc::SetSPBData(this, "adrMB_ID", Kxx->Com_param.adrMB);
}

void ConfDialog::FillBack()
{
    int i;
    WDFunc::SPBData(this, "Abs_104", ConfigMain->MainBlk.Abs_104);
    WDFunc::SPBData(this, "Cycle_104", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SPBData(this, "T1_104", ConfigMain->MainBlk.T1_104);
    WDFunc::SPBData(this, "T2_104", ConfigMain->MainBlk.T2_104);
    WDFunc::SPBData(this, "T3_104", ConfigMain->MainBlk.T3_104);
    WDFunc::SPBData(this, "k_104", ConfigMain->MainBlk.k_104);
    WDFunc::SPBData(this, "w_104", ConfigMain->MainBlk.w_104);

    int cbidx = ((ConfigMain->MainBlk.Ctype & 0x01) ? 1 : 0);
    ConfigMain->MainBlk.Ctype = (0x00000001 << cbidx) - 1;

    //.......................................................................

    WDFunc::SPBData(this, "RTerm", Kxx->TempConf.RTerm);
    WDFunc::SPBData(this, "W100", Kxx->TempConf.W100);
    WDFunc::SPBData(this, "Trele_pred", Kxx->StrTrele.Trele_pred);
    WDFunc::SPBData(this, "Trele_alarm", Kxx->StrTrele.Trele_alarm);

    cbidx = ((Kxx->StrModBus.MBMaster & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "MBMaster", cbidx);

    //нет с 1061-1064

    if (Kxx->StrModBus.MBMab1[0] > 4)
        Kxx->StrModBus.MBMab1[0] = 0;
    if (Kxx->StrModBus.MBMab2[0] > 4)
        Kxx->StrModBus.MBMab2[0] = 0;
    if (Kxx->StrModBus.MBMab3[0] > 4)
        Kxx->StrModBus.MBMab3[0] = 0;
    if (Kxx->StrModBus.MBMab4[0] > 4)
        Kxx->StrModBus.MBMab4[0] = 0;

    WDFunc::SetCBIndex(this, "MBMab1[0]", Kxx->StrModBus.MBMab1[0]);
    WDFunc::SetCBIndex(this, "MBMab2[0]", Kxx->StrModBus.MBMab2[0]);
    WDFunc::SetCBIndex(this, "MBMab3[0]", Kxx->StrModBus.MBMab3[0]);
    WDFunc::SetCBIndex(this, "MBMab4[0]", Kxx->StrModBus.MBMab4[0]);

    WDFunc::SetCBIndex(this, "MBMab1sk[1]", Kxx->StrModBus.MBMab1[1] & 0x0F);
    WDFunc::SetCBIndex(this, "MBMab2sk[1]", Kxx->StrModBus.MBMab2[1] & 0x0F);
    WDFunc::SetCBIndex(this, "MBMab3sk[1]", Kxx->StrModBus.MBMab3[1] & 0x0F);
    WDFunc::SetCBIndex(this, "MBMab4sk[1]", Kxx->StrModBus.MBMab4[1] & 0x0F);

    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab1ch[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab2ch[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab3ch[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab4ch[1]", cbidx);

    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab1bt[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab2bt[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab3bt[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab4bt[1]", cbidx);

    WDFunc::SetSPBData(this, "MBMab1per[2]", Kxx->StrModBus.MBMab1[2]);
    WDFunc::SetSPBData(this, "MBMab2per[2]", Kxx->StrModBus.MBMab2[2]);
    WDFunc::SetSPBData(this, "MBMab3per[2]", Kxx->StrModBus.MBMab3[2]);
    WDFunc::SetSPBData(this, "MBMab4per[2]", Kxx->StrModBus.MBMab4[2]);

    WDFunc::SetSPBData(this, "MBMab1adr[3]", Kxx->StrModBus.MBMab1[3]);
    WDFunc::SetSPBData(this, "MBMab2adr[3]", Kxx->StrModBus.MBMab2[3]);
    WDFunc::SetSPBData(this, "MBMab3adr[3]", Kxx->StrModBus.MBMab3[3]);
    WDFunc::SetSPBData(this, "MBMab4adr[3]", Kxx->StrModBus.MBMab4[3]);

    //................................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QString NameIP = "IP_ID", NameMask = "Mask_ID", NameSNTP = "SNTP_ID", NameGate = "GW_ID";
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
        Kxx->Com_param.IP[i] = inIP.at(i).toInt();
        Kxx->Com_param.Mask[i] = inMask.at(i).toInt();
        Kxx->Com_param.GateWay[i] = inGate.at(i).toInt();
        Kxx->Com_param.SNTP[i] = inSNTP.at(i).toInt();
    }

    WDFunc::SPBData(this, "Port_ID" + QString::number(0), Kxx->Com_param.Port[0]);

    cbidx = WDFunc::CBIndex(this, "Baud_ID");
    Kxx->Com_param.Baud = (Sbaud.at(cbidx).toInt());
    cbidx = WDFunc::CBIndex(this, "Parity_ID");
    Kxx->Com_param.Parity = cbidx;
    cbidx = WDFunc::CBIndex(this, "Stopbit_ID");
    Kxx->Com_param.Stopbit = cbidx;

    WDFunc::SPBData(this, "adrMB_ID", Kxx->Com_param.adrMB);
}

void ConfDialog::SetDefConf()
{
    ConfigMain->SetDefBlock();
    /*  ConfigMain->MainBlk.Abs_104 = DEF_ABS_104;
      ConfigMain->MainBlk.Ctype = DEF_CTYPE;
      ConfigMain->MainBlk.Cycle_104 = DEF_CYCLE_104;
      ConfigMain->MainBlk.k_104 = DEF_K_104;
      ConfigMain->MainBlk.w_104 = DEF_W_104;
      ConfigMain->MainBlk.T1_104 = DEF_T1_104;
      ConfigMain->MainBlk.T2_104 = DEF_T2_104;
      ConfigMain->MainBlk.T3_104 = DEF_T3_104;
      //...........................................*/

    Kxx->TempConf.RTerm = DEF_RTERM;
    Kxx->TempConf.W100 = static_cast<float>(DEF_W100);

    Kxx->StrTrele.Trele_pred = DEF_TRELE_PRED;
    Kxx->StrTrele.Trele_alarm = DEF_TRELE_ALARM;

    Kxx->StrModBus.MBMaster = DEF_MBMASTER;

    for (int i = 0; i < 4; i++)
    {

        Kxx->StrModBus.MBMab1[i] = 0;
        Kxx->StrModBus.MBMab2[i] = 0;
        Kxx->StrModBus.MBMab3[i] = 0;
        Kxx->StrModBus.MBMab4[i] = 0;
    }

    //........................................................

    Kxx->Com_param.IP[0] = 172;
    Kxx->Com_param.IP[1] = 16;
    Kxx->Com_param.IP[2] = 29;
    Kxx->Com_param.IP[3] = 12;

    Kxx->Com_param.Mask[0] = 255;
    Kxx->Com_param.Mask[1] = 255;
    Kxx->Com_param.Mask[2] = 252;
    Kxx->Com_param.Mask[3] = 0;

    Kxx->Com_param.GateWay[0] = 172;
    Kxx->Com_param.GateWay[1] = 16;
    Kxx->Com_param.GateWay[2] = 29;
    Kxx->Com_param.GateWay[3] = 1;

    Kxx->Com_param.Port[0] = 2404;
    Kxx->Com_param.Port[1] = 2405;
    Kxx->Com_param.Port[2] = 502;
    Kxx->Com_param.Port[3] = 502;

    Kxx->Com_param.SNTP[0] = 172;
    Kxx->Com_param.SNTP[1] = 16;
    Kxx->Com_param.SNTP[2] = 31;
    Kxx->Com_param.SNTP[3] = 220;

    Kxx->Com_param.Baud = DEF_BAUD;
    Kxx->Com_param.Parity = DEF_PARITY;
    Kxx->Com_param.Stopbit = DEF_STOPBIT;

    Kxx->Com_param.adrMB = DEF_ADRMB;

    Fill();
}
