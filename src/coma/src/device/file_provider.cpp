#include "device/file_provider.h"

#include <device/current_device.h>

namespace Device
{

FileProvider::FileProvider(CurrentDevice *device)
    : QObject(device), m_currentDevice(device), m_lastRequestedFile(0xffff)
{
    m_currentDevice->async()->connection(this, &FileProvider::responseReceived);
    connect(this, &FileProvider::response, m_currentDevice->getS2Datamanager(), &S2DataManager::parseS2File);
}

void FileProvider::responseReceived(const QByteArray &file)
{
    auto filenum = *reinterpret_cast<quint16 *>(file.left(sizeof(quint16)).data());
    if (filenum == m_lastRequestedFile)
        emit response(file);
}

void FileProvider::request(const S2::FilesEnum filenum, bool withCheck)
{
    if (m_currentDevice->health().isNoConfig() && withCheck)
    {
        emit noConfigurationError();
        return;
    }

    m_lastRequestedFile = std_ext::to_underlying(filenum);
    m_currentDevice->async()->reqFile(m_lastRequestedFile, DataTypes::FileFormat::DefaultS2);
}

} // namespace Device
