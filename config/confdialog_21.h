/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG_21_H
#define CONFDIALOG_21_H

#include <QDialog>
#include <QGridLayout>

#include "../publicclass.h"
#include "config21.h"

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

#define ACONFER(a)       ERMSG(publicclass::ER_ACONF,__LINE__,a)
#define ACONFDBG         DBGMSG(publicclass::ER_ACONF,__LINE__)
#define ACONFWARN        WARNMSG(publicclass::ER_ACONF,__LINE__)
#define ACONFINFO(a)     INFOMSG(publicclass::ER_ACONF,__LINE__,a)

class confdialog_2x : public QDialog
{
    Q_OBJECT
public:
    explicit confdialog_2x(QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    QByteArray confba;
    config_21 *aconf;
    bool ChTypModelIsFilling;

    void FillConfData();
    void SetRange(int Range, int ChNum);
    void SetRangeCB(int ChNum, int ChTypCB);
    bool CheckConf();
    void SetMinMax(int i);
    void SetRangeWidget(int ChNum, int RangeType);
    void SetSpinboxValue(QString name, double value);
    int GetChNumFromObjectName(QString ObjectName);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
    void SetChTypData();
    void SetChOsc(int);
    void SetChOscSrc(int);
    void SetIn();
    void SetRangemA();
    void SetRangeV();
    void Set104();
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void WriteConfDataToModule();
    void SetupUI();
    void SetNewConf();
    void SetDefConf();
    void SaveConf();
    void LoadConf();
    void DisableChannel(int ChNum, bool Disable);
    void SetRangeWidgetSlot(QString RangeType);
};

#endif // CONFDIALOG_21_H
