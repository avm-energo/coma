#ifndef TUNEKIVCHECK_H
#define TUNEKIVCHECK_H

#include "../../config/configkiv.h"
#include "../abstracttunedialog.h"
#include "tunekiv.h"

#include <QWidget>

class TuneKIVCheck : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVCheck(ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent = nullptr);

private:
    TuneKIV *TKIV;
    ConfigKIV *CKIV;
    ConfigKIV::Bci m_BciSaveBlock;

    void SetLbls() override;
    void SetPf() override;
    void FillBac(int bacnum) override;
    void FillBackBac(int bacnum) override;
    void GetBdAndFill() override;
    Error::Msg SaveWorkConfig(int configblocknum) override;
    QWidget *MainUI() override;

private slots:
    int ReadAnalogMeasurements() override;
    void SetDefCoefs() override;

    Error::Msg setSMode2();
    Error::Msg setNewConfig();
    Error::Msg showScheme();
};

#endif // TUNEKIVCHECK_H
