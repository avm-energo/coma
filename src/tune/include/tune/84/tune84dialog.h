#pragma once

#include <tune/generaltunedialog.h>

class Tune84Dialog final : public GeneralTuneDialog
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
