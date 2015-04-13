#ifndef CONSET_H
#define CONSET_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include "fwupdialog.h"
#include "downloaddialog.h"
#include "A/checkdialog.h"
#include "A/confdialog.h"
#include "A/tunedialog.h"

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
    QThread *thread;
    a_checkdialog *ACheckDialog;
    a_confdialog *AConfDialog;
    a_tunedialog *ATuneDialog;
    fwupdialog *FwUpDialog;
    downloaddialog *DownDialog;
    QByteArray inbuf;
    QTimer *TimeoutTimer, *ReadPortTimer;
    bool NothingReceived;

    struct Bsi
    {
        qint32 MType;
        qint32 MType1;
        qint32 HWver;
        qint32 FWver;
        qint32 Cfcrc;
        qint32 Rst;
        qint32 Hth;
    };

    void InitiateWriteDataToPort(QByteArray ba);
    QString ByteToHex(quint8);
    void AllIsOk();
    void StopThreads();

private slots:
    void Exit();
    void Connect();
    void Disconnect();
    void Next();
    void GetAbout();
    void ShowErrMsg(int);
    void GetBsi();
    void CheckBsi();
    void UpdateMainTE(QByteArray);
    void Timeout();
    void KillSThread();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // CONSET_H
