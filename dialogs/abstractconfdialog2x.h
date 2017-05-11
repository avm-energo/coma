/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef ABSTRACTCONFDIALOG2X_H
#define ABSTRACTCONFDIALOG2X_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../publicclass.h"

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

class AbstractConfDialog2x : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog2x(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    struct SpecificParams
    {
        const QStringList InTypes;
        const QStringList RangeTypes;
        const int NumCh;
    };
    QByteArray confba;
    bool isBaseBoard;
    SpecificParams Params;

    void SetupUI();
    int GetChNumFromObjectName(QString ObjectName);
    void PrereadConf();
    void DisableChannel(int chnum, bool disable);

signals:

public slots:

private slots:
    virtual void SetChTypData() = 0; // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    virtual void SetChOsc(int) = 0; //
    virtual void SetIn(); // задать значение одного из свойств сигнала канала (мин, макс, инж. ед., уставки)
    virtual void SetOscDly(int); // задать задержку перед началом осциллографирования
    virtual void SetRange(int RangeType);
};

#endif // ABSTRACTCONFDIALOG2X_H
