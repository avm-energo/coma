#ifndef TUNE82CHECKMIP_H
#define TUNE82CHECKMIP_H

#include "../abstracttunedialog.h"

class Tune82CheckMip : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82CheckMip(ConfigV *config, int tuneStep, QWidget *parent = nullptr);

private:
    void setMessages() override;
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();

protected:
};

#endif // TUNE82CHECKMIP_H
