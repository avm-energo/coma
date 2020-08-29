#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "../config/confdialog.h"
#include "../config/confdialogkxx.h"
#include "../config/configkiv.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "abstractconfdialog.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class ConfDialogKIV : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialogKIV(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);
    ~ConfDialogKIV();

    QLabel *SysTime;
    QTimer *timer;
    QTimer *timerRead;
    QStringList Sbaud = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

private:
    ConfigKIV *CKIV;
    ConfDialog *Conf;
    ConfDialogKxx *ConfKxx;
    //    Config *ConfigMain;

    bool Variable;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();

signals:

public slots:
    //    void Set104(double);
    //    void SetCType(int);

private slots:
    void SetDefConf();
    void Start_Timer();
    void Stop_Timer();
    //    void Write_PCDate();
    //    void Write_Date();
};

#endif // CONFDIALOG84_H
