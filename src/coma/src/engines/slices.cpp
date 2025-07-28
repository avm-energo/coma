#include <engines/slices.h>

namespace Engines
{

Slices::Slices(Device::CurrentDevice *dev, QObject *parent) : QObject(parent), m_curDev(dev)
{
    // auto conn = m_device->async();
    // auto bsiExt = m_device->bsiExt();
    // connect(bsiExt, &Device::BlockStartupInfoExtended::wasUpdated, this, &InfoDialog::syncExt);
    // connect(m_device, &Device::CurrentDevice::bsiReceived, this, &InfoDialog::sync);
    // connect(this, &InfoDialog::fetchBsi, conn, &AsyncConnection::reqBSI);
}

Error::Msg Slices::CreateSlice()
{
    return Error::Msg::NoError;
}

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
