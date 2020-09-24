#ifndef TUNEKIVMAIN_H
#define TUNEKIVMAIN_H

#include "eabstracttunedialog.h"
#include <QWidget>

class TuneKIVMain : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVMain(QWidget *parent = nullptr);
};

#endif // TUNEKIVMAIN_H
