#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QStringListModel>
#include <QCoreApplication>
#include <QTime>
#include "eusbcom.h"
#include "../widgets/messagebox.h"

EUsbCom::EUsbCom(QObject *parent) : EAbstractProtocomChannel(parent)
{
    FirstPass = true;
}

EUsbCom::~EUsbCom()
{
}

bool EUsbCom::Connect()
{
    while ((!Connected) && (!Cancelled))
    {
        pc.PrbMessage = "Загрузка данных...";

        QSerialPortInfo info;
        if (!SetPort(pc.Port, info))
        {
            if (FirstPass)
                FirstPass = false;
            else
            {
                MessageBox2::error(this, "Ошибка", "Порт не найден");
                pc.ErMsg(USB_COMER);
            }
            ShowConnectDialog();
            continue;
        }
        int baud = 115200;
        if (!InitializePort(info, baud))
        {
            if (FirstPass)
                FirstPass = false;
            else
                MessageBox2::error(this, "Ошибка", "Связь не может быть установлена");
            ShowConnectDialog();
            continue;
        }
    }
    if (Cancelled)
        return false;
    return true;
}

QByteArray EUsbCom::RawRead(int bytes)
{
    return port->read(bytes);
}

qint64 EUsbCom::RawWrite(QByteArray &ba)
{
    return port->write(ba);
}

void EUsbCom::RawClose()
{
    if (!Connected)
        return;
    try
    {
        Connected = false;
        if (port->isOpen())
            port->close();
    }
    catch(...)
    {
        DBGMSG;
    }
}

bool EUsbCom::InitializePort(QSerialPortInfo &pinfo, int baud)
{
    port = new QSerialPort;
    port->setPort(pinfo);
    port->clearError();
    connect(port,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(Error(QSerialPort::SerialPortError)));
    QTimer *OpenTimeoutTimer = new QTimer;
    OpenTimeoutTimer->setInterval(2000);
    connect(OpenTimeoutTimer,SIGNAL(timeout()),this,SLOT(Disconnect()));
    OpenTimeoutTimer->start();
    if (!port->open(QIODevice::ReadWrite))
    {
        OpenTimeoutTimer->stop();
        return false;
    }
    port->setBaudRate(baud);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setStopBits(QSerialPort::OneStop);
    connect(port,SIGNAL(readyRead()),this,SLOT(CheckForData()));
    Connected = true;
    OpenTimeoutTimer->stop();
    return true;
}

void EUsbCom::ShowConnectDialog()
{
    int i;
    QDialog *dlg = new QDialog(this);
    dlg->setMinimumWidth(150);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("connectdlg");
    QVBoxLayout *lyout = new QVBoxLayout;
    if (info.size() == 0)
    {
        QLabel *lbl = new QLabel("Ошибка, в системе нет последовательных портов");
        lyout->addWidget(lbl);
        pc.ErMsg(USB_NOCOMER);
    }
    else
    {
        QComboBox *portscb = new QComboBox;
        connect(portscb,SIGNAL(currentIndexChanged(QString)),this,SLOT(SetPortSlot(QString)));
        QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts();
        QStringListModel *tmpmodel = new QStringListModel;
        QStringList tmpsl;
        for (i = 0; i < info.size(); i++)
            tmpsl << info.at(i).portName();
        tmpmodel->setStringList(tmpsl);
        portscb->setModel(tmpmodel);
        lyout->addWidget(portscb);
    }
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, SIGNAL(clicked(bool)),dlg,SLOT(close()));
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked(bool)),this, SLOT(SetCancelled()));
    connect(pb, SIGNAL(clicked(bool)),dlg, SLOT(close()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->exec();
}

bool EUsbCom::SetPort(const QString &port, QSerialPortInfo &info)
{
    QList<QSerialPortInfo> infolist = QSerialPortInfo::availablePorts();
    if (infolist.size() == 0)
    {
        MessageBox2::error(this, "Ошибка", "В системе нет последовательных портов");
        pc.ErMsg(USB_NOCOMER);
        return false;
    }
    for (int i = 0; i < infolist.size(); i++)
    {
        if (infolist.at(i).portName() == port)
        {
            pc.Port = port;
            info = infolist.at(i);
            return true;
        }
    }
    return false;
}

void EUsbCom::Error(QSerialPort::SerialPortError err)
{
    if (!err) // нет ошибок
        return;
    quint16 ernum = err + COM_ERROR;
    pc.ErMsg(ernum);
    if (Connected)
        Disconnect();
}

void EUsbCom::SetCancelled()
{
    Cancelled = true;
}

void EUsbCom::SetPortSlot(QString port)
{
    pc.Port = port;
}
