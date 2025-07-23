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
    Slices(Device::CurrentDevice *dev, QObject *parent = nullptr);

    Error::Msg CreateSlice();

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
