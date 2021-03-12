#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

#include "../config/configkdv.h"
#include "abstractconfdialog.h"

class ConfKDVDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfKDVDialog(ConfigKDV *ckdv, QWidget *parent = nullptr);

private:
    ConfigKDV *CKDV;
    //    ConfDialog *Conf;
    //    ConfKxxDialog *ConfKxx;
    //    Config *ConfigMain;

    bool Variable;
    //    QTimer *timerRead;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;
    //    void setConnections() override;

    QWidget *analogWidget();

public slots:
    //    void Set104(double);
    //    void SetCType(int);

private slots:
    void SetDefConf() override;
    //    void Start_Timer();
    //    void Stop_Timer();
    //    void Write_PCDate();
    //    void Write_Date();
};

#endif // CONFIGDIALOGKDV_H
