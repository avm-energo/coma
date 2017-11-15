/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-3xхх, хх3x и последующих
 */

#ifndef ABSTRACTCONFDIALOG3X_H
#define ABSTRACTCONFDIALOG3X_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"

class AbstractConfDialog3x : public AbstractConfDialog
{
    Q_OBJECT
public:
    struct SpecificParams
    {
        QStringList InTypes;
        QStringList OutTypes;
        int InNumCh;
        int OutNumCh;
    };

    explicit AbstractConfDialog3x(QWidget *parent = 0);

    void DisableChannel(int chnum, bool disable);
    void SetupUI();
    void SetInputs(QStringList InputsTypes, int InputsNum);
    void SetOutputs(QStringList OutputsTypes, int OutputsNum);

private:

    SpecificParams Params;

signals:

public slots:

private slots:
    virtual void SetChTypData(int) = 0; // задать тип канала (номер канала - в objectName.split(".").at(1)
    virtual void SetDly(double) = 0; // задать задержку для контроля дребезга
    virtual void SetPair(int) = 0; // задать парность каналов
};

#endif // ABSTRACTCONFDIALOG3X_H
