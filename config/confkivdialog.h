#ifndef CONFDIALOGKIV_H
#define CONFDIALOGKIV_H

#include "../config/confdialog.h"
#include "../config/configkiv.h"
#include "../config/confkxxdialog.h"

#include <QLabel>

class ConfKIVDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKIVDialog(ConfigKIV *ckiv, QWidget *parent = nullptr);
    ~ConfKIVDialog();

private:
    ConfigKIV *CKIV;
    ConfDialog *Conf;
    ConfKxxDialog *ConfKxx;
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
