#ifndef CONFDIALOG84_H
#define CONFDIALOG84_H

#include <QDialog>

#include "../config/config84.h"
#include "abstractconfdialog.h"
#include <QVBoxLayout>
#include <QLabel>

class ConfDialog84 : public AbstractConfDialog
{
    Q_OBJECT
public:
    explicit ConfDialog84(QVector<S2::DataRec> &S2Config, QWidget *parent = nullptr);
    ~ConfDialog84();

    QLabel *SysTime;

private:

    Config84 *C84;
    QTimer *timer;

    void Fill();
    void FillBack();
    void SetupUI();
    void CheckConf();



signals:

public slots:

private slots:
    void SetDefConf();
    void slot_timeOut();
};

#endif // CONFDIALOG84_H
