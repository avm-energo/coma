#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "../modbus/serialport.h"
#include "../iec104/iec104.h"

#define MAXREGISTRYINTERFACECOUNT   5 // how much entries can we have for interfaces of each type in registry

class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    struct ConnectStruct
    {
//        int type; // USB, Ethernet, RS485
        IEC104::Settings iec104st; // IEC104 settings
        SerialPort::Settings serialst;
    };

    ConnectDialog();

signals:
    void Accepted(ConnectStruct &);
    void Cancelled();

private slots:
    void ParseString(QString str);
    void ParseInter();
    void SetInterface();

private:
    ConnectStruct Connect;

};

#endif // CONNECTDIALOG_H
