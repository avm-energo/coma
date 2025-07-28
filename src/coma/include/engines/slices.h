#pragma once

#include <device/current_device.h>
#include <gen/error.h>
#include <gen/files.h>

#include <QObject>

namespace Engines
{

class Slices : public QObject
{
    Q_OBJECT
public:
    enum Stages
    {
        ConfigLoad,
        BsiLoad,
        TuneLoad,
        MeasJourLoad,
        SysJourLoad,
        WorkJourLoad,
        StartupLoad,
        GetCurrentState,
        Save,
        GetOsc
    };

    Slices(Device::CurrentDevice *dev, QObject *parent = nullptr);

    Error::Msg CreateSlice();

signals:
    void setProgressRange(Stages stage, qint64 max);
    void setProgressValue(Stages stage, qint64 value);

private:
    Device::CurrentDevice *m_curDev;

    bool getWorkJournal();
    bool getSysJournal();
    bool getMeasJournal();
    bool getCurrentState();
    bool getConfig();
    bool getStartup();
    bool getBsi();
    bool getOscs();
    bool getTune();
};

}
