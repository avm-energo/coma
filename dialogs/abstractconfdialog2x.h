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

    virtual void SetRange(int Range, int ChNum) = 0;
    virtual void SetInP2(int type, int numch, double value) = 0; // запись в структуру Bci_block значения из spinbox: 0 - in_min, 1 - in_max,..., 7 - setmaxmax
    bool CheckConf();
    void SetMinMax(int i);
    virtual void SetRangeWidget(int ChNum, int RangeType) = 0;
    int GetChNumFromObjectName(QString ObjectName);
    void PrereadConf();

signals:

public slots:

private slots:
    virtual void SetChTypData() = 0;
    void SetChOsc(int);
    void SetChOscSrc(int);
    void SetIn();
    void SetOscDly(int);
    void SetupUI();
    void DisableChannel(int chnum, bool disable);
    void SetRangeWidgetSlot(int RangeType);
};

#endif // ABSTRACTCONFDIALOG2X_H
