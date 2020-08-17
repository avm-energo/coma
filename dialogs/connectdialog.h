#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include "../iec104/iec104.h"
#include "../modbus/serialport.h"

#include <QDialog>

#define MAXREGISTRYINTERFACECOUNT 5 // how much entries can we have for interfaces of each type in registry

class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    struct ConnectStruct
    {
        QString name;
        IEC104::Settings iec104st; // IEC104 settings
        SerialPort::Settings serialst;
    };

    ConnectDialog();

signals:
    void Accepted(ConnectDialog::ConnectStruct *);
    void Cancelled();

private slots:
    void ParseInter();
    void SetInterface();
    void SetUsb();
    void AddEth();
    void EthAccepted();
    void SetCancelled();
    void SetEth();
    /*    void ScanEth();
        void AddRs();
        void SetRs();
        void ScanRs(); */
    void RotateSettings(
        const QString &type, const QString &name); // in: name of registry dir without index, out - name with index
    bool IsKeyExist(const QString &type, const QString &chstr);
    bool UpdateModel();

private:
    ConnectStruct Connect;
};

#endif // CONNECTDIALOG_H
