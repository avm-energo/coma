#ifndef CONFDIALOGKTF_H
#define CONFDIALOGKTF_H

#include "../config/confdialog.h"
#include "../config/configktf.h"
#include "../config/confkxxdialog.h"
//#include "../iec104/ethernet.h"
//#include "../iec104/iec104.h"
#include "abstractconfdialog.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class ConfKTFDialog : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfKTFDialog(ConfigKTF *cktf, QWidget *parent = nullptr);
    ~ConfKTFDialog();

    int getRCount();

private:
    ConfigKTF *CKTF;
    ConfDialog *Conf;
    ConfKxxDialog *ConfKxx;
    QLabel *SysTime;
    //    QTimer *timer;
    //    QTimer *timerRead;

    QStringList Rates = { "256", "128", "64", "32", "16" };

    bool DDosc;
    bool Mb;
    bool IsNtp;
    QList<QWidget *> WidgetList;

    void Fill() override;
    void FillBack() override;
    void SetupUI() override;
    void CheckConf() override;

signals:

public slots:

private slots:
    void SetDefConf() override;
    //    void Start_Timer();
    //    void Stop_Timer();
};

#endif // CONFDIALOGKTF_H
