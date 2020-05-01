#ifndef CONFDIALOG84_H
#define CONFDIALOG84_H

#include <QDialog>

#include "../config/config84.h"
#include "abstractconfdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include "../iec104/iec104.h"
#include "../iec104/ethernet.h"

class ConfDialog84 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent = nullptr);
    ~ConfDialog84();

    QLabel *SysTime; 
    QLabel *SysTime2;
    QTimer *timer;
    QTimer *timerRead;

private:

    Config84 *C84;
    bool    IsNtp;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();



signals:

public slots:
    void Set104(double);
    void SetCType(int);

private slots:
    void SetDefConf();
    void slot_timeOut();
    void slot2_timeOut();
    void Start_Timer();
    void Stop_Timer();
    void Write_PCDate();
    void Write_Date();



};

#endif // CONFDIALOG84_H
