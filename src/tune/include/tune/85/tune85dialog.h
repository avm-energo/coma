#pragma once

#include <tune/generaltunedialog.h>

class Tune85Dialog final : public GeneralTuneDialog
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
