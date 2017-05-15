/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-3xхх, хх3x и последующих
 */

#ifndef ABSTRACTCONFDIALOG3X_H
#define ABSTRACTCONFDIALOG3X_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../publicclass.h"

class AbstractConfDialog3x : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog3x(QWidget *parent = 0);

    struct SpecificParams
    {
        QStringList InTypes;
        QStringList RangeTypes;
        int NumCh;
    };

    QByteArray confba;
    SpecificParams Params;

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

#endif // ABSTRACTCONFDIALOG3X_H
