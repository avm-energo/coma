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

private:
    int baud;
    QSerialPortInfo port;
    checkdialog *CheckDialog;
    confdialog *ConfDialog;
    fwupdialog *FwUpDialog;
    tunedialog *TuneDialog;
    downloaddialog *DownDialog;
    SerialThread *SThread;

    QTimer *TimeoutTimer;

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
    void CheckBsi(QByteArray);
    void AllIsOk();
    void UpdateMainTE(QByteArray);
    void Timeout();
};

#endif // CONSET_H
