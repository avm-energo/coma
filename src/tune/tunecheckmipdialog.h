#ifndef TUNECHECKMIPDIALOG_H
#define TUNECHECKMIPDIALOG_H

#include "../module/modules.h"
#include "abstracttunedialog.h"
#include "mip.h"

class TuneCheckMipDialog : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneCheckMipDialog(S2::Configuration &config, int tuneStep, //
        Modules::MezzanineBoard type, QWidget *parent = nullptr);

    void setModuleType(Modules::MezzanineBoard type);

private:
    void setTuneFunctions() override;
    Error::Msg showScheme();
    Error::Msg check();

    using MType = Modules::MezzanineBoard;

    MType m_moduleType;

protected:
};

#endif // TUNECHECKMIPDIALOG_H
