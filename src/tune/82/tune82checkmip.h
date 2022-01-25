#ifndef TUNE82CHECKMIP_H
#define TUNE82CHECKMIP_H

#include "../../gen/modules.h"
#include "../abstracttunedialog.h"
#include "mip.h"

class Tune82CheckMip : public AbstractTuneDialog
{
    Q_OBJECT
public:
    Tune82CheckMip(ConfigV *config, int tuneStep, Modules::MezzanineBoard type, QWidget *parent = nullptr);

    void setModuleType(Modules::MezzanineBoard type);

private:
    void setMessages() override;
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();

    Mip::AvtukVariants m_moduleType;

protected:
};

#endif // TUNE82CHECKMIP_H
