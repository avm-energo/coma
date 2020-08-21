#include "confdialogkxx.h"

ConfDialogKxx::ConfDialogKxx(QVector<S2::DataRec> *S2Config, QWidget *parent) : QWidget(parent)
{
    Kxx = new ConfigKxx(S2Config);

    ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; "
                   "padding: 1px; color: black;"
                   "background-color: "
        + QString(ACONFOCLR) + "; font: bold 10px;}";
    WidgetFormat = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
    // QString tmps = "QWidget {background-color: " + QString(ACONFWCLR) + ";}";
}

QWidget *ConfDialogKxx::SetupComParam(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QString paramcolor = MAINWINCLR;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
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
    //    gb->setLayout(vlyout2);
    //    vlyout1->addWidget(gb);

    vlyout1->addLayout(vlyout2);

    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialogKxx::SetupModBus(QObject *parent)
{
    QswtParent = parent;
    QWidget *w = new QWidget;
    QString paramcolor = MAINWINCLR;
    QStackedWidget *qswt = new QStackedWidget;
    qswt->setObjectName("qswt");
    QVBoxLayout *vlyout1 = new QVBoxLayout;
    QVBoxLayout *vlyout2 = new QVBoxLayout;
    QGroupBox *gb = new QGroupBox;
    QGridLayout *glyout = new QGridLayout;
    EComboBox *сb = new EComboBox;
    QStringList cbl;
    QString Str;
    glyout->setColumnStretch(1, 20);

    int row = 0;

    QLabel *lbl = new QLabel("Modbus: ");
    glyout->addWidget(lbl, row, 0, 1, 1, Qt::AlignLeft);
    QStringList dopcbl = QStringList() << "slave"
                                       << "master";
    EComboBox *dopcb = WDFunc::NewCB(this, "MBMaster", dopcbl, paramcolor);
    connect(dopcb, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeWindow(int)));
    glyout->addWidget(dopcb, row, 1, 1, 1);
    row++;

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    vlyout1->addWidget(gb);

    gb = new QGroupBox("Настройки ModBus");
    vlyout2 = new QVBoxLayout;
    glyout = new QGridLayout;

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Скорость RS485 интерфейса:"), row, 0, 1, 1);
    cbl = QStringList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    EComboBox *cb = WDFunc::NewCB(this, "Baud_ID", cbl, paramcolor);
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

    for (int i = 1; i < 5;)
    {
        Str = "Датчик " + i;
        Str = Str + ":";
        line1 = new QLineEdit(this);
        line1->setText(QString(Str));
        glyout->addWidget(line1, ++i, 0, 1, 1);
    }

    cbl = QStringList { "нет", "тип 1", "тип 2", "тип 3" };
    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + QString::number(i);
        Str = Str + "[0]";
        cb = WDFunc::NewCB(this, QString(Str), cbl, paramcolor);
        glyout->addWidget(сb, ++i, 1, 1, 1);
    }

    cbl = QStringList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + i;
        Str = Str + "sk[1]";
        //        сb = WDFunc::NewCB(this, QString("MBMab" + "sk[1]"), cbl, paramcolor);
        сb = WDFunc::NewCB(this, Str, cbl, paramcolor);
        glyout->addWidget(сb, ++i, 2, 1, 1);
    }

    cbl = QStringList { "нет", "even", "old" };
    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + i;
        Str = Str + "ch[1]";
        сb = WDFunc::NewCB(this, QString(Str), cbl, paramcolor);
        glyout->addWidget(сb, ++i, 3, 1, 1);
    }

    cbl = QStringList { "1", "2" };
    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + i;
        Str = Str + "bt[1]";
        сb = WDFunc::NewCB(this, QString(Str), cbl, paramcolor);
        glyout->addWidget(сb, ++i, 4, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + i;
        Str = Str + "per[2]";
        glyout->addWidget(WDFunc::NewSPB(this, QString(Str), 0, 10000, 0, paramcolor), ++i, 5, 1, 1);
    }

    for (int i = 1; i < 5;)
    {
        Str = "MBMab" + i;
        Str = Str + "adr[3]";
        glyout->addWidget(WDFunc::NewSPB(this, QString(Str), 0, 10000, 0, paramcolor), ++i, 6, 1, 1);
    }

    vlyout2->addLayout(glyout);
    gb->setLayout(vlyout2);
    qswt->addWidget(gb);

    WidgetList.append(gb);

    vlyout1->addWidget(qswt);

    w->setLayout(vlyout1);
    w->setStyleSheet(WidgetFormat);
    return w;
}

QWidget *ConfDialogKxx::SetupBl(QWidget *parent)
{
    int row = 0;
    QString paramcolor = MAINWINCLR;
    QWidget *w = new QWidget(parent);
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

    QStackedWidget *QSWT = QswtParent->findChild<QStackedWidget *>("qswt");

    if (QSWT != nullptr)
        QSWT->setCurrentWidget(WidgetList.at(num));
}
