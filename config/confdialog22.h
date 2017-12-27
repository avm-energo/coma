/* Файл предназначен для конфигурирования модулей аналоговых АВ-ТУК-2xхх, хх2x и последующих
 */

#ifndef CONFDIALOG22_H
#define CONFDIALOG22_H

#include <QDialog>
#include <QGridLayout>

#include "abstractconfdialog.h"
#include "../config/config22.h"

class ConfDialog22 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog22(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    Config22 *C22;

    void SetupUI();
    void Fill();
    void FillBack();
    void CheckConf();
    void SetDefConf();

signals:

public slots:

private slots:
};

#endif // CONFDIALOG22_H
