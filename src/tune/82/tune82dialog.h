#ifndef TUNE82DIALOG_H
#define TUNE82DIALOG_H

#include "../../models/report.h"
#include "../../widgets/udialog.h"
#include "../generaltunedialog.h"
#include "../kiv/tunekivtemp60.h"
//#include "tune82adc.h"
//#include "tune82check.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class Tune82Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune82Dialog(ConfigV *config, QWidget *parent = nullptr);

public slots:

private:
    //    LimeReport::ReportEngine *m_Report;
    void prepareReport() override;

private slots:
};

#endif // TUNE82DIALOG_H
