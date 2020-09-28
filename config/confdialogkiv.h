#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "../config/confdialog.h"
#include "../config/confdialogkxx.h"
#include "../config/configkiv.h"
#include "abstractconfdialog.h"

#include <QDialog>
#include <QLabel>

class ConfDialogKIV : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialogKIV(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);
    ~ConfDialogKIV();

    QLabel *SysTime;
    QTimer *timer;
    QTimer *timerRead;
    QStringList Sbaud { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

private:
    ConfigKIV *CKIV;
    ConfDialog *Conf;
    ConfDialogKxx *ConfKxx;

    bool Variable;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;

signals:

public slots:

private slots:
    void SetDefConf() override;
    void Start_Timer();
    void Stop_Timer();
};

#endif // CONFDIALOG84_H
