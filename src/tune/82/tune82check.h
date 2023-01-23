#pragma once

#include "../abstracttunedialog.h"

#include <QShowEvent>
#include <QWidget>

class Tune82Check : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82Check(ConfigV *config, int tuneStep, Modules::MezzanineBoard type, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
    Error::Msg checkMip();

    Modules::MezzanineBoard m_typeM;

private slots:

protected:
    void showEvent(QShowEvent *e) override;
};
