#pragma once

#include <engines/slices.h>
#include <device/current_device.h>

#include <QDialog>

class SliceGetDialog : public QDialog
{
    Q_OBJECT
public:
    SliceGetDialog(Device::CurrentDevice *device, QWidget *parent=nullptr);

    void SetupUI();

private slots:
    void setRange(Engines::Slices::Stages stage, qint64 max);
    void setPrbValue(Engines::Slices::Stages stage, qint64 value);

private:
    Engines::Slices *m_engine;
    const QMap<Engines::Slices::Stages, QString> c_ProgressMap = {
        { Engines::Slices::Stages::BsiLoad, "prbbsi" },         //
        { Engines::Slices::Stages::ConfigLoad, "prbcfg" },      //
        { Engines::Slices::Stages::MeasJourLoad, "prbmjn" },    //
        { Engines::Slices::Stages::SysJourLoad, "prbsjn" },     //
        { Engines::Slices::Stages::WorkJourLoad, "prbwjn" },    //
        { Engines::Slices::Stages::StartupLoad, "prbstp" },     //
        { Engines::Slices::Stages::TuneLoad, "prbtun" },        //
        { Engines::Slices::Stages::GetCurrentState, "prbstt" }, //
        { Engines::Slices::Stages::Save, "prbsav" },            //
    };
};
