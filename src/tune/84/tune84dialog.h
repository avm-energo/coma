#ifndef TUNE84DIALOG_H
#define TUNE84DIALOG_H

#include "../../models/report.h"
#include "../generaltunedialog.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

class Tune84Dialog : public GeneralTuneDialog
{
    Q_OBJECT
public:
    explicit Tune84Dialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

public slots:

private:
    //    Tune84Check *T84CheckDialog;
    //    Tune84ADC *T84ADCUDialog, *T84ADCIDialog;
    //    TuneKIVTemp60 *T8460Dialog, *T8420Dialog;

    //    LimeReport::ReportEngine *m_Report;
    void prepareReport() override;

    //    void SetupUI();

private slots:
    //    void GenerateReport();
};

#endif // TUNE84DIALOG_H
