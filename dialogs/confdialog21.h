/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG21_H
#define CONFDIALOG21_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog2x.h"
#include "../publicclass.h"
#include "../config/config21.h"

#define RT_mA          0
#define RT_V           1
#define RT_M           2

#define RT_mAText   "Предуст. мА"
#define RT_VText    "Предуст. В"
#define RT_MText    "Произвольный"

#define RT_mA420    0
#define RT_mA020    1
#define RT_mA05     2
#define RT_V05      3
#define RT_V_55     4

class ConfDialog21 : public AbstractConfDialog2x
{
    Q_OBJECT
public:
    explicit ConfDialog21(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    QByteArray confba;
    Config21 *C21;
    const QVector<float> RangeInMins = {4.0, 0.0, 0.0, 0.0, -5.0};
    const QVector<float> RangeInMaxs = {20.0, 20.0, 5.0, 5.0, 5.0};

    void Fill();
    void SetRange(int Range, int ChNum);
    bool CheckConf();
    void SetMinMax(int i);
    void SetRangeWidget(int ChNum, int RangeType);
    void SetInP2(int type, int chnum, double value);

signals:

public slots:

private slots:
    void SetChTypData();
    void SetChOsc(int);
    void SetChOscSrc(int);
    void SetIn();
    void SetRangemA();
    void SetRangeV();
    void SetOscDly(int);
    void SetDefConf();
    void SetRangeWidgetSlot(QString RangeType);
};

#endif // CONFDIALOG21_H
