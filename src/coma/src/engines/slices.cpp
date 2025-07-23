#include <engines/slices.h>

namespace Engines
{

Slices::Slices(Device::CurrentDevice *dev, QObject *parent) : QObject(parent), m_curDev(dev) { }

bool Slices::getWorkJournal()
{
    return true;
}

bool Slices::getSysJournal()
{
    return true;
}

bool Slices::getMeasJournal()
{
    return true;
}

bool Slices::getCurrentState()
{
    return true;
}

bool Slices::getConfig()
{
    return true;
}

bool Slices::getStartup()
{
    return true;
}

bool Slices::getBsi()
{
    return true;
}

bool Slices::getOscs()
{
    return true;
}

bool Slices::getTune()
{
    return true;
}
}
