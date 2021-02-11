#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include "../interfaces/settingstypes.h"
#include "abstractinterfacedialog.h"

#include <QDialog>
#include <QModelIndex>
#include <QProgressDialog>
class ConnectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectDialog(QWidget *parent = nullptr);

signals:
    void accepted(ConnectStruct);
    void cancelled();
    void pingFinished();
    void modelUpdated();

private slots:
    void setInterface();
    void addEth();
    void ethAccepted();
    void rsAccepted();

    void setEth(QModelIndex index);
    void scanEth();
    void addRs();
    void setRs(QModelIndex index);

    // in: name of registry dir without index, out - name with index
    void rotateSettings(const QString &type, const QString &name);
    bool isKeyExist(const QString &type, const QString &chstr);
    bool updateModel(QDialog *dlg);

private:
    AbstractInterfaceDialog *m_idialog;
    bool updateUModel(QDialog *dlg);
    bool updateRModel(QDialog *dlg);
    bool updateEModel(QDialog *dlg);
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
