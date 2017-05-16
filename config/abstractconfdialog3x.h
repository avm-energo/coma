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
        int NumCh;
    };

    QByteArray confba;
    SpecificParams Params;

    void DisableChannel(int chnum, bool disable);
    void SetupUI();

private:

signals:

public slots:

private slots:
    virtual void SetChTypData() = 0; // задать тип канала (номер канала - в objectName.aplit(".").at(1)
    virtual void SetDly(int) = 0; // задать задержку для контроля дребезга
    virtual void SetPair(int) = 0; // задать парность каналов
};

#endif // ABSTRACTCONFDIALOG3X_H
