#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QAction>
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
#include "oscdialog.h"
#include "canal.h"
#include "publicclass.h"

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = 0);
    ~Coma();

signals:
    void receivecompleted();
    void stopall();
    void updateconfproper(bool);
    void updatetuneproper(bool);
    void mipparset();

private:
    QString Hth[32];
//    QThread *thread;
    a_checkdialog *ACheckDialog;
    a_confdialog *AConfDialog;
    a_tunedialog *ATuneDialog;
    e_checkdialog *ECheckDialog;
    e_confdialog *EConfDialog;
    e_tunedialog *ETuneDialog;
    fwupdialog *FwUpDialog;
    downloaddialog *DownDialog;
    oscdialog *OscDialog;
    bool DialogsAreReadyAlready;
    QAction *WriteSNAction;
    quint8 ReconTry;
//    canal *cn;
    publicclass::Bsi Bsi_block;
    void InitiateWriteDataToPort(QByteArray ba);
    QString ByteToHex(quint8);
    void StopThreads();
    void InitiateHth();
    void FillBsi(QString MType, bool clear=false);

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
    void UpdateMainTE104(QByteArray);
    void SetPort(QString str);
    void SetBaud(QString str);
    void WriteSN();
    void SetMipConPar();
    void SetMipDlg();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // COMA_H
