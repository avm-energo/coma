#pragma once

#include "../../models/report.h"
#include "../../widgets/udialog.h"
#include "../generaltunedialog.h"
#include "tune85adc.h"
#include "tune85check.h"
#include "tune85temp60.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class Tune85Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune85Dialog(ConfigV *config, QWidget *parent = nullptr);

public slots:

private:
    //    LimeReport::ReportEngine *m_Report;
    void prepareReport() override;

private slots:
    //    void GenerateReport();
};
