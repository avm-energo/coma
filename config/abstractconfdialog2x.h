/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef ABSTRACTCONFDIALOG2X_H
#define ABSTRACTCONFDIALOG2X_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../publicclass.h"

class AbstractConfDialog2x : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog2x(bool BaseBoard=true, QWidget *parent = 0);

    struct SpecificParams
    {
        QStringList InTypes;
        QStringList RangeTypes;
        int NumCh;
    };

    QByteArray confba;
    bool isBaseBoard;
    SpecificParams Params;
    bool NoProperConf; // в модуле нет нормальной конфигурации

    void DisableChannel(int chnum, bool disable);
    int GetChNumFromObjectName(QString ObjectName);
    void SetupUI();

private:

signals:

public slots:

private slots:
    virtual void SetChTypData() = 0; // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    virtual void SetChOsc(int) = 0; //
    virtual void SetIn() = 0; // задать значение одного из свойств сигнала канала (мин, макс, инж. ед., уставки)
    virtual void SetOscDly(double) = 0; // задать задержку перед началом осциллографирования
    virtual void SetRange(int RangeType) = 0;
};

#endif // ABSTRACTCONFDIALOG2X_H
