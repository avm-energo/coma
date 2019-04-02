#ifndef TUNEDIALOG84_H
#define TUNEDIALOG84_H

#include <QDialog>
#include "eabstracttunedialog.h"
#include "../check/check84.h"
#include "../gen/modulebsi.h"
#include "../config/config84.h"
#include "../iec104/iec104.h"
#include <QStandardItemModel>
#include "limereport/lrreportengine.h"
#include <QHBoxLayout>

#define TUNEFILELENGTH  256

// voltages
#define V60     60.0
#define V57     57.74
// frequencies
#define HZ50    50.0
// currents
#define I1      1.0
#define I5      5.0

// parameters for GetExtData
#define TD_GED_U    0x01 // напряжение
#define TD_GED_I    0x02 // ток
#define TD_GED_F    0x04 // частота
#define TD_GED_D    0x08 // угол нагрузки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz

class QGroupBox;

class TuneDialog84 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent = nullptr);
    //~TuneDialog85();

signals:

public slots:

private:

    Config84 *C84;
    QVector<S2::DataRec> *S2Config;
    Config84::Bci Bci_block_work;

    void SetupUI();

#if PROGSIZE != PROGSIZE_EMUL

#endif


private slots:
#if PROGSIZE != PROGSIZE_EMUL

#endif


};


#endif // TUNEDIALOG85_H
