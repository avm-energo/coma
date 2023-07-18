#ifndef TUNEKIVCHECK_H
#define TUNEKIVCHECK_H

#include "../abstracttunedialog.h"

#include <QShowEvent>
#include <QWidget>

class TuneKIVCheck : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVCheck(S2::Configuration &config, int tuneStep, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
    Error::Msg setSMode2();

protected:
    void showEvent(QShowEvent *e) override;
};

#endif // TUNEKIVCHECK_H
