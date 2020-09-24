#include "connectdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../usb/eprotocom.h"
#include "../widgets/wd_func.h"

#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QVBoxLayout>

ConnectDialog::ConnectDialog()
{
    QStringList intersl { "USB", "Ethernet", "RS485" };
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));
    lyout->addWidget(WDFunc::NewCB(this, "intercb", intersl));
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, &QPushButton::clicked, this, &ConnectDialog::SetInterface);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, &QPushButton::clicked, this, &ConnectDialog::Cancelled);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void ConnectDialog::SetInterface()
{
    auto comboBox = this->findChild<EComboBox *>();
    Board::GetInstance()->setProperty("interface", comboBox->currentText());
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    dlg->setMinimumWidth(400);
    QVBoxLayout *lyout = new QVBoxLayout;

    switch (Board::GetInstance()->interfaceType())
    {
    case Board::InterfaceType::USB:
        lyout->addWidget(WDFunc::NewTV(dlg, "usbtv", nullptr));
        break;

    case Board::InterfaceType::Ethernet:
    {
        lyout->addWidget(WDFunc::NewTV(dlg, "ethtv", nullptr));
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addStretch(10);
        hlyout->addWidget(WDFunc::NewPB(dlg, "newethpb", "Добавить", this, SLOT(AddEth())));
        hlyout->addWidget(WDFunc::NewPB(dlg, "scanethpb", "Сканировать", this, SLOT(ScanEth())));
        hlyout->addStretch(10);
        lyout->addLayout(hlyout);
        break;
    }
    case Board::InterfaceType::RS485:
    {
        lyout->addWidget(WDFunc::NewTV(dlg, "rstv", nullptr));
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addStretch(10);
        hlyout->addWidget(WDFunc::NewPB(dlg, "newrspb", "Добавить", this, SLOT(AddRs())));
        hlyout->addWidget(WDFunc::NewPB(dlg, "scanrspb", "Сканировать", this, SLOT(ScanRs())));
        hlyout->addStretch(10);
        lyout->addLayout(hlyout);
        break;
    }
    }
    QPushButton *pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)), this, SLOT(SetCancelled()));
    lyout->addStretch(20);
    lyout->addWidget(pb);
    lyout->addStretch(20);
    dlg->setLayout(lyout);
    WDFunc::TVConnect(dlg, "usbtv", WDFunc::CT_DCLICKED, this, SLOT(SetUsb()));
    WDFunc::TVConnect(dlg, "rstv", WDFunc::CT_DCLICKED, this, SLOT(SetRs()));
    WDFunc::TVConnect(dlg, "ethtv", WDFunc::CT_DCLICKED, this, SLOT(SetEth()));
    UpdateModel();
    dlg->exec();
}

void ConnectDialog::SetUsb()
{
    ConnectStruct st; // temporary var
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != nullptr)
    {
        EProtocom::GetInstance()->setDeviceName(WDFunc::TVData(dlg, "usbtv", 1).toString());
        EProtocom::GetInstance()->TranslateDeviceAndSave(WDFunc::TVData(dlg, "usbtv", 1).toString());
    }
    emit Accepted(&st);
}

void ConnectDialog::AddEth()
{
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("ethdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE(dlg, "namele"), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewLBL(dlg, "IP:"), count, 0, 1, 1, Qt::AlignLeft);
    for (int i = 0; i < 4; ++i)
    {
        lyout->addWidget(WDFunc::NewLE(dlg, "iple." + QString::number(i)), count, (i * 2 + 1), 1, 1);
        if (i != 3)
            lyout->addWidget(WDFunc::NewLBL(dlg, "."), count, (i * 2 + 2), 1, 1);
    }
    ++count;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Адрес БС:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB(dlg, "bsadrspb", 1, 255, 0), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewPB(dlg, "acceptpb", "Сохранить", this, SLOT(EthAccepted())), count, 0, 1, 4);
    lyout->addWidget(WDFunc::NewPB(dlg, "cancelpb", "Отмена", dlg, SLOT(close())), count, 4, 1, 3);
    dlg->setLayout(lyout);
    dlg->exec();
}

void ConnectDialog::EthAccepted()
{
    QDialog *dlg = this->findChild<QDialog *>("ethdlg");
    if (dlg != nullptr)
    {
        QString name = WDFunc::LEData(dlg, "namele");
        // check if there's such name in registry
        if (IsKeyExist("Ethernet-", name))
        {
            QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
            return;
        }
        QString ipstr = WDFunc::LEData(dlg, "iple.0") + "." + WDFunc::LEData(dlg, "iple.1") + "."
            + WDFunc::LEData(dlg, "iple.2") + "." + WDFunc::LEData(dlg, "iple.3");
        RotateSettings("Ethernet-", name);
        QString key = PROGNAME;
        key += "\\" + name;
        QSettings *sets = new QSettings(SOFTDEVELOPER, key);
        sets->setValue("ip", ipstr);
        int spbdata;
        WDFunc::SPBData(dlg, "bsadrspb", spbdata);
        sets->setValue("bs", QString::number(spbdata));
        QDialog *dlg2 = this->findChild<QDialog *>("connectdlg");
        if (dlg2 != nullptr)
            UpdateModel();
        dlg->close();
    }
}

void ConnectDialog::RsAccepted()
{
    QDialog *dlg = this->findChild<QDialog *>("rsdlg");
    if (dlg != nullptr)
    {
        QString name = WDFunc::LEData(dlg, "namele");
        // check if there's such name in registry
        if (IsKeyExist("RS485-", name))
        {
            QMessageBox::critical(this, "Ошибка", "Такое имя уже имеется");
            return;
        }
        RotateSettings("RS485-", name);
        QString key = PROGNAME;
        key += "\\" + name;
        QSettings *sets = new QSettings(SOFTDEVELOPER, key);
        sets->setValue("port", WDFunc::CBData(dlg, "portcb"));
        sets->setValue("speed", WDFunc::CBData(dlg, "speedcb"));
        sets->setValue("parity", WDFunc::CBData(dlg, "paritycb"));
        sets->setValue("stop", WDFunc::CBData(dlg, "stopcb"));
        int spbdata;
        WDFunc::SPBData(dlg, "addressspb", spbdata);
        sets->setValue("address", QString::number(spbdata));
        QDialog *dlg2 = this->findChild<QDialog *>("connectdlg");
        if (dlg2 != nullptr)
            UpdateModel();
        dlg->close();
    }
}

void ConnectDialog::SetCancelled()
{
    emit Cancelled();
}

void ConnectDialog::SetEth()
{
    ConnectStruct st;
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != nullptr)
    {
        st.name = WDFunc::TVData(dlg, "ethtv", 1).toString();
        st.iec104st.ip = WDFunc::TVData(dlg, "ethtv", 2).toString();
        st.iec104st.baseadr = WDFunc::TVData(dlg, "ethtv", 3).toUInt();
    }
    emit Accepted(&st);
}

void ConnectDialog::AddRs()
{
    QStringList ports;
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, portlist)
        ports << info.portName();
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("rsdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE(dlg, "namele"), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL(dlg, "Порт:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB(dlg, "portcb", ports), count++, 1, 1, 1);
    QStringList sl { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    lyout->addWidget(WDFunc::NewLBL(dlg, "Скорость:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB(dlg, "speedcb", sl), count++, 1, 1, 1);
    sl = QStringList() << "Нет"
                       << "Нечет"
                       << "Чет";
    lyout->addWidget(WDFunc::NewLBL(dlg, "Чётность:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewCB(dlg, "paritycb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL(dlg, "Стоп бит:"), count, 0, 1, 1, Qt::AlignLeft);
    sl = QStringList() << "1"
                       << "2";
    lyout->addWidget(WDFunc::NewCB(dlg, "stopbitcb", sl), count++, 1, 1, 1);
    lyout->addWidget(WDFunc::NewLBL(dlg, "Адрес:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewSPB(dlg, "addressspb", 1, 255, 0), count++, 1, 1, 1);
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(dlg, "acceptpb", "Сохранить", this, SLOT(RsAccepted())));
    hlyout->addWidget(WDFunc::NewPB(dlg, "cancelpb", "Отмена", dlg, SLOT(close())));
    lyout->addLayout(hlyout, count, 0, 1, 2, Qt::AlignCenter);
    dlg->setLayout(lyout);
    dlg->exec();
}

void ConnectDialog::SetRs()
{
    ConnectStruct st;
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != nullptr)
    {
        st.name = WDFunc::TVData(dlg, "rstv", 1).toString();
        st.serialst.Port = WDFunc::TVData(dlg, "rstv", 2).toString();
        st.serialst.Baud = WDFunc::TVData(dlg, "rstv", 3).toUInt();
        st.serialst.Parity = WDFunc::TVData(dlg, "rstv", 4).toString();
        st.serialst.Stop = WDFunc::TVData(dlg, "rstv", 5).toString();
        st.serialst.Address = WDFunc::TVData(dlg, "rstv", 6).toUInt();
    }
    emit Accepted(&st);
}

void ConnectDialog::RotateSettings(const QString &type, const QString &name)
{
    QSettings *sets = new QSettings(SOFTDEVELOPER, PROGNAME);
    QStringList sl;
    QString namename, oldnamename;
    // 1. get all type+'i' from registry (count)
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        namename = type + QString::number(i);
        QString value = sets->value(namename, "").toString();
        if (!value.isEmpty())
            sl << value;
    }
    // 2. if count < 5 then return count
    if (sl.size() < MAXREGISTRYINTERFACECOUNT)
        sl.append("");
    else
    {
        // 3. else delete group "type + sl.size()-1"
        namename = type + QString::number(sl.size() - 1);
        sets->remove(sets->value(namename, "").toString());
    }
    // and rotate it (1->0, 2->1 etc)
    for (int i = (sl.size() - 1); i > 0; --i)
    {
        oldnamename = type + QString::number(i);
        namename = type + QString::number(i - 1);
        sets->setValue(oldnamename, sets->value(namename, ""));
    }
    namename = type + "0";
    sets->setValue(namename, name);
}

bool ConnectDialog::IsKeyExist(const QString &type, const QString &chstr)
{
    QSettings *sets = new QSettings(SOFTDEVELOPER, PROGNAME);
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString key = type + QString::number(i);
        if (sets->value(key, "").toString() == chstr)
            return true;
    }
    return false;
}

bool ConnectDialog::UpdateModel()
{
    QStringList ethlist, rslist;

    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (!dlg)
        return false;
    for (int i = 0; i < MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings(SOFTDEVELOPER, PROGNAME));
        QString rsname = "RS485-" + QString::number(i);
        QString ethname = "Ethernet-" + QString::number(i);
        ethlist << sets->value(ethname, "").toString();
        rslist << sets->value(rsname, "").toString();
    }
    switch (Board::GetInstance()->interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        QStringList USBsl = EProtocom::GetInstance()->DevicesFound();
        QStringList sl { "#", "Device" };
        ETableModel *mdl = new ETableModel(dlg);

        if (USBsl.isEmpty())
        {
            QMessageBox::critical(this, "Ошибка", "Устройства не найдены");
            Error::ShowErMsg(Error::Msg::CN_NOPORTSERROR);
            return false;
        }
        mdl->setHeaders(sl);

        for (int i = 0; i < USBsl.size(); ++i)
        {
            QVector<QVariant> vl;
            vl << QString::number(i + 1) << USBsl.at(i);
            mdl->addRowWithData(vl);
        }
        WDFunc::SetTVModel(dlg, "usbtv", mdl);
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        QStringList sl { "#", "Имя", "IP", "Адрес БС" };
        ETableModel *mdl = new ETableModel(dlg);

        mdl->setHeaders(sl);
        for (int i = 0; i < ethlist.size(); ++i)
        {

            QString key = PROGNAME;
            key += "\\" + ethlist.at(i);
            QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings(SOFTDEVELOPER, key));
            QVector<QVariant> vl { QString::number(i + 1), ethlist.at(i), sets->value("ip", ""),
                sets->value("bs", "") };
            mdl->addRowWithData(vl);
        }
        WDFunc::SetTVModel(dlg, "ethtv", mdl);
        break;
    }
    case Board::InterfaceType::RS485:
    {
        QStringList sl { "#", "Имя", "Порт", "Скорость", "Четность", "Стоп бит", "Адрес" };
        ETableModel *mdl = new ETableModel(dlg);

        mdl->setHeaders(sl);
        for (int i = 0; i < rslist.size(); ++i)
        {

            QString key = PROGNAME;
            key += "\\" + rslist.at(i);
            QScopedPointer<QSettings> sets = QScopedPointer<QSettings>(new QSettings(SOFTDEVELOPER, key));
            QVector<QVariant> vl { QString::number(i + 1), rslist.at(i), sets->value("port", ""),
                sets->value("speed", ""), sets->value("parity", ""), sets->value("stop", ""),
                sets->value("address", "") };
            mdl->addRowWithData(vl);
        }
        WDFunc::SetTVModel(dlg, "rstv", mdl);
        break;
    }
    default:
        return false;
    }
    return true;
}
