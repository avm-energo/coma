#ifndef CONFDIALOG87_H
#define CONFDIALOG87_H

/* Файл предназначен для конфигурирования модулей ЧР АВ-ТУК-87
 */

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../config/config87.h"

class ConfDialog87 : public AbstractConfDialog
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

    explicit ConfDialog87(QVector<S2::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = nullptr);
    //~ConfDialog87();

private:
    struct SpecificParams
    {
        QStringList InTypes;
        QStringList RangeTypes;
        int NumCh;
    };

    QByteArray confba;
    SpecificParams Params;
    Config87 *C87;
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

#endif // CONFDIALOG87_H
