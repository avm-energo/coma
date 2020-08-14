#ifndef CONFDIALOG84_H
#define CONFDIALOG84_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

#include "../config/config84.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "abstractconfdialog.h"

class ConfDialog84 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog84(QVector<S2::DataRec> *S2Config, QWidget *parent = nullptr);
    ~ConfDialog84();

    QLabel *SysTime;
    QTimer *timer;
    QTimer *timerRead;
    QStringList Rates = QStringList() << "1200"
                                      << "2400"
                                      << "4800"
                                      << "9600"
                                      << "19200"
                                      << "38400"
                                      << "57600"
                                      << "115200";

private:
    Config84 *C84;
    bool IsNtp;

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
    void Start_Timer();
    void Stop_Timer();
    void Write_PCDate();
    void Write_Date();
};

#endif // CONFDIALOG84_H
