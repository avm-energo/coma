#ifndef CONSET_H
#define CONSET_H

#include <QMainWindow>
#include <QTimer>
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

private:
    int baud;
    QSerialPortInfo port;
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
    QString ByteToHex(qint8);
    void AllIsOk();

private slots:
    void Exit();
    void Connect();
    void Disconnect();
    void Next();
    void GetAbout();
    void SetBaud(QString);
    void SetPort(QString);
    void GetBsi();
    void CheckBsi();
    void UpdateMainTE(QByteArray);
    void Timeout();
};

#endif // CONSET_H
