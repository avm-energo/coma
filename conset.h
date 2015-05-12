#ifndef CONSET_H
#define CONSET_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include "fwupdialog.h"
#include "downloaddialog.h"
#include "A/a_checkdialog.h"
#include "A/a_confdialog.h"
#include "A/a_tunedialog.h"
#include "E/e_checkdialog.h"
#include "E/e_confdialog.h"
#include "E/e_tunedialog.h"
#include "canal.h"
#include "publicclass.h"

class ConSet : public QMainWindow
{
    Q_OBJECT

public:
    ConSet(QWidget *parent = 0);
    ~ConSet();

signals:
    void portopened();
    void receivecompleted();
    void stopall();

private:
    canal *cn;
    QThread *thread;
    a_checkdialog *ACheckDialog;
    a_confdialog *AConfDialog;
    a_tunedialog *ATuneDialog;
    e_checkdialog *ECheckDialog;
    e_confdialog *EConfDialog;
    e_tunedialog *ETuneDialog;
    fwupdialog *FwUpDialog;
    downloaddialog *DownDialog;
    bool DialogsAreReadyAlready;

    publicclass::Bsi Bsi_block;
    void InitiateWriteDataToPort(QByteArray ba);
    QString ByteToHex(quint8);
    void StopThreads();

public slots:
    void GetBsi();

private slots:
    void AllIsOk();
    void Exit();
    void Connect();
    void Disconnect();
    void Next();
    void GetAbout();
    void ShowErrMsg(int);
    void CheckBsi();
    void UpdateMainTE(QByteArray);
    void KillSThread();
    void SetPort(QString str);
    void SetBaud(QString str);

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // CONSET_H
