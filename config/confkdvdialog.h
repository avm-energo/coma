#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

//#include "../config/confdialog.h"
#include "../config/configkdv.h"
#include "../config/confkxxdialog.h"
//#include "../interfaces/ethernet.h"
//#include "../iec104/iec104.h"
#include "abstractconfdialog.h"

#include <QVBoxLayout>

class ConfKDVDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfKDVDialog(ConfigKDV *ckdv, QWidget *parent = nullptr);

private:
    ConfigKDV *CKDV;
    //    ConfDialog *Conf;
    ConfKxxDialog *ConfKxx;
    //    Config *ConfigMain;

    bool Variable;
    //    QTimer *timerRead;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;
    void uponInterfaceSetting();
    //    void setConnections() override;

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
