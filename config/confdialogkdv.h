#ifndef CONFDIALOGKDV_H
#define CONFDIALOGKDV_H

#include "../config/confdialog.h"
#include "../config/confdialogkxx.h"
#include "../config/configkdv.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "abstractconfdialog.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class ConfDialogKDV : public AbstractConfDialog
{
    Q_OBJECT
public:
    ConfDialogKDV(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);

    // QStringList Rates = QStringList()  << "256"<< "128" << "64" << "32" << "16"
    // ;
    QStringList Sbaud = { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };

private:
    ConfigKDV *KDV;
    ConfDialog *Conf;
    ConfDialogKxx *ConfKxx;
    Config *ConfigMain;

    bool Variable;
    QTimer *timerRead;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();

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

#endif // CONFIGDIALOGKDV_H
