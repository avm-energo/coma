#include "confdialogkxx.h"

ConfDialogKxx::ConfDialogKxx(QVector<S2::DataRec> *S2Config, QWidget *parent) : QWidget(parent)
{
    Kxx = new ConfigKxx(S2Config);

    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(Colors::ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
    // QString tmps = "QWidget {background-color: " + QString(Colors::ACONFWCLR) + ";}";
}

QWidget *ConfDialogKxx::SetupComParam(QObject *parent)
{
    ParentSetup = parent;
    QWidget *w = new QWidget;
    QString paramcolor = Colors::MAINWINCLR;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    QString Str;
    QLocale german(QLocale::German);
    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);
    glyout->setColumnStretch(2, 50);

    int i = 0;
    int row = 7;
    for (i = 0; i < 4; i++)
    {
        Kxx->Com_param.IP[i] = 0;
        if (i == 3)
            Str.append(german.toString(Kxx->Com_param.IP[i]));
        else
            Str.append(german.toString(Kxx->Com_param.IP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL(this, "IP адрес устройства:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE(this, "IP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        Kxx->Com_param.Mask[i] = 0;
        if (i == 3)
            Str.append(german.toString(Kxx->Com_param.Mask[i]));
        else
            Str.append(german.toString(Kxx->Com_param.Mask[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Маска:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE(this, "Mask_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        Kxx->Com_param.GateWay[i] = 0;
        if (i == 3)
            Str.append(german.toString(Kxx->Com_param.GateWay[i]));
        else
            Str.append(german.toString(Kxx->Com_param.GateWay[i]) + ".");
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Шлюз:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE(this, "GW_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Порт протокола 104:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewSPB(this, "Port_ID", 0, 10000, 0, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    row++;
    Str.clear();
    for (i = 0; i < 4; i++)
    {
        Kxx->Com_param.SNTP[i] = 0;
        if (i == 3)
            Str.append(german.toString(Kxx->Com_param.SNTP[i]));
        else
            Str.append(german.toString(Kxx->Com_param.SNTP[i]) + ".");
    }

    glyout->addWidget(WDFunc::NewLBL(this, "Адрес SNTP сервера:"), row, 0, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLE(this, "SNTP_ID", Str, paramcolor), row, 1, 1, 1, Qt::AlignLeft);

    vlyout2->addLayout(glyout);
    vlyout1->addLayout(vlyout2);

    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialogKxx::SetupModBus(QObject *parent)
{
    ParentMB = parent;
    QWidget *w = new QWidget;
    QString paramcolor = Colors::MAINWINCLR;
    QStackedWidget *qswt = new QStackedWidget;
    qswt->setObjectName("qswt");
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;
    EComboBox *cb = new EComboBox;
    QStringList cbl;
    QString Str;
    glyout->setColumnStretch(1, 20);

    QFont font;
    font.setFamily("Times");
    font.setPointSize(11);

    int row = 0;

    QLabel *lbl = new QLabel("Modbus: ");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList dopcbl = QStringList { "slave", "master" };
    EComboBox *dopcb = WDFunc::NewCB(this, "MBMaster", dopcbl, paramcolor);
    connect(dopcb, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeWindow(int)));
    glyout->addWidget(dopcb, row, 1, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройки ModBus");
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    lbl = WDFunc::NewLBL(this, "Скорость RS485 интерфейса:");
    glyout->addWidget(lbl, row, 0, 1, 1);
    cbl = QStringList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    cb = WDFunc::NewCB(this, "Baud_ID", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Чётность:"), row, 0, 1, 1);
    cbl = QStringList { "NoParity", "EvenParity", "OddParity" };
    cb = WDFunc::NewCB(this, "Parity_ID", cbl, paramcolor);
    glyout->addWidget(cb, row, 1, 1, 1, Qt::AlignLeft);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Количество стоповых битов:"), row, 0, 1, 1);
    cbl = QStringList { "1", "2" };
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
    gb->setFont(font);
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;
    QLabel *line1 = new QLabel();
    line1 = new QLabel(this);

    line1->setText("тип датчика");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 1, 1, 1);

    line1 = new QLabel(this);
    line1->setText("");
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 1, 1, 1);

    line1 = new QLabel(this);
    line1->setText("параметры связи");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 0, 2, 1, 3);

    line1 = new QLabel(this);
    line1->setText("");
    line1->setStyleSheet("QLabel {border: 1px solid darkgreen;}");
    glyout->addWidget(line1, 0, 5, 1, 1);

    line1 = new QLabel(this);
    line1->setText("");
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 0, 6, 1, 1);
    line1 = new QLabel(this);
    line1->setText("");
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 7, 1, 1);
    line1 = new QLabel(this);
    line1->setText("");
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 8, 1, 1);
    line1 = new QLabel(this);
    line1->setText("");
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen ;}");
    glyout->addWidget(line1, 0, 9, 1, 1);

    line1 = new QLabel(this);
    line1->setText("скорость");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid darkgreen;}");
    glyout->addWidget(line1, 1, 2, 1, 1);

    line1 = new QLabel(this);
    line1->setText("чётность");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 3, 1, 1);

    line1 = new QLabel(this);
    line1->setText("стопБиты");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 4, 1, 1);

    line1 = new QLabel(this);
    line1->setText("переуд опроса");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 5, 1, 1);

    line1 = new QLabel(this);
    line1->setText("адрес абонента");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 6, 1, 1);
    line1 = new QLabel(this);

    line1 = new QLabel(this);
    line1->setText("функция");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 7, 1, 1);

    line1 = new QLabel(this);
    line1->setText("тип данных");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 8, 1, 1);

    line1 = new QLabel(this);
    line1->setText("адрес регистра");
    line1->setAlignment(Qt::AlignCenter);
    line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
    glyout->addWidget(line1, 1, 9, 1, 1);

    int j = 1;
    for (int i = 1; i < 5; i++)
    {

        j++;
        line1 = new QLabel(this);
        line1->setText("Датчик " + QString::number(i) + ":");
        line1->setAlignment(Qt::AlignCenter);
        line1->setStyleSheet("QLabel {border: 1px solid  darkgreen;}");
        glyout->addWidget(line1, j, 0, 1, 1);

        cbl = QStringList { "нет", "тип 1", "тип 2", "тип 3" };
        cb = WDFunc::NewCB(this, "MBMab" + QString::number(i) + "[0]", cbl, paramcolor);
        glyout->addWidget(cb, j, 1, 1, 1);

        cbl = QStringList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
        cb = WDFunc::NewCB(this, "MBMab" + QString::number(i) + "sk[1]", cbl, paramcolor);
        glyout->addWidget(cb, j, 2, 1, 1);

        cbl = QStringList { "нет", "even", "odd" };
        cb = WDFunc::NewCB(this, "MBMab" + QString::number(i) + "ch[1]", cbl, paramcolor);
        glyout->addWidget(cb, j, 3, 1, 1);

        cbl = QStringList { "1", "2" };
        cb = WDFunc::NewCB(this, "MBMab" + QString::number(i) + "bt[1]", cbl, paramcolor);
        glyout->addWidget(cb, j, 4, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "per[2]";
        glyout->addWidget(WDFunc::NewSPB(this, QString(Str), 0, 10000, 0, paramcolor), ++i, 5, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "adr[3]";
        glyout->addWidget(WDFunc::NewSPB(this, QString(Str), 0, 10000, 0, paramcolor), ++i, 6, 1, 1);
    }

    j = 1;
    for (int i = 1; i < 5; i++)
    {
        j++;
        cbl = QStringList { "Coils", "Holding", "Input" };
        cb = WDFunc::NewCB(this, "MBMab" + QString::number(i) + "func[4]", cbl, paramcolor);
        glyout->addWidget(cb, j, 7, 1, 1);

        cbl = QStringList { "Word", "Int", "float" };
        cb = WDFunc::NewCB(this, "MBMab" + QString::number(i) + "tdat[4]", cbl, paramcolor);
        glyout->addWidget(cb, j, 8, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "reg[5]";
        glyout->addWidget(WDFunc::NewSPB(this, QString(Str), 0, 10000, 0, paramcolor), ++i, 9, 1, 1);
    }

    line1 = new QLabel(this);
    line1->setText("");

    glyout->addWidget(line1, 7, 0, 1, 1);
    glyout->addWidget(line1, 8, 0, 1, 1);
    glyout->addWidget(line1, 9, 0, 1, 1);

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);

    WidgetList.append(gb);

    vlyout1->addWidget(qswt);

    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialogKxx::SetupBl(QObject *parent)
{
    ParentSetupBl = parent;
    int row = 0;
    QString paramcolor = Colors::MAINWINCLR;
    QWidget *w = new QWidget;
    QGroupBox *gb = new QGroupBox();
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
    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

void ConfDialogKxx::ChangeWindow(int num)
{
    Kxx->StrModBus.MBMaster = num;

    QStackedWidget *QSWT = ParentMB->findChild<QStackedWidget *>("qswt");

    if (QSWT != nullptr)
        QSWT->setCurrentWidget(WidgetList.at(num));
}

void ConfDialogKxx::Fill()
{
    int i, cbidx;
    quint16 tmp16;
    //    quint8 tmp;
    //....................................................
    WDFunc::SetSPBData(ParentSetupBl, "RTerm", Kxx->TempConf.RTerm);
    WDFunc::SetSPBData(ParentSetupBl, "W100", Kxx->TempConf.W100);
    WDFunc::SetSPBData(ParentSetupBl, "Trele_pred", Kxx->StrTrele.Trele_pred);
    WDFunc::SetSPBData(ParentSetupBl, "Trele_alarm", Kxx->StrTrele.Trele_alarm);

    //.................................................................

    cbidx = ((Kxx->StrModBus.MBMaster & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(ParentMB, "MBMaster", cbidx);

    if (Kxx->StrModBus.MBMab1[0] > 4)
        Kxx->StrModBus.MBMab1[0] = 0;
    if (Kxx->StrModBus.MBMab2[0] > 4)
        Kxx->StrModBus.MBMab2[0] = 0;
    if (Kxx->StrModBus.MBMab3[0] > 4)
        Kxx->StrModBus.MBMab3[0] = 0;
    if (Kxx->StrModBus.MBMab4[0] > 4)
        Kxx->StrModBus.MBMab4[0] = 0;

    WDFunc::SetCBIndex(ParentMB, "MBMab1[0]", Kxx->StrModBus.MBMab1[0]);
    WDFunc::SetCBIndex(ParentMB, "MBMab2[0]", Kxx->StrModBus.MBMab2[0]);
    WDFunc::SetCBIndex(ParentMB, "MBMab3[0]", Kxx->StrModBus.MBMab3[0]);
    WDFunc::SetCBIndex(ParentMB, "MBMab4[0]", Kxx->StrModBus.MBMab4[0]);

    WDFunc::SetCBIndex(ParentMB, "MBMab1sk[1]", Kxx->StrModBus.MBMab1[1] & 0x0F);
    WDFunc::SetCBIndex(ParentMB, "MBMab2sk[1]", Kxx->StrModBus.MBMab2[1] & 0x0F);
    WDFunc::SetCBIndex(ParentMB, "MBMab3sk[1]", Kxx->StrModBus.MBMab3[1] & 0x0F);
    WDFunc::SetCBIndex(ParentMB, "MBMab4sk[1]", Kxx->StrModBus.MBMab4[1] & 0x0F);

    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab1ch[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab2ch[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab3ch[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x60) >> 5;
    WDFunc::SetCBIndex(ParentMB, "MBMab4ch[1]", cbidx);

    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab1bt[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab2bt[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab3bt[1]", cbidx);
    cbidx = (Kxx->StrModBus.MBMab1[1] & 0x80) >> 7;
    WDFunc::SetCBIndex(ParentMB, "MBMab4bt[1]", cbidx);

    WDFunc::SetSPBData(ParentMB, "MBMab1per[2]", Kxx->StrModBus.MBMab1[2]);
    WDFunc::SetSPBData(ParentMB, "MBMab2per[2]", Kxx->StrModBus.MBMab2[2]);
    WDFunc::SetSPBData(ParentMB, "MBMab3per[2]", Kxx->StrModBus.MBMab3[2]);
    WDFunc::SetSPBData(ParentMB, "MBMab4per[2]", Kxx->StrModBus.MBMab4[2]);

    WDFunc::SetSPBData(ParentMB, "MBMab1adr[3]", Kxx->StrModBus.MBMab1[3]);
    WDFunc::SetSPBData(ParentMB, "MBMab2adr[3]", Kxx->StrModBus.MBMab2[3]);
    WDFunc::SetSPBData(ParentMB, "MBMab3adr[3]", Kxx->StrModBus.MBMab3[3]);
    WDFunc::SetSPBData(ParentMB, "MBMab4adr[3]", Kxx->StrModBus.MBMab4[3]);

    for (i = 1; i < 5; i++)
    {
        if (i == 1)
            cbidx = Kxx->StrModBus.MBMab1[4] & 0x0F;
        if (i == 2)
            cbidx = Kxx->StrModBus.MBMab2[4] & 0x0F;
        if (i == 3)
            cbidx = Kxx->StrModBus.MBMab3[4] & 0x0F;
        if (i == 4)
            cbidx = Kxx->StrModBus.MBMab4[4] & 0x0F;

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
            cbidx = (Kxx->StrModBus.MBMab1[4] & 0xF0) >> 4;
        if (i == 2)
            cbidx = (Kxx->StrModBus.MBMab1[4] & 0xF0) >> 4;
        if (i == 3)
            cbidx = (Kxx->StrModBus.MBMab1[4] & 0xF0) >> 4;
        if (i == 4)
            cbidx = (Kxx->StrModBus.MBMab1[4] & 0xF0) >> 4;

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

    tmp16 = *((quint16 *)&Kxx->StrModBus.MBMab1[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab1reg[5]", tmp16);
    tmp16 = *((quint16 *)&Kxx->StrModBus.MBMab2[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab2reg[5]", tmp16);
    tmp16 = *((quint16 *)&Kxx->StrModBus.MBMab3[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab3reg[5]", tmp16);
    tmp16 = *((quint16 *)&Kxx->StrModBus.MBMab4[5]);
    WDFunc::SetSPBData(ParentMB, "MBMab4reg[5]", tmp16);

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

    WDFunc::SetSPBData(ParentSetup, "Port_ID", Kxx->Com_param.Port[0]);

    WDFunc::LE_write_data(ParentSetup, StrIP, "IP_ID");
    WDFunc::LE_write_data(ParentSetup, StrSNTP, "SNTP_ID");
    WDFunc::LE_write_data(ParentSetup, StrGate, "GW_ID");
    WDFunc::LE_write_data(ParentSetup, StrMask, "Mask_ID");

    for (int i = 0; i < 8; i++)
    {
        if (Kxx->Com_param.Baud == Sbaud.at(i).toUInt())
            cbidx = i;
    }
    WDFunc::SetCBIndex(ParentSetup, "Baud_ID", cbidx);

    if (Kxx->Com_param.Parity > 2)
        cbidx = 0;
    else
        cbidx = Kxx->Com_param.Parity;

    WDFunc::SetCBIndex(ParentSetup, "Parity_ID", cbidx);
    cbidx = ((Kxx->Com_param.Stopbit & 0x01) ? 1 : 0);
    WDFunc::SetCBIndex(ParentSetup, "Stopbit_ID", cbidx);

    WDFunc::SetSPBData(ParentSetup, "adrMB_ID", Kxx->Com_param.adrMB);
}

void ConfDialogKxx::FillBack()
{
    int i, cbidx;
    quint16 tmp16;
    quint8 tmp;
    //.......................................................................

    WDFunc::SPBData(ParentSetupBl, "RTerm", Kxx->TempConf.RTerm);
    WDFunc::SPBData(ParentSetupBl, "W100", Kxx->TempConf.W100);
    WDFunc::SPBData(ParentSetupBl, "Trele_pred", Kxx->StrTrele.Trele_pred);
    WDFunc::SPBData(ParentSetupBl, "Trele_alarm", Kxx->StrTrele.Trele_alarm);

    //..................................................................
    cbidx = WDFunc::CBIndex(ParentMB, "MBMaster");
    Kxx->StrModBus.MBMaster = cbidx;

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1[0]");
    Kxx->StrModBus.MBMab1[0] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2[0]");
    Kxx->StrModBus.MBMab2[0] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3[0]");
    Kxx->StrModBus.MBMab3[0] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4[0]");
    Kxx->StrModBus.MBMab4[0] = cbidx;

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1sk[1]");
    Kxx->StrModBus.MBMab1[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1ch[1]");
    Kxx->StrModBus.MBMab1[1] = Kxx->StrModBus.MBMab1[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab1bt[1]");
    Kxx->StrModBus.MBMab1[1] = Kxx->StrModBus.MBMab1[1] | (cbidx << 7);

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2sk[1]");
    Kxx->StrModBus.MBMab2[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2ch[1]");
    Kxx->StrModBus.MBMab2[1] = Kxx->StrModBus.MBMab2[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab2bt[1]");
    Kxx->StrModBus.MBMab2[1] = Kxx->StrModBus.MBMab2[1] | (cbidx << 7);

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3sk[1]");
    Kxx->StrModBus.MBMab3[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3ch[1]");
    Kxx->StrModBus.MBMab3[1] = Kxx->StrModBus.MBMab3[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab3bt[1]");
    Kxx->StrModBus.MBMab3[1] = Kxx->StrModBus.MBMab3[1] | (cbidx << 7);

    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4sk[1]");
    Kxx->StrModBus.MBMab4[1] = cbidx;
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4ch[1]");
    Kxx->StrModBus.MBMab4[1] = Kxx->StrModBus.MBMab4[1] | (cbidx << 5);
    cbidx = WDFunc::CBIndex(ParentMB, "MBMab4bt[1]");
    Kxx->StrModBus.MBMab4[1] = Kxx->StrModBus.MBMab4[1] | (cbidx << 7);

    WDFunc::SPBData(ParentMB, "MBMab1per[2]", Kxx->StrModBus.MBMab1[2]);
    WDFunc::SPBData(ParentMB, "MBMab2per[2]", Kxx->StrModBus.MBMab2[2]);
    WDFunc::SPBData(ParentMB, "MBMab3per[2]", Kxx->StrModBus.MBMab3[2]);
    WDFunc::SPBData(ParentMB, "MBMab4per[2]", Kxx->StrModBus.MBMab4[2]);

    WDFunc::SPBData(ParentMB, "MBMab1adr[3]", Kxx->StrModBus.MBMab1[3]);
    WDFunc::SPBData(ParentMB, "MBMab2adr[3]", Kxx->StrModBus.MBMab2[3]);
    WDFunc::SPBData(ParentMB, "MBMab3adr[3]", Kxx->StrModBus.MBMab3[3]);
    WDFunc::SPBData(ParentMB, "MBMab4adr[3]", Kxx->StrModBus.MBMab4[3]);

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
            Kxx->StrModBus.MBMab1[4] = tmp;
        if (i == 2)
            Kxx->StrModBus.MBMab2[4] = tmp;
        if (i == 3)
            Kxx->StrModBus.MBMab3[4] = tmp;
        if (i == 4)
            Kxx->StrModBus.MBMab4[4] = tmp;
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
        case 3:
            tmp = 4;
            break;
        case -1:
            tmp = 0;
            break;
        }

        if (i == 1)
            Kxx->StrModBus.MBMab1[4] = Kxx->StrModBus.MBMab1[4] | (tmp << 4);
        if (i == 2)
            Kxx->StrModBus.MBMab2[4] = Kxx->StrModBus.MBMab2[4] | (tmp << 4);
        if (i == 3)
            Kxx->StrModBus.MBMab3[4] = Kxx->StrModBus.MBMab3[4] | (tmp << 4);
        if (i == 4)
            Kxx->StrModBus.MBMab4[4] = Kxx->StrModBus.MBMab4[4] | (tmp << 4);
    }

    WDFunc::SPBData(ParentMB, "MBMab1reg[5]", tmp16);
    *((quint16 *)&Kxx->StrModBus.MBMab1[5]) = tmp16;
    WDFunc::SPBData(ParentMB, "MBMab2reg[5]", tmp16);
    *((quint16 *)&Kxx->StrModBus.MBMab2[5]) = tmp16;
    *((quint16 *)&Kxx->StrModBus.MBMab3[5]) = tmp16;
    WDFunc::SPBData(ParentMB, "MBMab3reg[5]", tmp16);
    *((quint16 *)&Kxx->StrModBus.MBMab4[5]) = tmp16;
    WDFunc::SPBData(ParentMB, "MBMab4reg[5]", tmp16);

    //................................................................

    QString StrIP, StrMask, StrSNTP, StrGate;
    QString NameIP = "IP_ID", NameMask = "Mask_ID", NameSNTP = "SNTP_ID", NameGate = "GW_ID";
    QStringList inIP, inMask, inSNTP, inGate;
    QLocale german(QLocale::German);

    WDFunc::LE_read_data(ParentSetup, NameIP, StrIP);
    WDFunc::LE_read_data(ParentSetup, NameSNTP, StrSNTP);
    WDFunc::LE_read_data(ParentSetup, NameGate, StrGate);
    WDFunc::LE_read_data(ParentSetup, NameMask, StrMask);

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

    WDFunc::SPBData(ParentSetup, "Port_ID" + QString::number(0), Kxx->Com_param.Port[0]);

    cbidx = WDFunc::CBIndex(ParentSetup, "Baud_ID");
    Kxx->Com_param.Baud = (Sbaud.at(cbidx).toInt());
    cbidx = WDFunc::CBIndex(ParentSetup, "Parity_ID");
    Kxx->Com_param.Parity = cbidx;
    cbidx = WDFunc::CBIndex(ParentSetup, "Stopbit_ID");
    Kxx->Com_param.Stopbit = cbidx;

    WDFunc::SPBData(ParentSetup, "adrMB_ID", Kxx->Com_param.adrMB);
}

void ConfDialogKxx::SetDefConf()
{
    Kxx->SetDefConf();
    //  Fill();
}
