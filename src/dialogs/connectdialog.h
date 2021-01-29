#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include "../interfaces/settingstypes.h"

#include <QDialog>
#include <QModelIndex>
#include <QProgressDialog>

#define MAXREGISTRYINTERFACECOUNT 5 // how much entries can we have for interfaces of each type in registry

class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void Accepted(ConnectStruct);
    void Cancelled();
    void PingFinished();
    void ModelUpdated();

private slots:
    void SetInterface();
    void SetUsb(QModelIndex index);
    void AddEth();
    void EthAccepted();
    void RsAccepted();
    // void SetCancelled();
    //    void SetEth();
    void SetEth(QModelIndex index);
    void ScanEth();
    void AddRs();
    //    void SetRs();
    void SetRs(QModelIndex index);

    //    void ScanRs();
    void RotateSettings(
        const QString &type, const QString &name); // in: name of registry dir without index, out - name with index
    bool IsKeyExist(const QString &type, const QString &chstr);
    bool UpdateModel(QDialog *dlg);

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
