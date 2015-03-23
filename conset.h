#ifndef CONSET_H
#define CONSET_H

#include <QMainWindow>
#include <QTimer>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include "checkdialog.h"
#include "confdialog.h"
#include "fwupdialog.h"
#include "tunedialog.h"
#include "downloaddialog.h"
#include "serialthread.h"

class ConSet : public QMainWindow
{
    Q_OBJECT

public:
    ConSet(QWidget *parent = 0);
    ~ConSet();

signals:
    void portopened();
    void moduleisok();
    void receivecompleted();

private:
    int baud;
    QSerialPortInfo port;
    checkdialog *CheckDialog;
    confdialog *ConfDialog;
    fwupdialog *FwUpDialog;
    tunedialog *TuneDialog;
    downloaddialog *DownDialog;
    SerialThread *SThread;
    QByteArray inbuf;
    QTimer *TimeoutTimer, *ReadPortTimer;
    bool NothingReceived;

    struct Bsi
    {
        qint32 MType;
        qint32 HWver;
        qint32 FWver;
        qint32 Cfcrc;
        qint32 Rst;
        qint32 Hth;
    };

    void InitiateWriteDataToPort(QByteArray ba);
    QString HalfByteToChar(qint8);

private slots:
    void Exit();
    void Connect();
    void Disconnect();
    void Next();
    void DownloadArchives();
    void DownloadWaveforms();
    void GetAbout();
    void SetBaud(QString);
    void SetPort(QString);
    void GetBsi();
    void CheckBsi();
    void AllIsOk();
    void UpdateMainTE(QByteArray);
    void Timeout();
    void UpdateReadBuf(QByteArray);
};

#endif // CONSET_H
