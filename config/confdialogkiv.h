#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "../config/confdialog.h"
#include "../config/confdialogkxx.h"
#include "../config/configkiv.h"

#include <QLabel>

class ConfDialogKIV : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialogKIV(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);
    ~ConfDialogKIV();

private:
    ConfigKIV *CKIV;
    ConfDialog *Conf;
    ConfDialogKxx *ConfKxx;
    QLabel *SysTime;
    QTimer *timer;
    QTimer *timerRead;

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
