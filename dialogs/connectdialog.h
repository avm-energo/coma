#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include "../iec104/iec104.h"
#include "../modbus/serialport.h"

#include <QDialog>
#include <QModelIndex>
#include <QProgressDialog>

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

    explicit ConnectDialog();

signals:
    void Accepted(ConnectDialog::ConnectStruct *);
    void Cancelled();
    void PingFinished();

private slots:
    void SetInterface();
    void SetUsb(QModelIndex index);
    void AddEth();
    void EthAccepted();
    void RsAccepted();
    void SetCancelled();
    void SetEth();
    void SetEth(QModelIndex index);
    void ScanEth();
    void AddRs();
    void SetRs();
    void SetRs(QModelIndex index);

    //    void ScanRs();
    void RotateSettings(
        const QString &type, const QString &name); // in: name of registry dir without index, out - name with index
    bool IsKeyExist(const QString &type, const QString &chstr);
    bool UpdateModel();

private:
    ConnectStruct Connect;

    QList<quint32> m_hosts;

    /// Кандидаты для отдельного класса
    QProgressDialog *m_progress;
    void handlePing();
    void handlePingFinish();
    void handlePortFinish();
    void createPingTask(quint32 ip);
    void createPortTask();
    ///
};

#endif // CONNECTDIALOG_H
