#pragma once

#include <device/current_device.h>
#include <engines/slices.h>

#include <QDialog>

class SliceGetDialog : public QDialog
{
    Q_OBJECT
public:
    SliceGetDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

    void SetupUI();

signals:
    void cancel();

private slots:
    void setRange(Engines::Slices::Stages stage, qint64 max);
    void setPrbValue(Engines::Slices::Stages stage, qint64 value);
    void startProcess();
    void finished(int id);
    void sliceResultReceived(Error::Msg result);

private:
    Device::CurrentDevice *m_device;
    int m_proc_id;
    bool isCancelled;
    const QMap<Engines::Slices::Stages, QString> c_ProgressMap = {
        { Engines::Slices::Stages::BsiLoad, "prbbsi" },         //
        { Engines::Slices::Stages::BsiLoadExt, "prbbsx" },      //
        { Engines::Slices::Stages::ConfigLoad, "prbcfg" },      //
        { Engines::Slices::Stages::MeasJourLoad, "prbmjn" },    //
        { Engines::Slices::Stages::SysJourLoad, "prbsjn" },     //
        { Engines::Slices::Stages::WorkJourLoad, "prbwjn" },    //
        { Engines::Slices::Stages::StartupLoad, "prbstp" },     //
        { Engines::Slices::Stages::TuneLoad, "prbtun" },        //
        { Engines::Slices::Stages::GetOsc, "prbosc" },          //
        { Engines::Slices::Stages::GetCurrentState, "prbstt" }, //
        { Engines::Slices::Stages::Save, "prbsav" },            //
    };

    void clearPrbs();
};
