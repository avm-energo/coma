#pragma once

#include "../../models/report.h"
#include "../../widgets/udialog.h"
#include "../generaltunedialog.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class Tune85Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune85Dialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

public slots:

private:
    //    LimeReport::ReportEngine *m_Report;
    void prepareReport() override;

private slots:
    //    void GenerateReport();
};
