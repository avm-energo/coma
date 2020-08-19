#include "confdialogktf.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../widgets/ecombobox.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

ConfDialogKTF::ConfDialogKTF(QVector<S2::DataRec> *S2Config, QWidget *parent) : AbstractConfDialog(parent)
{
    QString tmps = "QDialog {background-color: " + QString(ACONFCLR) + ";}";
    setStyleSheet(tmps);
    this->S2Config = S2Config;
    KTF = new ConfigKTF(S2Config);
    Conf = new ConfDialog(S2Config, MTypeB, MTypeM);
    ConfKxx = new ConfDialogKxx(S2Config);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    PrereadConf();
}

ConfDialogKTF::~ConfDialogKTF()
{
}

void ConfDialogKTF::Fill()
{
    int i;
    //    WDFunc::SetSPBData(this, "Abs_104", KTF->MainBlk.Abs_104);
    //    WDFunc::SetSPBData(this, "Cycle_104", KTF->MainBlk.Cycle_104);
    //    WDFunc::SetSPBData(this, "T1_104", KTF->MainBlk.T1_104);
    //    WDFunc::SetSPBData(this, "T2_104", KTF->MainBlk.T2_104);
    //    WDFunc::SetSPBData(this, "T3_104", KTF->MainBlk.T3_104);
    //    WDFunc::SetSPBData(this, "k_104", KTF->MainBlk.k_104);
    //    WDFunc::SetSPBData(this, "w_104", KTF->MainBlk.w_104);

    //    int cbidx = ((KTF->MainBlk.Ctype & 0x01) ? 1 : 0);
    //    WDFunc::SetCBIndex(this, "Ctype", cbidx);

    WDFunc::SetSPBData(this, "NFiltr_ID", KTF->Bci_block.NFiltr);
    WDFunc::SetSPBData(this, "NHarmFilt_ID", KTF->Bci_block.NHarmFilt);

    //   WDFunc::SetSPBData(this, "DDosc_ID", KTF->Bci_block.DDosc_ID);

    if (KTF->Bci_block.DDosc & 0x04)
        WDFunc::SetChBData(this, "DDosc_ID1", true);
    else
        WDFunc::SetChBData(this, "DDosc_ID1", false);

    if (KTF->Bci_block.DDosc & 0x01)
        WDFunc::SetChBData(this, "DDosc_ID2", true);
    else
        WDFunc::SetChBData(this, "DDosc_ID2", false);

    WDFunc::SetSPBData(this, "Unom1", KTF->Bci_block.Unom1);
    WDFunc::SetSPBData(this, "DUosc", KTF->Bci_block.DUosc);
    WDFunc::SetSPBData(this, "DIosc_ID", KTF->Bci_block.DIosc);
    WDFunc::SetSPBData(this, "DUImin_ID", KTF->Bci_block.DUImin);
    WDFunc::SetSPBData(this, "Imin", KTF->Bci_block.Imin);
    //    WDFunc::SetSPBData(this, "RTerm", KTF->Bci_block.RTerm);
    //    WDFunc::SetSPBData(this, "W100", KTF->Bci_block.W100);
    WDFunc::SetSPBData(this, "T_Data_Rec", KTF->Bci_block.T_Data_Rec);
    //    WDFunc::SetSPBData(this, "Trele_pred", KTF->Bci_block.Trele_pred);
    //    WDFunc::SetSPBData(this, "Trele_alarm", KTF->Bci_block.Trele_alarm);
    WDFunc::SetSPBData(this, "U2nom", KTF->Bci_block.U2nom);
    WDFunc::SetSPBData(this, "ITT1nom", KTF->Bci_block.ITT1nom);
    WDFunc::SetSPBData(this, "ITT2nom", KTF->Bci_block.ITT2nom);

    //    cbidx = ((KTF->Bci_block.MBMaster & 0x01) ? 1 : 0);
    //    WDFunc::SetCBIndex(this, "MBMaster", cbidx);

    //    //нет с 1061-1064

    //    if (KTF->Bci_block.MBMab1[0] > 4)
    //        KTF->Bci_block.MBMab1[0] = 0;
    //    if (KTF->Bci_block.MBMab2[0] > 4)
    //        KTF->Bci_block.MBMab2[0] = 0;
    //    if (KTF->Bci_block.MBMab3[0] > 4)
    //        KTF->Bci_block.MBMab3[0] = 0;
    //    if (KTF->Bci_block.MBMab4[0] > 4)
    //        KTF->Bci_block.MBMab4[0] = 0;

    //    WDFunc::SetCBIndex(this, "MBMab1[0]", KTF->Bci_block.MBMab1[0]);
    //    WDFunc::SetCBIndex(this, "MBMab2[0]", KTF->Bci_block.MBMab2[0]);
    //    WDFunc::SetCBIndex(this, "MBMab3[0]", KTF->Bci_block.MBMab3[0]);
    //    WDFunc::SetCBIndex(this, "MBMab4[0]", KTF->Bci_block.MBMab4[0]);

    //    WDFunc::SetCBIndex(this, "MBMab1sk[1]", KTF->Bci_block.MBMab1[1] & 0x0F);
    //    WDFunc::SetCBIndex(this, "MBMab2sk[1]", KTF->Bci_block.MBMab2[1] & 0x0F);
    //    WDFunc::SetCBIndex(this, "MBMab3sk[1]", KTF->Bci_block.MBMab3[1] & 0x0F);
    //    WDFunc::SetCBIndex(this, "MBMab4sk[1]", KTF->Bci_block.MBMab4[1] & 0x0F);

    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x60) >> 5;
    //    WDFunc::SetCBIndex(this, "MBMab1ch[1]", cbidx);
    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x60) >> 5;
    //    WDFunc::SetCBIndex(this, "MBMab2ch[1]", cbidx);
    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x60) >> 5;
    //    WDFunc::SetCBIndex(this, "MBMab3ch[1]", cbidx);
    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x60) >> 5;
    //    WDFunc::SetCBIndex(this, "MBMab4ch[1]", cbidx);

    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x80) >> 7;
    //    WDFunc::SetCBIndex(this, "MBMab1bt[1]", cbidx);
    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x80) >> 7;
    //    WDFunc::SetCBIndex(this, "MBMab2bt[1]", cbidx);
    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x80) >> 7;
    //    WDFunc::SetCBIndex(this, "MBMab3bt[1]", cbidx);
    //    cbidx = (KTF->Bci_block.MBMab1[1] & 0x80) >> 7;
    //    WDFunc::SetCBIndex(this, "MBMab4bt[1]", cbidx);

    //    WDFunc::SetSPBData(this, "MBMab1per[2]", KTF->Bci_block.MBMab1[2]);
    //    WDFunc::SetSPBData(this, "MBMab2per[2]", KTF->Bci_block.MBMab2[2]);
    //    WDFunc::SetSPBData(this, "MBMab3per[2]", KTF->Bci_block.MBMab3[2]);
    //    WDFunc::SetSPBData(this, "MBMab4per[2]", KTF->Bci_block.MBMab4[2]);

    //    WDFunc::SetSPBData(this, "MBMab1adr[3]", KTF->Bci_block.MBMab1[3]);
    //    WDFunc::SetSPBData(this, "MBMab2adr[3]", KTF->Bci_block.MBMab2[3]);
    //    WDFunc::SetSPBData(this, "MBMab3adr[3]", KTF->Bci_block.MBMab3[3]);
    //    WDFunc::SetSPBData(this, "MBMab4adr[3]", KTF->Bci_block.MBMab4[3]);

    int cbidx = ((KTF->Bci_block.Cool_type & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Cool_type", cbidx);
    cbidx = ((KTF->Bci_block.W_mat & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "W_mat", cbidx);

    WDFunc::SetSPBData(this, "TNNTdop", KTF->Bci_block.TNNTdop);
    WDFunc::SetSPBData(this, "TNNTpred", KTF->Bci_block.TNNTpred);
    WDFunc::SetSPBData(this, "Tamb_nom", KTF->Bci_block.Tamb_nom);
    WDFunc::SetSPBData(this, "dTNNTnom", KTF->Bci_block.dTNNTnom);
    WDFunc::SetSPBData(this, "Kdob", KTF->Bci_block.Kdob);
    WDFunc::SetSPBData(this, "TauWnom", KTF->Bci_block.TauWnom);
    WDFunc::SetSPBData(this, "Umaxm", KTF->Bci_block.Umax);
    WDFunc::SetSPBData(this, "Imaxm", KTF->Bci_block.Imax);
    WDFunc::SetSPBData(this, "Iwnom", KTF->Bci_block.Iwnom);

    for (int i = 0; i < 5; i++)
    {
        if (KTF->Bci_block.OscPoints == Rates.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(this, "OscPoints", cbidx);

    WDFunc::SetSPBData(this, "GTnnt", KTF->Bci_block.GTnnt);
    WDFunc::SetSPBData(this, "GOvc", KTF->Bci_block.GOvc);
    WDFunc::SetSPBData(this, "TdatNum", KTF->Bci_block.TdatNum);

    //......................................

    //    QString StrIP, StrMask, StrSNTP, StrGate;
    //    QLocale german(QLocale::German);

    //    for (i = 0; i < 4; i++)
    //    {

    //        if (i == 3)
    //        {
    //            StrIP.append(german.toString(KTF->Com_param.IP[i]));
    //            StrMask.append(german.toString(KTF->Com_param.Mask[i]));
    //            StrGate.append(german.toString(KTF->Com_param.GW[i]));
    //            StrSNTP.append(german.toString(KTF->Com_param.SNTP[i]));
    //        }
    //        else
    //        {
    //            StrIP.append(german.toString(KTF->Com_param.IP[i]) + ".");
    //            StrMask.append(german.toString(KTF->Com_param.Mask[i]) + ".");
    //            StrGate.append(german.toString(KTF->Com_param.GW[i]) + ".");
    //            StrSNTP.append(german.toString(KTF->Com_param.SNTP[i]) + ".");
    //        }
    //    }

    //    WDFunc::SetSPBData(this, "Port_ID", KTF->Com_param.Port[0]);

    //    WDFunc::LE_write_data(this, StrIP, "IP_ID");
    //    WDFunc::LE_write_data(this, StrSNTP, "SNTP_ID");
    //    WDFunc::LE_write_data(this, StrGate, "GW_ID");
    //    WDFunc::LE_write_data(this, StrMask, "Mask_ID");

    //    for (int i = 0; i < 8; i++)
    //    {
    //        if (KTF->Com_param.Baud == Sbaud.at(i).toUInt())
    //            cbidx = i;
    //    }
    //    WDFunc::SetCBIndex(this, "Baud_ID", cbidx);

    //    if (KTF->Com_param.Parity > 2)
    //        cbidx = 0;
    //    else
    //        cbidx = KTF->Com_param.Parity;

    //    WDFunc::SetCBIndex(this, "Parity_ID", cbidx);
    //    cbidx = ((KTF->Com_param.Stopbit & 0x01) ? 1 : 0);
    //    WDFunc::SetCBIndex(this, "Stopbit_ID", cbidx);

    //    WDFunc::SetSPBData(this, "adrMB_ID", KTF->Com_param.adrMB);

    //    if (KTF->Com_param.IsNTP)
    //        WDFunc::SetChBData(this, "ISNTP_ID", true);
    //    else
    //        WDFunc::SetChBData(this, "ISNTP_ID", false);
}

void ConfDialogKTF::FillBack()
{
    int i;
    int cbidx;
    QString tmps;

    //    WDFunc::SPBData(this, "Abs_104", KTF->MainBlk.Abs_104);
    //    WDFunc::SPBData(this, "Cycle_104", KTF->MainBlk.Cycle_104);
    //    WDFunc::SPBData(this, "T1_104", KTF->MainBlk.T1_104);
    //    WDFunc::SPBData(this, "T2_104", KTF->MainBlk.T2_104);
    //    WDFunc::SPBData(this, "T3_104", KTF->MainBlk.T3_104);
    //    WDFunc::SPBData(this, "k_104", KTF->MainBlk.k_104);
    //    WDFunc::SPBData(this, "w_104", KTF->MainBlk.w_104);

    //    cbidx = ((KTF->MainBlk.Ctype & 0x01) ? 1 : 0);
    //    KTF->MainBlk.Ctype = (0x00000001 << cbidx) - 1;

    WDFunc::SPBData(this, "NFiltr_ID", KTF->Bci_block.NFiltr);
    WDFunc::SPBData(this, "NHarmFilt_ID", KTF->Bci_block.NHarmFilt);

    // WDFunc::SPBData(this, "DDosc_ID", KTF->Bci_block.DDosc_ID);

    WDFunc::ChBData(this, "DDosc_ID1", DDosc);
    if (DDosc)
    {
        KTF->Bci_block.DDosc = 0x04;
    }

    else
        KTF->Bci_block.DDosc = 0;

    WDFunc::ChBData(this, "DDosc_ID2", DDosc);
    if (DDosc)
    {
        KTF->Bci_block.DDosc = KTF->Bci_block.DDosc | 0x01;
    }

    WDFunc::SPBData(this, "Unom1", KTF->Bci_block.Unom1);
    WDFunc::SPBData(this, "DUosc", KTF->Bci_block.DUosc);
    WDFunc::SPBData(this, "DIosc_ID", KTF->Bci_block.DIosc);
    WDFunc::SPBData(this, "DUImin_ID", KTF->Bci_block.DUImin);
    WDFunc::SPBData(this, "Imin", KTF->Bci_block.Imin);
    //    WDFunc::SPBData(this, "RTerm", KTF->Bci_block.RTerm);
    //    WDFunc::SPBData(this, "W100", KTF->Bci_block.W100);
    WDFunc::SPBData(this, "T_Data_Rec", KTF->Bci_block.T_Data_Rec);
    //    WDFunc::SPBData(this, "Trele_pred", KTF->Bci_block.Trele_pred);
    //    WDFunc::SPBData(this, "Trele_alarm", KTF->Bci_block.Trele_alarm);
    WDFunc::SPBData(this, "U2nom", KTF->Bci_block.U2nom);
    WDFunc::SPBData(this, "ITT1nom", KTF->Bci_block.ITT1nom);
    WDFunc::SPBData(this, "ITT2nom", KTF->Bci_block.ITT2nom);

    //    cbidx = WDFunc::CBIndex(this, "MBMaster");
    //    KTF->Bci_block.MBMaster = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "MBMab1[0]");
    //    KTF->Bci_block.MBMab1[0] = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "MBMab2[0]");
    //    KTF->Bci_block.MBMab2[0] = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "MBMab3[0]");
    //    KTF->Bci_block.MBMab3[0] = cbidx;

    //    cbidx = WDFunc::CBIndex(this, "MBMab4[0]");
    //    KTF->Bci_block.MBMab4[0] = cbidx;

    //..........................

    //    cbidx = WDFunc::CBIndex(this, "MBMab1sk[1]");
    //    KTF->Bci_block.MBMab1[1] = cbidx;
    //    cbidx = WDFunc::CBIndex(this, "MBMab1ch[1]");
    //    KTF->Bci_block.MBMab1[1] = KTF->Bci_block.MBMab1[1] | (cbidx << 5);
    //    cbidx = WDFunc::CBIndex(this, "MBMab1bt[1]");
    //    KTF->Bci_block.MBMab1[1] = KTF->Bci_block.MBMab1[1] | (cbidx << 7);

    //    cbidx = WDFunc::CBIndex(this, "MBMab2sk[1]");
    //    KTF->Bci_block.MBMab2[1] = cbidx;
    //    cbidx = WDFunc::CBIndex(this, "MBMab2ch[1]");
    //    KTF->Bci_block.MBMab2[1] = KTF->Bci_block.MBMab2[1] | (cbidx << 5);
    //    cbidx = WDFunc::CBIndex(this, "MBMab2bt[1]");
    //    KTF->Bci_block.MBMab2[1] = KTF->Bci_block.MBMab2[1] | (cbidx << 7);

    //    cbidx = WDFunc::CBIndex(this, "MBMab3sk[1]");
    //    KTF->Bci_block.MBMab3[1] = cbidx;
    //    cbidx = WDFunc::CBIndex(this, "MBMab3ch[1]");
    //    KTF->Bci_block.MBMab3[1] = KTF->Bci_block.MBMab3[1] | (cbidx << 5);
    //    cbidx = WDFunc::CBIndex(this, "MBMab3bt[1]");
    //    KTF->Bci_block.MBMab3[1] = KTF->Bci_block.MBMab3[1] | (cbidx << 7);

    //    cbidx = WDFunc::CBIndex(this, "MBMab4sk[1]");
    //    KTF->Bci_block.MBMab4[1] = cbidx;
    //    cbidx = WDFunc::CBIndex(this, "MBMab4ch[1]");
    //    KTF->Bci_block.MBMab4[1] = KTF->Bci_block.MBMab4[1] | (cbidx << 5);
    //    cbidx = WDFunc::CBIndex(this, "MBMab4bt[1]");
    //    KTF->Bci_block.MBMab4[1] = KTF->Bci_block.MBMab4[1] | (cbidx << 7);

    //    WDFunc::SPBData(this, "MBMab1per[2]", KTF->Bci_block.MBMab1[2]);
    //    WDFunc::SPBData(this, "MBMab2per[2]", KTF->Bci_block.MBMab2[2]);
    //    WDFunc::SPBData(this, "MBMab3per[2]", KTF->Bci_block.MBMab3[2]);
    //    WDFunc::SPBData(this, "MBMab4per[2]", KTF->Bci_block.MBMab4[2]);

    //    WDFunc::SPBData(this, "MBMab1adr[3]", KTF->Bci_block.MBMab1[3]);
    //    WDFunc::SPBData(this, "MBMab2adr[3]", KTF->Bci_block.MBMab2[3]);
    //    WDFunc::SPBData(this, "MBMab3adr[3]", KTF->Bci_block.MBMab3[3]);
    //    WDFunc::SPBData(this, "MBMab4adr[3]", KTF->Bci_block.MBMab4[3]);

    cbidx = WDFunc::CBIndex(this, "Cool_type");
    KTF->Bci_block.Cool_type = cbidx;

    cbidx = WDFunc::CBIndex(this, "W_mat");
    KTF->Bci_block.W_mat = cbidx;

    WDFunc::SPBData(this, "TNNTdop", KTF->Bci_block.TNNTdop);
    WDFunc::SPBData(this, "TNNTpred", KTF->Bci_block.TNNTpred);
    WDFunc::SPBData(this, "Tamb_nom", KTF->Bci_block.Tamb_nom);
    WDFunc::SPBData(this, "dTNNTnom", KTF->Bci_block.dTNNTnom);
    WDFunc::SPBData(this, "Kdob", KTF->Bci_block.Kdob);
    WDFunc::SPBData(this, "TauWnom", KTF->Bci_block.TauWnom);
    WDFunc::SPBData(this, "Umaxm", KTF->Bci_block.Umax);
    WDFunc::SPBData(this, "Imaxm", KTF->Bci_block.Imax);
    WDFunc::SPBData(this, "Iwnom", KTF->Bci_block.Iwnom);

    cbidx = WDFunc::CBIndex(this, "OscPoints");
    KTF->Bci_block.OscPoints = (Rates.at(cbidx).toInt());

    WDFunc::SPBData(this, "GTnnt", KTF->Bci_block.GTnnt);
    WDFunc::SPBData(this, "GOvc", KTF->Bci_block.GOvc);
    WDFunc::SPBData(this, "TdatNum", KTF->Bci_block.TdatNum);

    //..................................

    //    QString StrIP, StrMask, StrSNTP, StrGate;
    //    QString NameIP = "IP_ID", NameMask = "Mask_ID", NameSNTP = "SNTP_ID", NameGate = "GW_ID";
    //    QStringList inIP, inMask, inSNTP, inGate;
    //    QLocale german(QLocale::German);

    //    WDFunc::LE_read_data(this, NameIP, StrIP);
    //    WDFunc::LE_read_data(this, NameSNTP, StrSNTP);
    //    WDFunc::LE_read_data(this, NameGate, StrGate);
    //    WDFunc::LE_read_data(this, NameMask, StrMask);

    //    inIP.append(StrIP.split("."));
    //    inMask.append(StrMask.split("."));
    //    inSNTP.append(StrSNTP.split("."));
    //    inGate.append(StrGate.split("."));

    //    for (i = 0; i < 4; i++)
    //    {
    //        KTF->Com_param.IP[i] = inIP.at(i).toInt();
    //        KTF->Com_param.Mask[i] = inMask.at(i).toInt();
    //        KTF->Com_param.GW[i] = inGate.at(i).toInt();
    //        KTF->Com_param.SNTP[i] = inSNTP.at(i).toInt();
    //    }

    //    WDFunc::SPBData(this, "Port_ID" + QString::number(0), KTF->Com_param.Port[0]);

    //    cbidx = WDFunc::CBIndex(this, "Baud_ID");
    //    KTF->Com_param.Baud = (Sbaud.at(cbidx).toInt());
    //    cbidx = WDFunc::CBIndex(this, "Parity_ID");
    //    KTF->Com_param.Parity = cbidx;
    //    cbidx = WDFunc::CBIndex(this, "Stopbit_ID");
    //    KTF->Com_param.Stopbit = cbidx;

    //    WDFunc::SPBData(this, "adrMB_ID", KTF->Com_param.adrMB);

    //    WDFunc::ChBData(this, "ISNTP_ID", IsNtp);
    //    if (IsNtp)
    //        KTF->Com_param.IsNTP = 1;
    //    else
    //        KTF->Com_param.IsNTP = 0;
}

void ConfDialogKTF::SetupUI()
{
    QString phase[3] = { "Фаза A:", "Фаза B:", "Фаза C:" };
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QScrollArea *area = new QScrollArea;
    QScrollArea *area2 = new QScrollArea;
    QWidget *wind1 = new QWidget;
    QWidget *wind2 = new QWidget;
    QWidget *wind3 = new QWidget;
    QWidget *wind4 = new QWidget;
    QWidget *wind5 = new QWidget;
    QWidget *wind6 = new QWidget;
    QWidget *wind7 = new QWidget;
    QList<QWidget *> wl;

    //    QStackedWidget *qswt = new QStackedWidget;
    //    qswt->setObjectName("qswt");

    QLineEdit *line1 = new QLineEdit();

    QString tmps = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
    wind1->setStyleSheet(tmps);
    wind2->setStyleSheet(tmps);
    wind3->setStyleSheet(tmps);
    wind4->setStyleSheet(tmps);
    wind5->setStyleSheet(tmps);
    wind6->setStyleSheet(tmps);
    wind7->setStyleSheet(tmps);

    area->setStyleSheet("QScrollArea {background-color: rgba(0,0,0,0);}");
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);

    uint32_t unixtimestamp = 1423062000;

    QDateTime myDateTime;

    myDateTime.setTime_t(unixtimestamp);

    QString paramcolor = MAINWINCLR;
    QFont font;

    QGroupBox *gb = new QGroupBox;

    //.........................................................................

    int row = 0;
    gb = new QGroupBox("Аналоговые параметры");
    font.setFamily("Times");
    font.setPointSize(11);
    // setFont(font);
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Класс напряжения, кВ:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Unom1", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение , В:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "U2nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный первичный ток, А:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "ITT1nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный вторичный ток , А:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "ITT2nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this,
                          "Максимальное измеряемое фазное напряжение на входе "
                          "прибора, В эфф (не более 305В):"),
        row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Umaxm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this,
                          "Максимальное измеряемый ток на входе "
                          "прибора, А эфф (не более 33А) :"),
        row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imaxm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальный ток контролирууемой обмотки, А:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Iwnom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума сигналов:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "DUImin_ID", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Уставка контроля минимума токов:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Imin", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    wind1->setLayout(vlyout1);

    //.........................................................................

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    row = 0;

    QLabel *lbl = new QLabel();
    lbl = new QLabel("Вид охлаждения: ");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList cbl = QStringList() << "естественное"
                                    << "принудительное";
    EComboBox *cb = WDFunc::NewCB(this, "Cool_type", cbl, paramcolor);
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);
    row++;
    lbl = new QLabel("Материал охлаждения: ");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList() << "медь"
                        << "алюминий";
    cb = WDFunc::NewCB(this, "W_mat", cbl, paramcolor);
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коэффициент добавочных потерь :"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Kdob", 0, 10000, 1, paramcolor), row, 1, 1, 1);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Постоянная времени нагрева обмотки в номинальном режиме, мин:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TauWnom", 0, 10000, 0, paramcolor), row, 1, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    wind2->setLayout(vlyout1);

    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Предельно допустимая температура ННТ в°С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TNNTdop", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка предупредительной сигнализации по температуре ННТ в °С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "TNNTpred", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная температура окружающей среды, °С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Tamb_nom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Превышение температуры ННТ при номинальной нагрузке, °С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "dTNNTnom", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по температуре ННТ,  град.С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "GTnnt", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Гистерезис сигнализации по токовой перегрузке, % от :"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "GOvc", 0, 10000, 1, paramcolor), row, 2, 1, 3);
    row++;

    //    glyout->addWidget(WDFunc::NewLBL(this, "Задержка срабатывания реле предупредительной сигнализации:"), row, 1,
    //    1, 1); glyout->addWidget(WDFunc::NewSPB(this, "Trele_pred", 0, 10000, 0, paramcolor), row, 2, 1, 3); row++;

    //    glyout->addWidget(WDFunc::NewLBL(this, "Задержка срабатывания реле аварийной сигнализации:"), row, 1, 1, 1);
    //    glyout->addWidget(WDFunc::NewSPB(this, "Trele_alarm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    //    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    wind3->setLayout(vlyout1);

    //.........................................................................

    gb = new QGroupBox();
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    vlyout1 = new QVBoxLayout;
    glyout = new QGridLayout;
    row = 0;

    glyout->addWidget(WDFunc::NewLBL(this, "Разрешение запуска осциллограммы:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewChB(this, "DDosc_ID1", "по команде 104"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewChB(this, "DDosc_ID2", "по резкому изменению"), row, 2, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox();
    gb->setFont(font);
    // vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    row = 0;

    glyout->addWidget(WDFunc::NewLBL(this,
                          "Уставка скачка напряжения для запуска "
                          "осциллографирования - % от номинала:"),
        row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "DUosc", 0, 10000, 1, paramcolor), row, 1, 1, 1);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL(this, "Уставка скачка тока для запуска осциллографирования -  % от I2nom:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "DIosc_ID", 0, 10000, 1, paramcolor), row, 1, 1, 1);
    row++;

    lbl = new QLabel("Количество точек осциллограммы на период основной частоты: ");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    cbl = QStringList() << "256"
                        << "128"
                        << "64"
                        << "32"
                        << "16";
    cb = WDFunc::NewCB(this, "OscPoints", cbl, paramcolor);
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(cb, row, 1, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    wind4->setLayout(vlyout1);

    //.........................................................................

    //    row = 0;
    //    gb = new QGroupBox();
    //    gb->setFont(font);
    //    glyout = new QGridLayout;
    //    vlyout1 = new QVBoxLayout;
    //    vlyout1->setObjectName("VL");
    //    vlyout2 = new QVBoxLayout;
    //    glyout->setColumnStretch(1, 20);

    //    lbl = new QLabel("Modbus: ");
    //    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    //    QStringList dopcbl = QStringList() << "slave"
    //                                       << "master";
    //    EComboBox *dopcb = WDFunc::NewCB(this, "MBMaster", dopcbl, paramcolor);
    //    //  connect(dopcb,SIGNAL(currentIndexChanged(int)),this,SLOT(SetCType(int)));
    //    connect(dopcb, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeWindow(int)));
    //    glyout->addWidget(dopcb, row, 1, 1, 1);
    //    row++;

    //    vlyout2->addLayout(glyout);
    //    gb->setLayout(vlyout2);
    //    vlyout1->addWidget(gb);

    //    //  <<<<<<<<<<<<<<<<<<-
    //    row = 0;
    //    gb = new QGroupBox("Настройки ModBus");
    //    gb->setObjectName("Gb1");
    //    gb->setFont(font);
    //    vlyout2 = new QVBoxLayout;
    //    glyout = new QGridLayout;
    //    // glyout->setColumnStretch(2, 50);

    //    row++;
    //    glyout->addWidget(WDFunc::NewLBL(this, "Скорость RS485 интерфейса:"), row, 0, 1, 1);
    //    cbl = QStringList() << "1200"
    //                        << "2400"
    //                        << "4800"
    //                        << "9600"
    //                        << "19200"
    //                        << "38400"
    //                        << "57600"
    //                        << "115200";
    //    cb = WDFunc::NewCB(this, "Baud_ID", cbl, paramcolor);
    //    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    //    row++;
    //    glyout->addWidget(WDFunc::NewLBL(this, "Чётность:"), row, 0, 1, 1);
    //    cbl = QStringList() << "NoParity"
    //                        << "EvenParity"
    //                        << "OddParity";
    //    cb = WDFunc::NewCB(this, "Parity_ID", cbl, paramcolor);
    //    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    //    row++;
    //    glyout->addWidget(WDFunc::NewLBL(this, "Количество стоповых битов:"), row, 0, 1, 1);
    //    cbl = QStringList() << "1"
    //                        << "2";
    //    cb = WDFunc::NewCB(this, "Stopbit_ID", cbl, paramcolor);
    //    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    //    row++;
    //    glyout->addWidget(WDFunc::NewLBL(this, "Адрес устройства для Modbus:"), row, 0, 1, 1);
    //    glyout->addWidget(WDFunc::NewSPB(this, "adrMB_ID", 1, 254, 0, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    //    vlyout2->addLayout(glyout);
    //    gb->setLayout(vlyout2);
    //    qswt->addWidget(gb);
    //    WidgetList.append(gb);

    //    //  <<<<<<<<<<<<<<<<<<<<-

    //    row = 0;
    //    gb = new QGroupBox("Настройки ModBus");
    //    gb->setObjectName("Gb2");
    //    gb->setFont(font);
    //    vlyout2 = new QVBoxLayout;
    //    glyout = new QGridLayout;
    //    line1 = new QLineEdit(this);

    //    line1->setText("тип датчика:");
    //    glyout->addWidget(line1, 1, 1, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("           ");
    //    glyout->addWidget(line1, 0, 1, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("              параметры связи:          ");
    //    glyout->addWidget(line1, 0, 2, 1, 3);

    //    line1 = new QLineEdit(this);
    //    line1->setText("           ");
    //    glyout->addWidget(line1, 0, 5, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("           ");
    //    glyout->addWidget(line1, 0, 6, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("скорость:");
    //    glyout->addWidget(line1, 1, 2, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("чётность:");
    //    glyout->addWidget(line1, 1, 3, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("стопБиты:");
    //    glyout->addWidget(line1, 1, 4, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("переод опроса:");
    //    glyout->addWidget(line1, 1, 5, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("адрес абонента:");
    //    glyout->addWidget(line1, 1, 6, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("Датчик 1:");
    //    glyout->addWidget(line1, 2, 0, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("Датчик 2:");
    //    glyout->addWidget(line1, 3, 0, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("Датчик 3:");
    //    glyout->addWidget(line1, 4, 0, 1, 1);

    //    line1 = new QLineEdit(this);
    //    line1->setText("Датчик 4:");
    //    glyout->addWidget(line1, 5, 0, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "тип 1"
    //                        << "тип 2"
    //                        << "тип 3";
    //    EComboBox *сb = WDFunc::NewCB(this, "MBMab1[0]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 2, 1, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "тип 1"
    //                        << "тип 2"
    //                        << "тип 3";
    //    сb = WDFunc::NewCB(this, "MBMab2[0]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 3, 1, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "тип 1"
    //                        << "тип 2"
    //                        << "тип 3";
    //    сb = WDFunc::NewCB(this, "MBMab3[0]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 4, 1, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "тип 1"
    //                        << "тип 2"
    //                        << "тип 3";
    //    сb = WDFunc::NewCB(this, "MBMab4[0]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 5, 1, 1, 1);

    //    cbl = QStringList() << "1200"
    //                        << "2400"
    //                        << "4800"
    //                        << "9600"
    //                        << "19200"
    //                        << "38400"
    //                        << "57600"
    //                        << "115200";
    //    сb = WDFunc::NewCB(this, "MBMab1sk[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 2, 2, 1, 1);

    //    cbl = QStringList() << "1200"
    //                        << "2400"
    //                        << "4800"
    //                        << "9600"
    //                        << "19200"
    //                        << "38400"
    //                        << "57600"
    //                        << "115200";
    //    сb = WDFunc::NewCB(this, "MBMab2sk[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 3, 2, 1, 1);

    //    cbl = QStringList() << "1200"
    //                        << "2400"
    //                        << "4800"
    //                        << "9600"
    //                        << "19200"
    //                        << "38400"
    //                        << "57600"
    //                        << "115200";
    //    сb = WDFunc::NewCB(this, "MBMab3sk[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 4, 2, 1, 1);

    //    cbl = QStringList() << "1200"
    //                        << "2400"
    //                        << "4800"
    //                        << "9600"
    //                        << "19200"
    //                        << "38400"
    //                        << "57600"
    //                        << "115200";
    //    сb = WDFunc::NewCB(this, "MBMab4sk[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 5, 2, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "even"
    //                        << "old";
    //    сb = WDFunc::NewCB(this, "MBMab1ch[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 2, 3, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "even"
    //                        << "old";
    //    сb = WDFunc::NewCB(this, "MBMab2ch[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 3, 3, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "even"
    //                        << "old";
    //    сb = WDFunc::NewCB(this, "MBMab3ch[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 4, 3, 1, 1);

    //    cbl = QStringList() << "нет"
    //                        << "even"
    //                        << "old";
    //    сb = WDFunc::NewCB(this, "MBMab4ch[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 5, 3, 1, 1);

    //    cbl = QStringList() << "1"
    //                        << "2";
    //    сb = WDFunc::NewCB(this, "MBMab1bt[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 2, 4, 1, 1);

    //    cbl = QStringList() << "1"
    //                        << "2";
    //    сb = WDFunc::NewCB(this, "MBMab2bt[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 3, 4, 1, 1);

    //    cbl = QStringList() << "1"
    //                        << "2";
    //    сb = WDFunc::NewCB(this, "MBMab3bt[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 4, 4, 1, 1);

    //    cbl = QStringList() << "1"
    //                        << "2";
    //    сb = WDFunc::NewCB(this, "MBMab4bt[1]", cbl, paramcolor);
    //    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(сb, 5, 4, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab1per[2]", 0, 10000, 0, paramcolor), 2, 5, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab2per[2]", 0, 10000, 0, paramcolor), 3, 5, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab3per[2]", 0, 10000, 0, paramcolor), 4, 5, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab4per[2]", 0, 10000, 0, paramcolor), 5, 5, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab1adr[3]", 0, 10000, 0, paramcolor), 2, 6, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab2adr[3]", 0, 10000, 0, paramcolor), 3, 6, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab3adr[3]", 0, 10000, 0, paramcolor), 4, 6, 1, 1);

    //    glyout->addWidget(WDFunc::NewSPB(this, "MBMab4adr[3]", 0, 10000, 0, paramcolor), 5, 6, 1, 1);

    //    vlyout2->addLayout(glyout);
    //    gb->setLayout(vlyout2);
    //    qswt->addWidget(gb);

    //    WidgetList.append(gb);

    //    vlyout1->addWidget(qswt);

    //    wind5->setLayout(vlyout1);
    //    area2->setWidget(wind5);

    //.........................................................................

    row = 0;
    gb = new QGroupBox();
    gb->setFont(font);
    glyout = new QGridLayout;
    vlyout1 = new QVBoxLayout;
    vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Интервал усреднения данных, период:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "NFiltr_ID", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Постоянная времени фильтрации гармоник, цикл:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "NHarmFilt_ID", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Интервал записи данных в ПЗУ (тренд), с:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "T_Data_Rec", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    //    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное сопротивление термометра при 0 град.С:"), row, 1, 1, 1);
    //    glyout->addWidget(WDFunc::NewSPB(this, "RTerm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    //    row++;

    //    glyout->addWidget(WDFunc::NewLBL(this, "Температурный коэффициент термометра:"), row, 1, 1, 1);
    //    glyout->addWidget(WDFunc::NewSPB(this, "W100", 0, 10000, 3, paramcolor), row, 2, 1, 3);
    //    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    wind6->setLayout(vlyout1);

    //.........................................................................

    //    row = 0;
    //    gb = new QGroupBox("Конфигурация 104");
    //    gb->setFont(font);
    //    glyout = new QGridLayout;
    //    vlyout1 = new QVBoxLayout;
    //    vlyout2 = new QVBoxLayout;

    //    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    //    gb->setFont(font);
    //    glyout = new QGridLayout;
    //    glyout->setColumnStretch(2, 50);
    //    lbl = new QLabel("Адрес базовой станции:");
    //    glyout->addWidget(lbl, 0, 0, 1, 1, Qt::AlignLeft);
    //    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "Abs_104", 0, 65535, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    //    lbl = new QLabel("Интервал циклического опроса, с:");
    //    glyout->addWidget(lbl, 1, 0, 1, 1, Qt::AlignLeft);
    //    dspbls = WDFunc::NewSPB(this, "Cycle_104", 0, 255, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);
    //    // lbl=new QLabel("c");
    //    // glyout->addWidget(lbl,1,2,1,1,Qt::AlignLeft);
    //    lbl = new QLabel("Тайм-аут t1, с:");
    //    glyout->addWidget(lbl, 2, 0, 1, 1, Qt::AlignLeft);
    //    dspbls = WDFunc::NewSPB(this, "T1_104", 0, 255, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);
    //    // lbl=new QLabel("c");
    //    // glyout->addWidget(lbl,2,2,1,1,Qt::AlignLeft);
    //    lbl = new QLabel("Тайм-аут t2, с:");
    //    glyout->addWidget(lbl, 3, 0, 1, 1, Qt::AlignLeft);
    //    dspbls = WDFunc::NewSPB(this, "T2_104", 0, 255, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);
    //    // lbl=new QLabel("c");
    //    // glyout->addWidget(lbl,3,2,1,1,Qt::AlignLeft);
    //    lbl = new QLabel("Тайм-аут t3, с:");
    //    glyout->addWidget(lbl, 4, 0, 1, 1, Qt::AlignLeft);
    //    dspbls = WDFunc::NewSPB(this, "T3_104", 0, 255, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);
    //    // lbl=new QLabel("c");
    //    // glyout->addWidget(lbl,4,2,1,1,Qt::AlignLeft);
    //    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    //    glyout->addWidget(lbl, 5, 0, 1, 1, Qt::AlignLeft);
    //    dspbls = WDFunc::NewSPB(this, "k_104", 0, 255, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);
    //    // lbl=new QLabel("c");
    //    // glyout->addWidget(lbl,5,2,1,1,Qt::AlignLeft);
    //    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    //    glyout->addWidget(lbl, 6, 0, 1, 1, Qt::AlignLeft);
    //    dspbls = WDFunc::NewSPB(this, "w_104", 0, 255, 0, paramcolor);
    //    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    //    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);
    //    row++;

    //    row += 7;

    //    QString Str;
    //    QLocale german(QLocale::German);
    //    int i;
    //    for (i = 0; i < 4; i++)
    //    {
    //        KTF->Com_param.IP[i] = 0;
    //        if (i == 3)
    //            Str.append(german.toString(KTF->Com_param.IP[i]));
    //        else
    //            Str.append(german.toString(KTF->Com_param.IP[i]) + ".");
    //    }

    //    glyout->addWidget(WDFunc::NewLBL(this, "IP адрес устройства:"), row, 0, 1, 1);
    //    glyout->addWidget(WDFunc::NewLE(this, "IP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    //    row++;
    //    Str.clear();
    //    for (i = 0; i < 4; i++)
    //    {
    //        KTF->Com_param.Mask[i] = 0;
    //        if (i == 3)
    //            Str.append(german.toString(KTF->Com_param.Mask[i]));
    //        else
    //            Str.append(german.toString(KTF->Com_param.Mask[i]) + ".");
    //    }
    //    glyout->addWidget(WDFunc::NewLBL(this, "Маска:"), row, 0, 1, 1);
    //    glyout->addWidget(WDFunc::NewLE(this, "Mask_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    //    row++;
    //    Str.clear();
    //    for (i = 0; i < 4; i++)
    //    {
    //        KTF->Com_param.GW[i] = 0;
    //        if (i == 3)
    //            Str.append(german.toString(KTF->Com_param.GW[i]));
    //        else
    //            Str.append(german.toString(KTF->Com_param.GW[i]) + ".");
    //    }
    //    glyout->addWidget(WDFunc::NewLBL(this, "Шлюз:"), row, 0, 1, 1);
    //    glyout->addWidget(WDFunc::NewLE(this, "GW_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    //    row++;
    //    glyout->addWidget(WDFunc::NewLBL(this, "Порт протокола 104:"), row, 0, 1, 1);
    //    glyout->addWidget(WDFunc::NewSPB(this, "Port_ID", 0, 10000, 0, paramcolor), row, 1, 1, 1);

    //    row++;
    //    glyout->addWidget(WDFunc::NewChB(this, "ISNTP_ID", "Использование SNTP сервера"), row, 0, 1, 1);

    //    row++;
    //    Str.clear();
    //    for (i = 0; i < 4; i++)
    //    {
    //        KTF->Com_param.SNTP[i] = 0;
    //        if (i == 3)
    //            Str.append(german.toString(KTF->Com_param.SNTP[i]));
    //        else
    //            Str.append(german.toString(KTF->Com_param.SNTP[i]) + ".");
    //    }
    //    glyout->addWidget(WDFunc::NewLBL(this, "Адрес SNTP сервера:"), row, 0, 1, 1);
    //    glyout->addWidget(WDFunc::NewLE(this, "SNTP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    //    vlyout2->addLayout(glyout);
    //    gb->setLayout(vlyout2);
    //    vlyout1->addWidget(gb);

    //    gb = new QGroupBox("Настройка времени");
    //    gb->setFont(font);
    //    vlyout2 = new QVBoxLayout;
    //    glyout = new QGridLayout;
    //    glyout->setColumnStretch(2, 50);
    //    row++;
    //    lbl = new QLabel("Тип синхронизации времени:");
    //    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    //    cbl = QStringList() << "SNTP+PPS"
    //                        << "SNTP";
    //    cb = WDFunc::NewCB(this, "Ctype", cbl, paramcolor);
    //    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    //    glyout->addWidget(cb, row, 1, 1, 1);

    //    vlyout2->addLayout(glyout);
    //    gb->setLayout(vlyout2);
    //    vlyout1->addWidget(gb);

    //    wind7->setLayout(vlyout1);
    //    area->setWidget(wind7);

    //.........................................................................

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(TABCOLOR) + ";}";
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(wind1, "Аналоговые");
    ConfTW->addTab(wind2, "Трансформатор");
    ConfTW->addTab(wind3, "Сигнализация");
    ConfTW->addTab(wind4, "Осциллографирование");
    // ConfTW->addTab(wind5, "ModBusMaster");
    //  area2->verticalScrollBar()->setValue(area2->verticalScrollBar()->maximum());

    ConfTW->addTab(Conf->SetupMainBlk(this), "Связь");
    ConfTW->addTab(ConfKxx->SetupModBus(), "ModBusMaster");
    ConfTW->addTab(ConfKxx->SetupBl(this), "Общее");

    //   ConfTW->addTab(Conf->SetupComParam(), "ModBusMaster");
    //  ConfTW->addTab(Conf->SetupBl(this), "Общее");

    ConfTW->addTab(wind6, "Прочее");

    //    if (MTypeB == Config::MTB_A2)
    //    {
    //        ConfTW->addTab(area, "Связь");
    //        area->verticalScrollBar()->setValue(area->verticalScrollBar()->maximum());
    //    }

    lyout->addWidget(ConfTW);

    QWidget *wdgt = ConfButtons();
    lyout->addWidget(wdgt);
    setLayout(lyout);
}

void ConfDialogKTF::CheckConf()
{
}

void ConfDialogKTF::SetDefConf()
{
    KTF->SetDefConf();
    Fill();
}

void ConfDialogKTF::Start_Timer()
{
    timerRead->start(1000);
}

void ConfDialogKTF::Stop_Timer()
{
    timerRead->stop();
}

void ConfDialogKTF::Write_PCDate()
{
}

void ConfDialogKTF::Write_Date()
{
    QDateTime myDateTime;
    uint *time = new uint;
    QString qStr;
    WDFunc::LE_read_data(this, "Date", qStr);
    myDateTime = QDateTime::fromString(qStr, "dd-MM-yyyy HH:mm:ss");
    myDateTime.setOffsetFromUtc(0);
    *time = myDateTime.toTime_t();
}

// void ConfDialogKTF::Set104(double dbl)
//{
//    Q_UNUSED(dbl);
//}

// void ConfDialogKTF::SetCType(int num)
//{
//    // KTF->MainBlk.Ctype = num;
//}

// void ConfDialogKTF::ChangeWindow(int num)
//{
//    //    KTF->MainBlk.Ctype = num;

//    //    QStackedWidget *QSWT = this->findChild<QStackedWidget *>("qswt");

//    //    if (QSWT != nullptr)
//    //        QSWT->setCurrentWidget(WidgetList.at(num));
//}
