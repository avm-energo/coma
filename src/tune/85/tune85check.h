#ifndef TUNE85CHECK_H
#define TUNE85CHECK_H

#include "../abstracttunedialog.h"

#include <QShowEvent>
#include <QWidget>

class Tune85Check : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune85Check(S2::Configuration &config, int tuneStep, QWidget *parent = nullptr);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();
private slots:

protected:
    void showEvent(QShowEvent *e) override;
};

#endif // TUNE85CHECK_H
