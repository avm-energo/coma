#ifndef CONFDIALOGKTF_H
#define CONFDIALOGKTF_H


#include <QDialog>

#include "../config/configktf.h"
#include "abstractconfdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"

class ConfDialogKTF : public AbstractConfDialog
{
     Q_OBJECT
public:
    ConfDialogKTF(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);
    ~ConfDialogKTF();

    QLabel *SysTime;
    QTimer *timer;
    QTimer *timerRead;
    QStringList Rates = QStringList()  << "256"<< "128" << "64" << "32" << "16" ;
    QStringList Sbaud = QStringList() << "1200" << "2400"<< "4800" << "9600" << "19200" << "38400" << "57600" << "115200";
    QStringList MBs1 = QStringList() << "нет" << "тип 1"<< "тип 2" << "тип 3";


private:

    ConfigKTF *KTF;
    bool   DDosc ;
    bool   Mb ;
    bool    IsNtp;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();



signals:

public slots:
    void Set104(double);
    void SetCType(int);
    void ChangeWindow(int);

private slots:
    void SetDefConf();
    void Start_Timer();
    void Stop_Timer();
    void Write_PCDate();
    void Write_Date();






};

#endif // CONFDIALOGKTF_H
