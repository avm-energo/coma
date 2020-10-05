#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

#include "../config/confdialog.h"
#include "../config/confdialogkxx.h"
#include "../config/configkdv.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "abstractconfdialog.h"

#include <QVBoxLayout>

class ConfDialogKDV : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfDialogKDV(S2ConfigType *S2Config, QWidget *parent = nullptr);

private:
    ConfigKDV *KDV;
    ConfDialog *Conf;
    ConfDialogKxx *ConfKxx;
    //    Config *ConfigMain;

    bool Variable;
    QTimer *timerRead;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;

public slots:
    //    void Set104(double);
    //    void SetCType(int);

private slots:
    void SetDefConf() override;
    void Start_Timer();
    void Stop_Timer();
    //    void Write_PCDate();
    //    void Write_Date();
};

#endif // CONFIGDIALOGKDV_H
