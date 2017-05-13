/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG21_H
#define CONFDIALOG21_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog2x.h"
#include "../publicclass.h"
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

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    QByteArray confba;
    Config21 *C21;
    QVector<float> RangeInMins;
    QVector<float> RangeInMaxs;

    void Fill();
    bool CheckConf();
    void SetMinMax(int i); // установка значений виджетов в соответствии с конфигурацией

signals:

public slots:

private slots:
    void SetChTypData();
    void SetChOsc(int);
    void SetIn();
    void SetOscDly(double);
    void SetDefConf();
    void SetRange(int Range);
};

#endif // CONFDIALOG21_H
