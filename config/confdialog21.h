/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG21_H
#define CONFDIALOG21_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog2x.h"
#include "../gen/publicclass.h"
#include "../config/config21.h"

class ConfDialog21 : public AbstractConfDialog2x
{
    Q_OBJECT
public:
    enum RangeTypes
    {
        RT_I420,
        RT_I020,
        RT_I05,
        RT_V55,
        RT_V05,
        RT_IMANUAL,
        RT_VMANUAL
    };

    explicit ConfDialog21(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    Config21 *C21;
    QVector<float> RangeInMins;
    QVector<float> RangeInMaxs;

    void Fill();
    void FillBack();
    void CheckConf();
    void SetDefConf();
    void SetMinMax(int i); // установка значений виджетов в соответствии с конфигурацией

signals:

public slots:

private slots:
    void SetChTypData(int);
//    void SetChOsc(int);
//    void SetIn();
//    void SetOscDly(double);
    void SetRange(int RangeType);
};

#endif // CONFDIALOG21_H
