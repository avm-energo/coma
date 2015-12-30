#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QAction>
#include <QThread>
#include <QVBoxLayout>
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

#define PROGNAME    "КОМА 1.0 #0072-t"

#define MAINER(a)       ERMSG(publicclass::ER_MAIN,__LINE__,a)
#define MAINDBG         DBGMSG(publicclass::ER_MAIN,__LINE__)
#define MAINWARN        WARNMSG(publicclass::ER_MAIN,__LINE__)
#define MAININFO(a)     INFOMSG(publicclass::ER_MAIN,__LINE__,a)

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    Coma(QWidget *parent = 0);
    ~Coma();

signals:
    void receivecompleted();
    void stopall();
    void mipparset();

private:
    bool SWHide,ERHide, ERTimerIsOn;
    QRect SWGeometry, ERGeometry;
    QTimer *ERTimer;
    QMetaObject::Connection pbh1, pbh2;

    QString Hth[32];
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
    QAction *WriteSNAction, *WriteHWAction;
    quint8 ReconTry;
    publicclass::Bsi Bsi_block;
    void InitiateWriteDataToPort(QByteArray ba);
    QString ByteToHex(quint8);
    void StopThreads();
    void InitiateHth();
    void FillBsi(QString MType, bool clear=false);
    void AddLabelAndLineedit (QVBoxLayout *lyout, QString caption, QString lename);
    void ShowOrHideSlideSW();
    void ShowOrHideSlideER();
    void UpdateMainTE(QByteArray *ba);

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
    void ReadUpdateMainTE(QByteArray *ba);
    void WriteUpdateMainTE(QByteArray *ba);
    void UpdateMainTE104(QByteArray);
    void SetPort(QString str);
    void SetBaud(QString str);
    void WriteSN();
    void WriteHW();
    void SendHW();
    void SetMipConPar();
    void SetMipDlg();
    void UpdateErrorProtocol();
    void MouseMove();
    void HideErrorProtocol();
    void EmulA();
    void EmulE();
    void StartEmulE();
    void StartSettingsDialog();
    void AcceptSettings();
    void SetProgressBarSize(quint32);
    void SetProgressBar(quint32);

protected:
    void closeEvent(QCloseEvent *e);
//    void mouseMoveEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // COMA_H