#pragma once

#include <QObject>

namespace Device
{
class CurrentDevice;
} // namespace Device

namespace Xml
{

/// \brief Класс для загрузки XML конфигурации текущего подключённого устройства.
class ConfigLoader final : public QObject
{
    Q_OBJECT
private:
    Device::CurrentDevice *m_device;
    bool m_parseStatus;

    bool loadS2Data() noexcept;
    bool loadDeviceData() noexcept;

private slots:
    void parseErrorHandle(const QString errMsg);

public:
    explicit ConfigLoader(Device::CurrentDevice *device) noexcept;

    bool loadSettings() noexcept;
};

} // namespace Xml

using XmlConfigLoader = Xml::ConfigLoader;
