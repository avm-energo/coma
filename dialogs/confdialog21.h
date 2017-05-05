/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG21_H
#define CONFDIALOG21_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../publicclass.h"
#include "../config/config21b.h"
#include "../config/config21m.h"

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

#define AIN21_NUMCH 8

class ConfDialog21 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog21(bool BaseBoard=true, QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    QByteArray confba;
    bool isBaseBoard;
    Config21b *C21b;
    Config21m *C21m;
    bool ChTypModelIsFilling;

    void Fill();
    void SetRange(int Range, int ChNum);
    void SetRangeCB(int ChNum, int ChTypCB);
    bool CheckConf();
    void SetMinMax(int i);
    void SetRangeWidget(int ChNum, int RangeType);
    int GetChNumFromObjectName(QString ObjectName);

signals:

public slots:

private slots:
    void SetChTypData();
    void SetChOsc(int);
    void SetChOscSrc(int);
    void SetIn();
    void SetRangemA();
    void SetRangeV();
//    void Set104();
//    void SetCType(int);
    void SetOscDly(int);
    void ReadConf();
    void WriteConfDataToModule();
    void SetupUI();
    void SetDefConf();
    void SaveConfToFile();
    void LoadConfFromFile();
    void DisableChannel(int ChNum, bool Disable);
    void SetRangeWidgetSlot(QString RangeType);
};

#endif // CONFDIALOG21_H
