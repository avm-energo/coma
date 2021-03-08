#ifndef TUNE84DIALOG_H
#define TUNE84DIALOG_H

#include "../../check/checkkiv.h"
#include "../../config/config.h"
#include "../../config/configkiv.h"
#include "../../gen/report.h"
#include "../../widgets/udialog.h"
#include "../generaltunedialog.h"
#include "../kiv/tunekivadc.h"
#include "../kiv/tunekivcheck.h"
#include "../kiv/tunekivtemp60.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class Tune84Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune84Dialog(ConfigKIV *ckiv, QWidget *parent = nullptr);

public slots:

private:
    ConfigKIV *CKIV;
    TuneKIVCheck *TKIVCheckDialog;
    TuneKIVADC *TKIVADCDialog;
    TuneKIVTemp60 *TKIV60Dialog, *TKIV20Dialog;

    LimeReport::ReportEngine *m_Report;

    void SetupUI();

private slots:
    void GenerateReport();
};

#endif // TUNE84DIALOG_H
