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
#include <QStackedWidget>
#include <QVBoxLayout>

ConfDialog::ConfDialog(QVector<S2::DataRec> *S2Config, quint32 MTypeB, quint32 MTypeM, QWidget *parent)
    : QWidget(parent)
{
    ConfigMain = new Config(S2Config, MTypeB, MTypeM); // добавляем к переданному S2Config общую часть
                                                       // SetupUI();

    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(UCONFCLR) + ";}";
}

QWidget *ConfDialog::SetupMainBlk(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QString paramcolor = MAINWINCLR;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;
    QString Str;
    QLocale german(QLocale::German);
    int i = 0;

    gb->setTitle("Настройки протокола МЭК-60870-5-104");
    //   gb->setFont(font);
    glyout = new QGridLayout;
    glyout->setColumnStretch(2, 50);
    QLabel *lbl = new QLabel("Адрес базовой станции:");
    glyout->addWidget(lbl, 0, 0, 1, 1, Qt::AlignLeft);
    QDoubleSpinBox *dspbls = WDFunc::NewSPB(this, "Abs_104", 0, 65535, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 0, 1, 1, 1, Qt::AlignLeft);
    lbl = new QLabel("Интервал циклического опроса, с:");
    glyout->addWidget(lbl, 1, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "Cycle_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 1, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t1, с:");
    glyout->addWidget(lbl, 2, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T1_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 2, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t2, с:");
    glyout->addWidget(lbl, 3, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T2_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 3, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Тайм-аут t3, с:");
    glyout->addWidget(lbl, 4, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "T3_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 4, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Макс. число неподтв. APDU (k):");
    glyout->addWidget(lbl, 5, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "k_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 5, 1, 1, 1, Qt::AlignLeft);

    lbl = new QLabel("Макс. число посл. подтв. APDU (w):");
    glyout->addWidget(lbl, 6, 0, 1, 1, Qt::AlignLeft);
    dspbls = WDFunc::NewSPB(this, "w_104", 0, 255, 0, paramcolor);
    connect(dspbls, SIGNAL(valueChanged(double)), this, SLOT(Set104(double)));
    glyout->addWidget(dspbls, 6, 1, 1, 1, Qt::AlignLeft);

    QStringList cbl = QStringList() << "SNTP+PPS"
                                    << "SNTP";
    EComboBox *cb = WDFunc::NewCB(this, "Ctype", cbl, paramcolor);
    //  cb = WDFunc::NewCB(this, "Ctype", cbl, paramcolor);
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(cb, 7, 1, 1, 1);

    int row = 0;
    for (i = 0; i < 4; i++)
    {
        ConfigMain->Com_param.IP[i] = 0;
        if (i == 3)
            Str.append(german.toString(ConfigMain->Com_param.IP[i]));
        else
            Str.append(german.toString(ConfigMain->Com_param.IP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL(this, "IP адрес устройства:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "IP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        ConfigMain->Com_param.Mask[i] = 0;
        if (i == 3)
            Str.append(german.toString(ConfigMain->Com_param.Mask[i]));
        else
            Str.append(german.toString(ConfigMain->Com_param.Mask[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Маска:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "Mask_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        ConfigMain->Com_param.GateWay[i] = 0;
        if (i == 3)
            Str.append(german.toString(ConfigMain->Com_param.GateWay[i]));
        else
            Str.append(german.toString(ConfigMain->Com_param.GateWay[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Шлюз:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "GW_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Порт протокола 104:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Port_ID", 0, 10000, 0, paramcolor), row, 1, 1, 1);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        ConfigMain->Com_param.SNTP[i] = 0;
        if (i == 3)
            Str.append(german.toString(ConfigMain->Com_param.SNTP[i]));
        else
            Str.append(german.toString(ConfigMain->Com_param.SNTP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL(this, "Адрес SNTP сервера:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewLE(this, "SNTP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    gb->setLayout(glyout);
    vlyout->addWidget(gb);
    // setLayout(vlyout);

    vlyout->addStretch(100);
    w->setLayout(vlyout);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialog::SetupComParam(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QString paramcolor = MAINWINCLR;
    // QVBoxLayout *vlyout = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;

    QStackedWidget *qswt = new QStackedWidget;
    qswt->setObjectName("qswt");
    // int i;
    int row = 0;

    gb = new QGroupBox("Настройки ModBus");
    gb->setObjectName("Gb1");
    // gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость RS485 интерфейса:"), row, 0, 1, 1);
    QStringList cbl = QStringList() << "1200"
                                    << "2400"
                                    << "4800"
                                    << "9600"
                                    << "19200"
                                    << "38400"
                                    << "57600"
                                    << "115200";
    EComboBox *cb = WDFunc::NewCB(this, "Baud_ID", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Чётность:"), row, 0, 1, 1);
    cbl = QStringList() << "NoParity"
                        << "EvenParity"
                        << "OddParity";
    cb = WDFunc::NewCB(this, "Parity_ID", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество стоповых битов:"), row, 0, 1, 1);
    cbl = QStringList() << "1"
                        << "2";
    cb = WDFunc::NewCB(this, "Stopbit_ID", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Адрес устройства для Modbus:"), row, 0, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "adrMB_ID", 1, 254, 0, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);
    WidgetList.append(gb);

    //  <<<<<<<<<<<<<<<<<<<<-

    row = 0;
    gb = new QGroupBox("Настройки ModBus");
    gb->setObjectName("Gb2");
    // gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    QLineEdit *line1 = new QLineEdit();
    line1 = new QLineEdit(this);

    line1->setText("тип датчика:");
    glyout->addWidget(line1, 1, 1, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("           ");
    glyout->addWidget(line1, 0, 1, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("              параметры связи:          ");
    glyout->addWidget(line1, 0, 2, 1, 3);

    line1 = new QLineEdit(this);
    line1->setText("           ");
    glyout->addWidget(line1, 0, 5, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("           ");
    glyout->addWidget(line1, 0, 6, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("скорость:");
    glyout->addWidget(line1, 1, 2, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("чётность:");
    glyout->addWidget(line1, 1, 3, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("стопБиты:");
    glyout->addWidget(line1, 1, 4, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("переод опроса:");
    glyout->addWidget(line1, 1, 5, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("адрес абонента:");
    glyout->addWidget(line1, 1, 6, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("Датчик 1:");
    glyout->addWidget(line1, 2, 0, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("Датчик 2:");
    glyout->addWidget(line1, 3, 0, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("Датчик 3:");
    glyout->addWidget(line1, 4, 0, 1, 1);

    line1 = new QLineEdit(this);
    line1->setText("Датчик 4:");
    glyout->addWidget(line1, 5, 0, 1, 1);

    cbl = QStringList() << "нет"
                        << "тип 1"
                        << "тип 2"
                        << "тип 3";
    EComboBox *сb = WDFunc::NewCB(this, "MBMab1[0]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 2, 1, 1, 1);

    cbl = QStringList() << "нет"
                        << "тип 1"
                        << "тип 2"
                        << "тип 3";
    сb = WDFunc::NewCB(this, "MBMab2[0]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 3, 1, 1, 1);

    cbl = QStringList() << "нет"
                        << "тип 1"
                        << "тип 2"
                        << "тип 3";
    сb = WDFunc::NewCB(this, "MBMab3[0]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 4, 1, 1, 1);

    cbl = QStringList() << "нет"
                        << "тип 1"
                        << "тип 2"
                        << "тип 3";
    сb = WDFunc::NewCB(this, "MBMab4[0]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 5, 1, 1, 1);

    cbl = QStringList() << "1200"
                        << "2400"
                        << "4800"
                        << "9600"
                        << "19200"
                        << "38400"
                        << "57600"
                        << "115200";
    сb = WDFunc::NewCB(this, "MBMab1sk[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 2, 2, 1, 1);

    cbl = QStringList() << "1200"
                        << "2400"
                        << "4800"
                        << "9600"
                        << "19200"
                        << "38400"
                        << "57600"
                        << "115200";
    сb = WDFunc::NewCB(this, "MBMab2sk[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 3, 2, 1, 1);

    cbl = QStringList() << "1200"
                        << "2400"
                        << "4800"
                        << "9600"
                        << "19200"
                        << "38400"
                        << "57600"
                        << "115200";
    сb = WDFunc::NewCB(this, "MBMab3sk[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 4, 2, 1, 1);

    cbl = QStringList() << "1200"
                        << "2400"
                        << "4800"
                        << "9600"
                        << "19200"
                        << "38400"
                        << "57600"
                        << "115200";
    сb = WDFunc::NewCB(this, "MBMab4sk[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 5, 2, 1, 1);

    cbl = QStringList() << "нет"
                        << "even"
                        << "old";
    сb = WDFunc::NewCB(this, "MBMab1ch[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 2, 3, 1, 1);

    cbl = QStringList() << "нет"
                        << "even"
                        << "old";
    сb = WDFunc::NewCB(this, "MBMab2ch[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 3, 3, 1, 1);

    cbl = QStringList() << "нет"
                        << "even"
                        << "old";
    сb = WDFunc::NewCB(this, "MBMab3ch[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 4, 3, 1, 1);

    cbl = QStringList() << "нет"
                        << "even"
                        << "old";
    сb = WDFunc::NewCB(this, "MBMab4ch[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 5, 3, 1, 1);

    cbl = QStringList() << "1"
                        << "2";
    сb = WDFunc::NewCB(this, "MBMab1bt[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 2, 4, 1, 1);

    cbl = QStringList() << "1"
                        << "2";
    сb = WDFunc::NewCB(this, "MBMab2bt[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 3, 4, 1, 1);

    cbl = QStringList() << "1"
                        << "2";
    сb = WDFunc::NewCB(this, "MBMab3bt[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 4, 4, 1, 1);

    cbl = QStringList() << "1"
                        << "2";
    сb = WDFunc::NewCB(this, "MBMab4bt[1]", cbl, paramcolor);
    connect(сb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetCType(int)));
    glyout->addWidget(сb, 5, 4, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab1per[2]", 0, 10000, 0, paramcolor), 2, 5, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab2per[2]", 0, 10000, 0, paramcolor), 3, 5, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab3per[2]", 0, 10000, 0, paramcolor), 4, 5, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab4per[2]", 0, 10000, 0, paramcolor), 5, 5, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab1adr[3]", 0, 10000, 0, paramcolor), 2, 6, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab2adr[3]", 0, 10000, 0, paramcolor), 3, 6, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab3adr[3]", 0, 10000, 0, paramcolor), 4, 6, 1, 1);

    glyout->addWidget(WDFunc::NewSPB(this, "MBMab4adr[3]", 0, 10000, 0, paramcolor), 5, 6, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);

    WidgetList.append(gb);

    vlyout1->addWidget(qswt);

    vlyout1->addStretch(100);
    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialog::SetupBl(QWidget *parent)
{
    int row = 0;
    QString paramcolor = MAINWINCLR;
    QWidget *w = new QWidget(parent);
    QGroupBox *gb = new QGroupBox();
    // gb->setFont(font);
    QGridLayout *glyout = new QGridLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;

    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное сопротивление термометра при 0 град.С:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "RTerm", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Температурный коэффициент термометра:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "W100", 0, 10000, 3, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Задержка срабатывания реле предупредительной сигнализации:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Trele_pred", 0, 10000, 0, paramcolor), row, 2, 1, 3);
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Задержка срабатывания реле аварийной сигнализации:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, "Trele_alarm", 0, 10000, 0, paramcolor), row, 2, 1, 3);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);
    vlyout1->addStretch(100);
    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void ConfDialog::Fill()
{
    int i;
    WDFunc::SetSPBData(this, "Abs_104", ConfigMain->MainBlk.Abs_104);
    WDFunc::SetSPBData(this, "Cycle_104", ConfigMain->MainBlk.Cycle_104);
    WDFunc::SetSPBData(this, "T1_104", ConfigMain->MainBlk.T1_104);
    WDFunc::SetSPBData(this, "T2_104", ConfigMain->MainBlk.T2_104);
    WDFunc::SetSPBData(this, "T3_104", ConfigMain->MainBlk.T3_104);
    WDFunc::SetSPBData(this, "k_104", ConfigMain->MainBlk.k_104);
    WDFunc::SetSPBData(this, "w_104", ConfigMain->MainBlk.w_104);

    int cbidx = ((ConfigMain->MainBlk.Ctype & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Ctype", cbidx);

    //....................................................
    WDFunc::SetSPBData(this, "RTerm", StrD.RTerm);
    WDFunc::SetSPBData(this, "W100", StrD.W100);
    WDFunc::SetSPBData(this, "Trele_pred", StrTrele.Trele_pred);
    WDFunc::SetSPBData(this, "Trele_alarm", StrTrele.Trele_alarm);

    //.................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QLocale german(QLocale::German);

    for (i = 0; i < 4; i++)
    {

        if (i == 3)
        {
            StrIP.append(german.toString(ConfigMain->Com_param.IP[i]));
            StrMask.append(german.toString(ConfigMain->Com_param.Mask[i]));
            StrGate.append(german.toString(ConfigMain->Com_param.GateWay[i]));
            StrSNTP.append(german.toString(ConfigMain->Com_param.SNTP[i]));
        }
        else
        {
            StrIP.append(german.toString(ConfigMain->Com_param.IP[i]) + ".");
            StrMask.append(german.toString(ConfigMain->Com_param.Mask[i]) + ".");
            StrGate.append(german.toString(ConfigMain->Com_param.GateWay[i]) + ".");
            StrSNTP.append(german.toString(ConfigMain->Com_param.SNTP[i]) + ".");
        }
    }

    WDFunc::SetSPBData(this, "Port_ID", ConfigMain->Com_param.Port[0]);

    WDFunc::LE_write_data(this, StrIP, "IP_ID");
    WDFunc::LE_write_data(this, StrSNTP, "SNTP_ID");
    WDFunc::LE_write_data(this, StrGate, "GW_ID");
    WDFunc::LE_write_data(this, StrMask, "Mask_ID");

    // WDFunc::SetSPBData(this, "Baud_ID", KTF->Com_param.Baud_ID);
    for (int i = 0; i < 8; i++)
    {
        if (ConfigMain->Com_param.Baud == Sbaud.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(this, "Baud_ID", cbidx);

    if (ConfigMain->Com_param.Parity > 2)
        cbidx = 0;
    else
        cbidx = ConfigMain->Com_param.Parity;

    WDFunc::SetCBIndex(this, "Parity_ID", cbidx);
    cbidx = ((ConfigMain->Com_param.Stopbit & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "Stopbit_ID", cbidx);

    WDFunc::SetSPBData(this, "adrMB_ID", ConfigMain->Com_param.adrMB);
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

    WDFunc::SPBData(this, "RTerm", StrD.RTerm);
    WDFunc::SPBData(this, "W100", StrD.W100);
    WDFunc::SPBData(this, "Trele_pred", StrTrele.Trele_pred);
    WDFunc::SPBData(this, "Trele_alarm", StrTrele.Trele_alarm);

    cbidx = ((StrModBus.MBMaster & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(this, "MBMaster", cbidx);

    //нет с 1061-1064

    if (StrModBus.MBMab1[0] > 4)
        StrModBus.MBMab1[0] = 0;
    if (StrModBus.MBMab2[0] > 4)
        StrModBus.MBMab2[0] = 0;
    if (StrModBus.MBMab3[0] > 4)
        StrModBus.MBMab3[0] = 0;
    if (StrModBus.MBMab4[0] > 4)
        StrModBus.MBMab4[0] = 0;

    WDFunc::SetCBIndex(this, "MBMab1[0]", StrModBus.MBMab1[0]);
    WDFunc::SetCBIndex(this, "MBMab2[0]", StrModBus.MBMab2[0]);
    WDFunc::SetCBIndex(this, "MBMab3[0]", StrModBus.MBMab3[0]);
    WDFunc::SetCBIndex(this, "MBMab4[0]", StrModBus.MBMab4[0]);

    WDFunc::SetCBIndex(this, "MBMab1sk[1]", StrModBus.MBMab1[1] & 0x0F);
    WDFunc::SetCBIndex(this, "MBMab2sk[1]", StrModBus.MBMab2[1] & 0x0F);
    WDFunc::SetCBIndex(this, "MBMab3sk[1]", StrModBus.MBMab3[1] & 0x0F);
    WDFunc::SetCBIndex(this, "MBMab4sk[1]", StrModBus.MBMab4[1] & 0x0F);

    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab1ch[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab2ch[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab3ch[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(this, "MBMab4ch[1]", cbidx);

    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab1bt[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab2bt[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab3bt[1]", cbidx);
    cbidx = (StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(this, "MBMab4bt[1]", cbidx);

    WDFunc::SetSPBData(this, "MBMab1per[2]", StrModBus.MBMab1[2]);
    WDFunc::SetSPBData(this, "MBMab2per[2]", StrModBus.MBMab2[2]);
    WDFunc::SetSPBData(this, "MBMab3per[2]", StrModBus.MBMab3[2]);
    WDFunc::SetSPBData(this, "MBMab4per[2]", StrModBus.MBMab4[2]);

    WDFunc::SetSPBData(this, "MBMab1adr[3]", StrModBus.MBMab1[3]);
    WDFunc::SetSPBData(this, "MBMab2adr[3]", StrModBus.MBMab2[3]);
    WDFunc::SetSPBData(this, "MBMab3adr[3]", StrModBus.MBMab3[3]);
    WDFunc::SetSPBData(this, "MBMab4adr[3]", StrModBus.MBMab4[3]);

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
        ConfigMain->Com_param.IP[i] = inIP.at(i).toInt();
        ConfigMain->Com_param.Mask[i] = inMask.at(i).toInt();
        ConfigMain->Com_param.GateWay[i] = inGate.at(i).toInt();
        ConfigMain->Com_param.SNTP[i] = inSNTP.at(i).toInt();
    }

    WDFunc::SPBData(this, "Port_ID" + QString::number(0), ConfigMain->Com_param.Port[0]);

    cbidx = WDFunc::CBIndex(this, "Baud_ID");
    ConfigMain->Com_param.Baud = (Sbaud.at(cbidx).toInt());
    cbidx = WDFunc::CBIndex(this, "Parity_ID");
    ConfigMain->Com_param.Parity = cbidx;
    cbidx = WDFunc::CBIndex(this, "Stopbit_ID");
    ConfigMain->Com_param.Stopbit = cbidx;

    WDFunc::SPBData(this, "adrMB_ID", ConfigMain->Com_param.adrMB);
}

void ConfDialog::SetDefConf()
{
    ConfigMain->MainBlk.Abs_104 = DEF_ABS_104;
    ConfigMain->MainBlk.Ctype = DEF_CTYPE;
    ConfigMain->MainBlk.Cycle_104 = DEF_CYCLE_104;
    ConfigMain->MainBlk.k_104 = DEF_K_104;
    ConfigMain->MainBlk.w_104 = DEF_W_104;
    ConfigMain->MainBlk.T1_104 = DEF_T1_104;
    ConfigMain->MainBlk.T2_104 = DEF_T2_104;
    ConfigMain->MainBlk.T3_104 = DEF_T3_104;

    ConfigMain->Com_param.IP[0] = 172;
    ConfigMain->Com_param.IP[1] = 16;
    ConfigMain->Com_param.IP[2] = 29;
    ConfigMain->Com_param.IP[3] = 12;

    ConfigMain->Com_param.Mask[0] = 255;
    ConfigMain->Com_param.Mask[1] = 255;
    ConfigMain->Com_param.Mask[2] = 252;
    ConfigMain->Com_param.Mask[3] = 0;

    ConfigMain->Com_param.GateWay[0] = 172;
    ConfigMain->Com_param.GateWay[1] = 16;
    ConfigMain->Com_param.GateWay[2] = 29;
    ConfigMain->Com_param.GateWay[3] = 1;

    ConfigMain->Com_param.Port[0] = 2404;
    ConfigMain->Com_param.Port[1] = 2405;
    ConfigMain->Com_param.Port[2] = 502;
    ConfigMain->Com_param.Port[3] = 502;

    ConfigMain->Com_param.SNTP[0] = 172;
    ConfigMain->Com_param.SNTP[1] = 16;
    ConfigMain->Com_param.SNTP[2] = 31;
    ConfigMain->Com_param.SNTP[3] = 220;

    ConfigMain->Com_param.Baud = DEF_BAUD;
    ConfigMain->Com_param.Parity = DEF_PARITY;
    ConfigMain->Com_param.Stopbit = DEF_STOPBIT;

    ConfigMain->Com_param.adrMB = DEF_ADRMB;

    Fill();
}

void ConfDialog::Set104(double dbl)
{
    QStringList sl = sender()->objectName().split(".");
    if (sl.size() < 1)
    {
        ERMSG("Некорректные данные");
        DBGMSG;
        return;
    }
    bool ok;
    int wnum = sl.at(1).toInt(&ok);
    if (!ok)
    {
        ERMSG("Некорректные данные");
        DBGMSG;
        return;
    }
    switch (wnum)
    {
    case 1:
    {
        ConfigMain->MainBlk.Abs_104 = dbl;
        break;
    }
    case 2:
    {
        ConfigMain->MainBlk.Cycle_104 = dbl;
        break;
    }
    case 3:
    {
        ConfigMain->MainBlk.T1_104 = dbl;
        break;
    }
    case 4:
    {
        ConfigMain->MainBlk.T2_104 = dbl;
        break;
    }
    case 5:
    {
        ConfigMain->MainBlk.T3_104 = dbl;
        break;
    }
    case 6:
    {
        ConfigMain->MainBlk.k_104 = dbl;
        break;
    }
    case 7:
    {
        ConfigMain->MainBlk.w_104 = dbl;
        break;
    }
    default:
        break;
    }
}

void ConfDialog::SetCType(int num)
{
    ConfigMain->MainBlk.Ctype = num;
}

void ConfDialog::ChangeWindow(int num)
{
    ConfigMain->MainBlk.Ctype = num;

    QStackedWidget *QSWT = this->findChild<QStackedWidget *>("qswt");

    if (QSWT != nullptr)
        QSWT->setCurrentWidget(WidgetList.at(num));
}
