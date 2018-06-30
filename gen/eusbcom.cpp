#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QStringListModel>
#include <QCoreApplication>
#include <QTime>
#include "eusbcom.h"
#include "stdfunc.h"
#include "error.h"
#include "../widgets/emessagebox.h"

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
        StdFunc::SetPrbMessage("Загрузка данных...");

        QSerialPortInfo info;
        if (!SetPort(StdFunc::Port, info))
        {
            if (FirstPass)
                FirstPass = false;
            else
            {
                emit ShowError("Порт не найден");
                Error::ShowErMsg(USB_COMER);
            }
            emit Retry();
            continue;
        }
        int baud = 115200;
        if (!InitializePort(info, baud))
        {
            if (FirstPass)
                FirstPass = false;
            else
                emit ShowError("Связь не может быть установлена");
            emit Retry();
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

bool EUsbCom::SetPort(const QString &port, QSerialPortInfo &info)
{
    QList<QSerialPortInfo> infolist = QSerialPortInfo::availablePorts();
    if (infolist.size() == 0)
    {
        emit ShowError("В системе нет последовательных портов");
        Error::ShowErMsg(USB_NOCOMER);
        return false;
    }
    for (int i = 0; i < infolist.size(); i++)
    {
        if (infolist.at(i).portName() == port)
        {
            StdFunc::Port = port;
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
    Error::ShowErMsg(ernum);
    if (Connected)
        Disconnect();
}
