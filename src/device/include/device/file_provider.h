#pragma once

#include <QByteArray>
#include <QObject>
#include <gen/integers.h>
#include <s2/s2datatypes.h>

namespace Device
{

class CurrentDevice;

/// \brief Class for providing requests in S2 data format from device.
class FileProvider final : public QObject
{
    Q_OBJECT
private:
    CurrentDevice *m_currentDevice;
    u16 m_lastRequestedFile;

private slots:
    /// \brief This slot is called when current service will receive a response from the device.
    void responseReceived(const QByteArray &file);

public:
    explicit FileProvider(CurrentDevice *device);

public slots:
    /// \brief This slot is called when an external subscriber will register request a data to the device.
    /// \param withCheck [in] - if the device has no valid requsted data, signal noConfigurationError will be emitting.
    void request(const S2::FilesEnum filenum, bool withCheck = false);

signals:
    /// \brief This signal is emitting when a response from the device received.
    /// \details Slot responseReceived checks whether the received data
    /// is a response to the last registered request.
    /// \see responseReceived
    void response(const QByteArray &response);
    /// \brief This signal is emitting when the device has no valid requested S2 data.
    void noConfigurationError();
};

} // namespace Device

using DeviceFileProvider = Device::FileProvider;
