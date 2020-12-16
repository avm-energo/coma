#include "configkxx.h"

#include "../gen/s2.h"
#include "../module/module_kxx.h"
#include "../widgets/wd_func.h"
#include "config.h"

#include <QComboBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QVBoxLayout>

ConfigKxx::ConfigKxx(QObject *parent) : QObject(parent)
{
    setConfig();
}

void ConfigKxx::setConfig()
{

    Config::removeFotter();

    S2::config->append({ Bci::RTERM, sizeof(TempConf.RTerm), &TempConf.RTerm });
    S2::config->append({ Bci::W100, sizeof(TempConf.W100), &TempConf.W100 });

    S2::config->append({ Bci::TRELE_PRED, sizeof(StrTrele.Trele_pred), &StrTrele.Trele_pred });
    S2::config->append({ Bci::TRELE_ALARM, sizeof(StrTrele.Trele_alarm), &StrTrele.Trele_alarm });

    S2::config->append({ Bci::MBMASTER, sizeof(StrModBus.MBMaster), &StrModBus.MBMaster });
    S2::config->append({ Bci::MBMAB1, sizeof(StrModBus.MBMab1), &StrModBus.MBMab1 });
    S2::config->append({ Bci::MBMAB2, sizeof(StrModBus.MBMab2), &StrModBus.MBMab2 });
    S2::config->append({ Bci::MBMAB3, sizeof(StrModBus.MBMab3), &StrModBus.MBMab3 });
    S2::config->append({ Bci::MBMAB4, sizeof(StrModBus.MBMab4), &StrModBus.MBMab4 });

    S2::config->append({ Bci::IP, sizeof(Com_param.IP), &Com_param.IP });
    S2::config->append({ Bci::MASK, sizeof(Com_param.Mask), &Com_param.Mask });
    S2::config->append({ Bci::GW, sizeof(Com_param.GateWay), &Com_param.GateWay });
    S2::config->append({ Bci::PORT, sizeof(Com_param.Port), &Com_param.Port });
    S2::config->append({ Bci::SNTP, sizeof(Com_param.SNTP), &Com_param.SNTP });
    S2::config->append({ Bci::BAUD, sizeof(Com_param.Baud), &Com_param.Baud });
    S2::config->append({ Bci::PARITY, sizeof(Com_param.Parity), &Com_param.Parity });
    S2::config->append({ Bci::STOPBIT, sizeof(Com_param.Stopbit), &Com_param.Stopbit });
    S2::config->append({ Bci::ADRMB, sizeof(Com_param.adrMB), &Com_param.adrMB });

    S2::config->append({ 0xFFFFFFFF, 0, nullptr });
}

void ConfigKxx::SetDefConf()
{
    TempConf = Bci::TempConfStruct();
    StrTrele = Bci::StructTrele();
    StrModBus = Bci::StructModBus();
    Com_param = Bci::Com();
}

void ConfigKxx::Fill()
{
    int i, cbidx;
    quint16 tmp16;
    //    quint8 tmp;
    //....................................................
    WDFunc::SetSPBData(ParentSetupBl, "RTerm", TempConf.RTerm);
    WDFunc::SetSPBData(ParentSetupBl, "W100", TempConf.W100);
    WDFunc::SetSPBData(ParentSetupBl, "Trele_pred", StrTrele.Trele_pred);
    WDFunc::SetSPBData(ParentSetupBl, "Trele_alarm", StrTrele.Trele_alarm);

    //.................................................................

    cbidx = ((StrModBus.MBMaster & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(ParentMB, "MBMaster", cbidx);

    if (StrModBus.MBMab1[0] > 4)
        StrModBus.MBMab1[0] = 0;
    if (StrModBus.MBMab2[0] > 4)
        StrModBus.MBMab2[0] = 0;
    if (StrModBus.MBMab3[0] > 4)
        StrModBus.MBMab3[0] = 0;
    if (StrModBus.MBMab4[0] > 4)
        StrModBus.MBMab4[0] = 0;

    WDFunc::SetCBIndex(ParentMB, "MBMab1[0]", StrModBus.MBMab1[0]);
    WDFunc::SetCBIndex(ParentMB, "MBMab2[0]", StrModBus.MBMab2[0]);
    WDFunc::SetCBIndex(ParentMB, "MBMab3[0]", StrModBus.MBMab3[0]);
    WDFunc::SetCBIndex(ParentMB, "MBMab4[0]", StrModBus.MBMab4[0]);

    WDFunc::SetCBIndex(ParentMB, "MBMab1sk[1]", StrModBus.MBMab1[1] & 0x0F);
    WDFunc::SetCBIndex(ParentMB, "MBMab2sk[1]", StrModBus.MBMab2[1] & 0x0F);
    WDFunc::SetCBIndex(ParentMB, "MBMab3sk[1]", StrModBus.MBMab3[1] & 0x0F);
    WDFunc::SetCBIndex(ParentMB, "MBMab4sk[1]", StrModBus.MBMab4[1] & 0x0F);

    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab1ch[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab2ch[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab3ch[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab4ch[1]", cbidx);

    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab1bt[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab2bt[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab3bt[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab4bt[1]", cbidx);

    WDFunc::SetSPBData(ParentMB, "MBMab1per[2]", StrModBus.MBMab1[2]);
    WDFunc::SetSPBData(ParentMB, "MBMab2per[2]", StrModBus.MBMab2[2]);
    WDFunc::SetSPBData(ParentMB, "MBMab3per[2]", StrModBus.MBMab3[2]);
    WDFunc::SetSPBData(ParentMB, "MBMab4per[2]", StrModBus.MBMab4[2]);

    WDFunc::SetSPBData(ParentMB, "MBMab1adr[3]", StrModBus.MBMab1[3]);
    WDFunc::SetSPBData(ParentMB, "MBMab2adr[3]", StrModBus.MBMab2[3]);
    WDFunc::SetSPBData(ParentMB, "MBMab3adr[3]", StrModBus.MBMab3[3]);
    WDFunc::SetSPBData(ParentMB, "MBMab4adr[3]", StrModBus.MBMab4[3]);

    for (i = 1; i < 5; i++)
    {
        if (i == 1)
            cbidx = StrModBus.MBMab1[4] & 0x0F;
        if (i == 2)
            cbidx = StrModBus.MBMab2[4] & 0x0F;
        if (i == 3)
            cbidx = StrModBus.MBMab3[4] & 0x0F;
        if (i == 4)
            cbidx = StrModBus.MBMab4[4] & 0x0F;

        switch (cbidx)
        {
        case 1:
            cbidx = 0;
            break;
        case 3:
            cbidx = 1;
            break;
        case 4:
            cbidx = 2;
            break;
        default:
            cbidx = -1;
        }
        WDFunc::SetCBIndex(ParentMB, "MBMab" + QString::number(i) + "func[4]", cbidx);
    }

    for (i = 1; i < 5; i++)
    {
        if (i == 1)
            cbidx = (StrModBus.MBMab1[4] & 0xF0) >> 4;
        if (i == 2)
            cbidx = (StrModBus.MBMab2[4] & 0xF0) >> 4;
        if (i == 3)
            cbidx = (StrModBus.MBMab3[4] & 0xF0) >> 4;
        if (i == 4)
            cbidx = (StrModBus.MBMab4[4] & 0xF0) >> 4;

        switch (cbidx)
        {
        case 0:
            cbidx = 0;
            break;
        case 1:
            cbidx = 1;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            cbidx = 2;
            break;
        default:
            cbidx = -1;
        }
        WDFunc::SetCBIndex(ParentMB, "MBMab" + QString::number(i) + "tdat[4]", cbidx);
    }

    tmp16 = *((quint16 *)&StrModBus.MBMab1[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab1reg[5]", tmp16);
    tmp16 = *((quint16 *)&StrModBus.MBMab2[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab2reg[5]", tmp16);
    tmp16 = *((quint16 *)&StrModBus.MBMab3[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab3reg[5]", tmp16);
    tmp16 = *((quint16 *)&StrModBus.MBMab4[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab4reg[5]", tmp16);

    //.................................................

    QString StrIP, StrMask, StrSNTP, StrGate;

    for (i = 0; i < 4; i++)
    {

        if (i == 3)
        {
            StrIP.append(QString::number(Com_param.IP[i]));
            StrMask.append(QString::number(Com_param.Mask[i]));
            StrGate.append(QString::number(Com_param.GateWay[i]));
            StrSNTP.append(QString::number(Com_param.SNTP[i]));
        }
        else
        {
            StrIP.append(QString::number(Com_param.IP[i]) + ".");
            StrMask.append(QString::number(Com_param.Mask[i]) + ".");
            StrGate.append(QString::number(Com_param.GateWay[i]) + ".");
            StrSNTP.append(QString::number(Com_param.SNTP[i]) + ".");
        }
    }

    WDFunc::SetSPBData(ParentSetup, "Port_ID", Com_param.Port[0]);

    WDFunc::SetLEData(ParentSetup, "IP_ID", StrIP);
    WDFunc::SetLEData(ParentSetup, "SNTP_ID", StrSNTP);
    WDFunc::SetLEData(ParentSetup, "GW_ID", StrGate);
    WDFunc::SetLEData(ParentSetup, "Mask_ID", StrMask);

    for (int i = 0; i < 8; i++)
    {
        if (Com_param.Baud == m_baudList.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(ParentSetup, "Baud_ID", cbidx);

    if (Com_param.Parity > 2)
        cbidx = 0;
    else
        cbidx = Com_param.Parity;

    WDFunc::SetCBIndex(ParentSetup, "Parity_ID", cbidx);
    cbidx = ((Com_param.Stopbit & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(ParentSetup, "Stopbit_ID", cbidx);

    WDFunc::SetSPBData(ParentSetup, "adrMB_ID", Com_param.adrMB);
}

void ConfigKxx::FillBack()
{
    int i, cbidx;
    quint16 tmp16;
    quint8 tmp = 0;
    //.......................................................................

    WDFunc::SPBData(ParentSetupBl, "RTerm", TempConf.RTerm);
    WDFunc::SPBData(ParentSetupBl, "W100", TempConf.W100);
    WDFunc::SPBData(ParentSetupBl, "Trele_pred", StrTrele.Trele_pred);
    WDFunc::SPBData(ParentSetupBl, "Trele_alarm", StrTrele.Trele_alarm);

    //..................................................................
    cbidx = WDFunc::CBIndex(ParentMB, "MBMaster");
    StrModBus.MBMaster = cbidx;

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1[0]");
    StrModBus.MBMab1[0] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2[0]");
    StrModBus.MBMab2[0] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3[0]");
    StrModBus.MBMab3[0] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4[0]");
    StrModBus.MBMab4[0] = cbidx;

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1sk[1]");
    StrModBus.MBMab1[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1ch[1]");
    StrModBus.MBMab1[1] = StrModBus.MBMab1[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1bt[1]");
    StrModBus.MBMab1[1] = StrModBus.MBMab1[1] | (cbidx << 7);

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2sk[1]");
    StrModBus.MBMab2[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2ch[1]");
    StrModBus.MBMab2[1] = StrModBus.MBMab2[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2bt[1]");
    StrModBus.MBMab2[1] = StrModBus.MBMab2[1] | (cbidx << 7);

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3sk[1]");
    StrModBus.MBMab3[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3ch[1]");
    StrModBus.MBMab3[1] = StrModBus.MBMab3[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3bt[1]");
    StrModBus.MBMab3[1] = StrModBus.MBMab3[1] | (cbidx << 7);

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4sk[1]");
    StrModBus.MBMab4[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4ch[1]");
    StrModBus.MBMab4[1] = StrModBus.MBMab4[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4bt[1]");
    StrModBus.MBMab4[1] = StrModBus.MBMab4[1] | (cbidx << 7);

    WDFunc::SPBData(ParentMB, "MBMab1per[2]", StrModBus.MBMab1[2]);
    WDFunc::SPBData(ParentMB, "MBMab2per[2]", StrModBus.MBMab2[2]);
    WDFunc::SPBData(ParentMB, "MBMab3per[2]", StrModBus.MBMab3[2]);
    WDFunc::SPBData(ParentMB, "MBMab4per[2]", StrModBus.MBMab4[2]);

    WDFunc::SPBData(ParentMB, "MBMab1adr[3]", StrModBus.MBMab1[3]);
    WDFunc::SPBData(ParentMB, "MBMab2adr[3]", StrModBus.MBMab2[3]);
    WDFunc::SPBData(ParentMB, "MBMab3adr[3]", StrModBus.MBMab3[3]);
    WDFunc::SPBData(ParentMB, "MBMab4adr[3]", StrModBus.MBMab4[3]);

    for (i = 1; i < 5; i++)
    {
        cbidx = WDFunc::CBIndex(ParentMB, "MBMab" + QString::number(i) + "func[4]");

        switch (cbidx)
        {
        case 0:
            tmp = 1;
            break;
        case 1:
            tmp = 3;
            break;
        case 2:
            tmp = 4;
            break;
        case -1:
            tmp = 0;
            break;
        }
        if (i == 1)
            StrModBus.MBMab1[4] = tmp;
        if (i == 2)
            StrModBus.MBMab2[4] = tmp;
        if (i == 3)
            StrModBus.MBMab3[4] = tmp;
        if (i == 4)
            StrModBus.MBMab4[4] = tmp;
    }

    for (i = 1; i < 5; i++)
    {
        cbidx = WDFunc::CBIndex(ParentMB, "MBMab" + QString::number(i) + "tdat[4]");

        switch (cbidx)
        {
        case 0:
            tmp = 0;

            break;
        case 1:
            tmp = 1;
            break;
        case 2:
            tmp = 4;
            break;
        case -1:
            tmp = 0;
            break;
        }

        if (i == 1)
            StrModBus.MBMab1[4] = StrModBus.MBMab1[4] | (tmp << 4);
        if (i == 2)
            StrModBus.MBMab2[4] = StrModBus.MBMab2[4] | (tmp << 4);
        if (i == 3)
            StrModBus.MBMab3[4] = StrModBus.MBMab3[4] | (tmp << 4);
        if (i == 4)
            StrModBus.MBMab4[4] = StrModBus.MBMab4[4] | (tmp << 4);
    }

    WDFunc::SPBData(ParentMB, "MBMab1reg[5]", tmp16);
    *((quint16 *)&StrModBus.MBMab1[5]) = tmp16;
    WDFunc::SPBData(ParentMB, "MBMab2reg[5]", tmp16);
    *((quint16 *)&StrModBus.MBMab2[5]) = tmp16;
    *((quint16 *)&StrModBus.MBMab3[5]) = tmp16;
    WDFunc::SPBData(ParentMB, "MBMab3reg[5]", tmp16);
    *((quint16 *)&StrModBus.MBMab4[5]) = tmp16;
    WDFunc::SPBData(ParentMB, "MBMab4reg[5]", tmp16);

    //................................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QString NameIP = "IP_ID", NameMask = "Mask_ID", NameSNTP = "SNTP_ID", NameGate = "GW_ID";
    QStringList inIP, inMask, inSNTP, inGate;

    //    WDFunc::LE_read_data(ParentSetup, NameIP, StrIP);
    //    WDFunc::LE_read_data(ParentSetup, NameSNTP, StrSNTP);
    //    WDFunc::LE_read_data(ParentSetup, NameGate, StrGate);
    //    WDFunc::LE_read_data(ParentSetup, NameMask, StrMask);
    StrIP = WDFunc::LEData(ParentSetup, NameIP);
    StrSNTP = WDFunc::LEData(ParentSetup, NameSNTP);
    StrGate = WDFunc::LEData(ParentSetup, NameGate);
    StrMask = WDFunc::LEData(ParentSetup, NameMask);

    inIP.append(StrIP.split("."));
    inMask.append(StrMask.split("."));
    inSNTP.append(StrSNTP.split("."));
    inGate.append(StrGate.split("."));

    for (i = 0; i < 4; i++)
    {
        Com_param.IP[i] = inIP.at(i).toInt();
        Com_param.Mask[i] = inMask.at(i).toInt();
        Com_param.GateWay[i] = inGate.at(i).toInt();
        Com_param.SNTP[i] = inSNTP.at(i).toInt();
    }

    WDFunc::SPBData(ParentSetup, "Port_ID" + QString::number(0), Com_param.Port[0]);

    cbidx = WDFunc::CBIndex(ParentSetup, "Baud_ID");
    Com_param.Baud = m_baudList.at(cbidx).toInt();
    cbidx = WDFunc::CBIndex(ParentSetup, "Parity_ID");
    Com_param.Parity = cbidx;
    cbidx = WDFunc::CBIndex(ParentSetup, "Stopbit_ID");
    Com_param.Stopbit = cbidx;

    WDFunc::SPBData(ParentSetup, "adrMB_ID", Com_param.adrMB);
}

QWidget *ConfigKxx::ComParam(QWidget *parent)
{
    ParentSetup = parent;
    QWidget *w = new QWidget;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString Str;

    glyout->setColumnStretch(2, 50);

    int i = 0;
    int row = 7;
    for (i = 0; i < 4; i++)
    {
        Com_param.IP[i] = 0;
        if (i == 3)
            Str.append(QString::number(Com_param.IP[i]));
        else
            Str.append(QString::number(Com_param.IP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL2(parent, "IP адрес устройства:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE2(parent, "IP_ID", Str), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        Com_param.Mask[i] = 0;
        if (i == 3)
            Str.append(QString::number(Com_param.Mask[i]));
        else
            Str.append(QString::number(Com_param.Mask[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL2(parent, "Маска:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE2(parent, "Mask_ID", Str), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        Com_param.GateWay[i] = 0;
        if (i == 3)
            Str.append(QString::number(Com_param.GateWay[i]));
        else
            Str.append(QString::number(Com_param.GateWay[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL2(parent, "Шлюз:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE2(parent, "GW_ID", Str), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Порт протокола 104:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB2(parent, "Port_ID", 0, 10000, 0), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        Com_param.SNTP[i] = 0;
        if (i == 3)
            Str.append(QString(Com_param.SNTP[i]));
        else
            Str.append(QString(Com_param.SNTP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес SNTP сервера:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE2(parent, "SNTP_ID", Str), row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    vlyout1->addLayout(vlyout2);

    w->setLayout(vlyout1);

    return w;
}

QWidget *ConfigKxx::ModbusWidget(QWidget *parent)
{
    ParentMB = parent;
    QWidget *w = new QWidget;
    QStackedWidget *qswt = new QStackedWidget;
    qswt->setObjectName("qswt");
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;

    QStringList cbl;
    QString Str;
    glyout->setColumnStretch(1, 20);

    int row = 0;

    QLabel *lbl = new QLabel("Modbus: ");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList dopcbl = QStringList { "slave", "master" };
    auto *dopcb = WDFunc::NewCB2(parent, "MBMaster", dopcbl /*, paramcolor*/);

    connect(dopcb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConfigKxx::ChangeModbusGUI);
    glyout->addWidget(dopcb, row, 1, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройки ModBus");

    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    lbl = WDFunc::NewLBL2(parent, "Скорость RS485 интерфейса:");
    glyout->addWidget(lbl, row, 0, 1, 1);
    auto *cb = WDFunc::NewCB2(parent, "Baud_ID", m_baudList /*, paramcolor*/);

    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Чётность:"), row, 0, 1, 1);
    cbl = QStringList { "NoParity", "EvenParity", "OddParity" };
    cb = WDFunc::NewCB2(parent, "Parity_ID", cbl /*, paramcolor*/);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Количество стоповых битов:"), row, 0, 1, 1);
    cbl = QStringList { "1", "2" };
    cb = WDFunc::NewCB2(parent, "Stopbit_ID", cbl /*, paramcolor*/);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL2(parent, "Адрес устройства для Modbus:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(parent, "adrMB_ID", 1, 254, 0), row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);
    WidgetList.append(gb);

    gb = new QGroupBox("Настройки ModBus");

    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    QLabel *line1 = new QLabel(parent);

    line1->setText("тип датчика");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 1, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("");
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 1, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("параметры связи");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 0, 2, 1, 3);

    line1 = new QLabel(parent);
    line1->setText("");
    // line1->setStyleSheet("QLabel {border: 1px solid darkgreen;}");
    glyout->addWidget(line1, 0, 5, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("");
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 0, 6, 1, 1);
    line1 = new QLabel(parent);
    line1->setText("");
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 7, 1, 1);
    line1 = new QLabel(parent);
    line1->setText("");
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 8, 1, 1);
    line1 = new QLabel(parent);
    line1->setText("");
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 9, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("скорость");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid darkgreen;}");
    glyout->addWidget(line1, 1, 2, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("чётность");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 3, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("стопБиты");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 4, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("период опроса");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 5, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("адрес абонента");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 6, 1, 1);
    line1 = new QLabel(parent);

    line1->setText("функция");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 7, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("тип данных");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 8, 1, 1);

    line1 = new QLabel(parent);
    line1->setText("адрес регистра");
    line1->setAlignment(Qt::AlignCenter);
    // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 9, 1, 1);

    int j = 1;
    for (int i = 1; i < 5; i++)
    {

        j++;
        line1 = new QLabel(parent);
        line1->setText("Датчик " + QString::number(i) + ":");
        line1->setAlignment(Qt::AlignCenter);
        // line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
        glyout->addWidget(line1, j, 0, 1, 1);

        cbl = QStringList { "нет", "тип 1", "тип 2", "тип 3" };
        cb = WDFunc::NewCB2(parent, "MBMab" + QString::number(i) + "[0]", cbl /*, paramcolor*/);
        glyout->addWidget(cb, j, 1, 1, 1);

        cb = WDFunc::NewCB2(parent, "MBMab" + QString::number(i) + "sk[1]", m_baudList /*, paramcolor*/);
        glyout->addWidget(cb, j, 2, 1, 1);

        cbl = QStringList { "нет", "even", "odd" };
        cb = WDFunc::NewCB2(parent, "MBMab" + QString::number(i) + "ch[1]", cbl /*, paramcolor*/);
        glyout->addWidget(cb, j, 3, 1, 1);

        cbl = QStringList { "1", "2" };
        cb = WDFunc::NewCB2(parent, "MBMab" + QString::number(i) + "bt[1]", cbl /*, paramcolor*/);
        glyout->addWidget(cb, j, 4, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "per[2]";
        glyout->addWidget(WDFunc::NewSPB2(parent, QString(Str), 0, 10000, 0), ++i, 5, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "adr[3]";
        glyout->addWidget(WDFunc::NewSPB2(parent, QString(Str), 0, 10000, 0), ++i, 6, 1, 1);
    }

    j = 1;
    for (int i = 1; i < 5; i++)
    {
        j++;
        cbl = QStringList { "Coils", "Holding", "Input" };
        cb = WDFunc::NewCB2(parent, "MBMab" + QString::number(i) + "func[4]", cbl /*, paramcolor*/);
        glyout->addWidget(cb, j, 7, 1, 1);

        cbl = QStringList { "Word", "Int", "float" };
        cb = WDFunc::NewCB2(parent, "MBMab" + QString::number(i) + "tdat[4]", cbl /*, paramcolor*/);
        glyout->addWidget(cb, j, 8, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "reg[5]";
        glyout->addWidget(WDFunc::NewSPB2(parent, QString(Str), 0, 10000, 0), ++i, 9, 1, 1);
    }

    glyout->addWidget(line1, 7, 0, 1, 1);
    glyout->addWidget(line1, 8, 0, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);

    WidgetList.append(gb);

    vlyout1->addWidget(qswt);

    w->setLayout(vlyout1);

    return w;
}

QWidget *ConfigKxx::VariousWidget(QWidget *parent)
{
    ParentSetupBl = parent;
    int row = 0;
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox();
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Номинальное сопротивление термометра при 0 град.С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(parent, "RTerm", 0, 10000, 0), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Температурный коэффициент термометра:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(parent, "W100", 0, 10000, 3), row, 2, 1, 3);
    row++;

    glyout->addWidget(
        WDFunc::NewLBL2(parent, "Задержка срабатывания реле предупредительной сигнализации:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(parent, "Trele_pred", 0, 10000, 0), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL2(parent, "Задержка срабатывания реле аварийной сигнализации:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB2(parent, "Trele_alarm", 0, 10000, 0), row, 2, 1, 3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    w->setLayout(vlyout1);

    return w;
}

void ConfigKxx::ChangeModbusGUI(int num)
{
    StrModBus.MBMaster = num;

    QStackedWidget *QSWT = ParentMB->findChild<QStackedWidget *>("qswt");

    if (QSWT != nullptr)
        QSWT->setCurrentWidget(WidgetList.at(num));
}
