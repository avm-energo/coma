#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QAction>
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

#define PROGNAME    "КОМА 1.0 #0063"

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
    void updateconfproper(bool);
    void updatetuneproper(bool);
    void mipparset();

private:
    bool SWHide,ERHide;
    QRect SWGeometry, ERGeometry;

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
    QAction *WriteSNAction;
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
    void UpdateErrorProtocol();
    void MouseMove();

protected:
    void closeEvent(QCloseEvent *e);
//    void mouseMoveEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // COMA_H
