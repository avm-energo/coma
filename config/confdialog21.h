/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG21_H
#define CONFDIALOG21_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../config/config21.h"

class ConfDialog21 : public AbstractConfDialog
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

    explicit ConfDialog21(QVector<S2::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = nullptr);

private:
    struct SpecificParams
    {
        QStringList InTypes;
        QStringList RangeTypes;
        int NumCh;
    };

    QByteArray confba;
    SpecificParams Params;
    Config21 *C21;
    QVector<float> RangeInMins;
    QVector<float> RangeInMaxs;

    void Fill();
    void FillBack();
    void CheckConf();
    void SetDefConf();
    void SetMinMax(int i); // установка значений виджетов в соответствии с конфигурацией
    void SetupUI();
    void DisableChannel(int chnum, bool disable);

signals:

public slots:

private slots:
    void SetChTypData(int); // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    void SetRange(int RangeType);
};

#endif // CONFDIALOG21_H
