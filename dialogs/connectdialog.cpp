#include <QVBoxLayout>
#include <QSettings>

#include "connectdialog.h"
#include "../gen/maindef.h"
#include "../gen/error.h"
#include "../models/etablemodel.h"
#include "../usb/usb.h"
#include "../widgets/emessagebox.h"
#include "../widgets/wd_func.h"

ConnectDialog::ConnectDialog()
{
    QByteArray ba;
    QStringList intersl = QStringList() << "USB" << "Ethernet" << "RS485";
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Выберите интерфейс связи"));
    lyout->addWidget(WDFunc::NewCB(this, "intercb", intersl));
    WDFunc::CBConnect(this, "intercb", WDFunc::CT_TEXTCHANGED,this,SLOT(ParseInter(QString)));
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, &QPushButton::clicked,this,&ConnectDialog::SetInterface);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, &QPushButton::clicked,this,&ConnectDialog::Cancelled);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

/*void ConnectDialog::ParseString(QString str)
{
    QStringList sl = Str.split(" ");

    if (sl.size() < 3)
    {
        EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        ERMSG("Некорректная запись в файле");
        return;
    }
    if ((sl.at(1) == "ETH") && (MainInterface == I_ETHERNET))
    {
       FullName = sl.at(0);
       AdrBaseStation = sl.at(2).toUShort();
       IPtemp = Str.split(" ").last();
    }
    else if ((sl.at(1) == "MODBUS") && (MainInterface == I_RS485))
    {
        if(sl.size() > 6)
        {
            bool ok;
            FullName = sl.at(0);
            Settings.Baud =  sl.at(2).toUInt(&ok);
            if (!ok)
            {
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                ERMSG("Некорректная запись в файле");
                return;
            }
            Settings.Parity =  sl.at(3);
            Settings.Stop =  sl.at(4);
            Settings.Address =  sl.at(5).toUInt(&ok);
            if (!ok)
            {
                ERMSG("Некорректная запись в файле");
                EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
                return;
            }
            Settings.Port =  sl.at(6);
        }
        else
        {
            EMessageBox::information(this, "Ошибка", "Некорректная запись в файле");
        }
    }
} */

void ConnectDialog::ParseInter()
{
    QString tmps = WDFunc::CBData(this, "intercb");
    if (tmps == "USB")
        MainInterface = I_USB;
    else if (tmps == "Ethernet")
        MainInterface = I_ETHERNET;
    else if (tmps == "RS485")
        MainInterface = I_RS485;
    else
        MainInterface = I_UNKNOWN;
}

void ConnectDialog::SetInterface()
{
    QStringList ethlist, rslist;
    QDialog *dlg = new QDialog;
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;

    QSettings *sets = new QSettings (SOFTDEVELOPER, PROGNAME);
    for (int i=0; i<MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString rsname = "RS485-" + QString::number(i);
        QString ethname = "Ethernet-" + QString::number(i);
        ethlist << sets->value(ethname, "").toString();
        rslist << sets->value(rsname, "").toString();
    }
    if (MainInterface == I_USB)
    {
        QStringList USBsl = cn->DevicesFound();
        QStringList sl = QStringList() << "#" << "Device";
        ETableModel *mdl = new ETableModel;

        if (USBsl.size() == 0)
        {
            lyout->addWidget(WDFunc::NewLBL(this, "Ошибка, устройства не найдены"));
            Error::ShowErMsg(CN_NOPORTSERROR);
            return;
        }
        mdl->SetHeaders(sl);
        for (int i=0; i<USBsl.size(); ++i)
        {
            QVector<QVariant> vl;
            vl << QString::number(i+1) << USBsl.at(i);
            mdl->AddRowWithData(vl);
        }
        lyout->addWidget(WDFunc::NewTV(dlg, "usbtv", mdl));
    }
    else if (MainInterface == I_ETHERNET)
    {
        QStringList sl = QStringList() << "#" << "Name" << "IP" << "BS address";
        ETableModel *mdl = new ETableModel;

        mdl->SetHeaders(sl);
        for (int i=0; i<ethlist.size(); ++i)
        {
            QVector<QVariant> vl;
            QString key = PROGNAME;
            key += "\\" + ethlist.at(i);
            sets = new QSettings(SOFTDEVELOPER, key);
            vl << QString::number(i+1) << sets->value("name", "") << sets->value("ip", "") << sets->value("bs", "");
            mdl->AddRowWithData(vl);
        }
        lyout->addWidget(WDFunc::NewTV(dlg, "ethtv", mdl));
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addStretch(10);
        hlyout->addWidget(WDFunc::NewPB(dlg, "newethpb", "Добавить", this, SLOT(AddEth())));
        hlyout->addWidget(WDFunc::NewPB(dlg, "scanethpb", "Сканировать", this, SLOT(ScanEth())));
        hlyout->addStretch(10);
        lyout->addLayout(hlyout);
    }
    else // RS485
    {
        QStringList sl = QStringList() << "#" << "Name" << "Port" << "Speed" << "Parity" << "Stop bits" << "Address";
        ETableModel *mdl = new ETableModel;

        mdl->SetHeaders(sl);
        for (int i=0; i<rslist.size(); ++i)
        {
            QVector<QVariant> vl;
            QString key = PROGNAME;
            key += "\\" + rslist.at(i);
            sets = new QSettings(SOFTDEVELOPER, key);
            vl << QString::number(i+1) << sets->value("name", "") << sets->value("port", "") << sets->value("speed", "") <<
                  sets->value("parity", "") << sets->value("stop", "") << sets->value("address", "");
            mdl->AddRowWithData(vl);
        }
        lyout->addWidget(WDFunc::NewTV(dlg, "rstv", mdl));
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addStretch(10);
        hlyout->addWidget(WDFunc::NewPB(dlg, "newrspb", "Добавить", this, SLOT(AddRs())));
        hlyout->addWidget(WDFunc::NewPB(dlg, "scanrspb", "Сканировать", this, SLOT(ScanRs())));
        hlyout->addStretch(10);
        lyout->addLayout(hlyout);
    }
    QPushButton *pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)),this,SLOT(SetCancelled()));
    lyout->addStretch(20);
    lyout->addWidget(pb);
    lyout->addStretch(20);
    dlg->setLayout(lyout);
    WDFunc::TVConnect(dlg, "usbtv", WDFunc::CT_DCLICKED,this,SLOT(SetUsb()));
    WDFunc::TVConnect(dlg, "rstv", WDFunc::CT_DCLICKED, this, SLOT(SetRs()));
    WDFunc::TVConnect(dlg, "ethtv", WDFunc::CT_DCLICKED, this, SLOT(SetEth()));
    dlg->exec();
}

void ConnectDialog::SetUsb()
{
    ConnectStruct st; // temporary var
    QDialog *dlg = this->findChild<QDialog *>("connectdlg");
    if (dlg != nullptr)
        cn->TranslateDeviceAndSave(WDFunc::TVData(dlg, "usbtv", 1).toString());
    emit Accepted(st);
}

void ConnectDialog::AddEth()
{
    QDialog *dlg = new QDialog;
    dlg->setObjectName("ethdlg");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *lyout = new QGridLayout;
    int count = 0;
    lyout->addWidget(WDFunc::NewLBL(dlg, "Имя:"), count, 0, 1, 1, Qt::AlignLeft);
    lyout->addWidget(WDFunc::NewLE(dlg, "namele"), count++, 1, 1, 7);
    lyout->addWidget(WDFunc::NewLBL(dlg, "IP:"), count, 0, 1, 1, Qt::AlignLeft);
    for (int i=0; i<4; ++i)
    {
        lyout->addWidget(WDFunc::NewLE(dlg, "iple."+QString::number(i)), count, (i*2+1), 1, 1);
        if (i != 3)
            lyout->addWidget(WDFunc::NewLBL(dlg, "."), count, (i*2+2), 1, 1);
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
            EMessageBox::error(this, "Ошибка", "Такое имя уже имеется");
            return;
        }
        QString ipstr = WDFunc::LEData(dlg, "iple.0") + "." + WDFunc::LEData(dlg, "iple.1") + "." + WDFunc::LEData(dlg, "iple.2") + "." + WDFunc::LEData(dlg, "iple.3");
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
            WDFunc::TVAutoResize(dlg2, "ethtv");
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
    emit Accepted(st);
}

void ConnectDialog::RotateSettings(const QString &type, const QString &name)
{
    QSettings *sets = new QSettings (SOFTDEVELOPER, PROGNAME);
    QStringList sl;
    QString namename, oldnamename;
    // 1. get all type+'i' from registry (count)
    for (int i=0; i<MAXREGISTRYINTERFACECOUNT; ++i)
    {
        namename = type + QString::number(i);
        QString value = sets->value(namename, "").toString();
        if (!value.isEmpty())
            sl << value;
    }
    // 2. if count < 5 then return count
    if (sl.size() < MAXREGISTRYINTERFACECOUNT)
    {
        namename = type + QString::number(sl.size());
        sets->setValue(namename, name);
        return;
    }
    // 3. else delete group "type + sl.size()-1"
    namename = type + QString::number(sl.size()-1);
    sets->remove(sets->value(namename, "").toString());
    // and rotate it (1->0, 2->1 etc)
    for (int i=(sl.size()-1); i>0; --i)
    {
        oldnamename = type + QString::number(i);
        namename = type + QString::number(i-1);
        sets->setValue(oldnamename, sets->value(namename, ""));
    }
    namename = type + "0";
    sets->setValue(namename, name);
}

bool ConnectDialog::IsKeyExist(const QString &type, const QString &chstr)
{
    QSettings *sets = new QSettings (SOFTDEVELOPER, PROGNAME);
    for (int i=0; i<MAXREGISTRYINTERFACECOUNT; ++i)
    {
        QString key = type+QString::number(i);
        if (sets->value(key, "").toString() == chstr)
            return true;
    }
    return false;
}
