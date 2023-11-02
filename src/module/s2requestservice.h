#pragma once

#include "../s2/s2datatypes.h"

#include <QByteArray>
#include <QObject>
#include <gen/datamanager/typesproxy.h>
#include <gen/stdfunc.h>

/// \brief Class for providing requests in S2 data format from device.
class S2RequestService final : public QObject
{
    Q_OBJECT
private:
    UniquePointer<DataTypesProxy> m_proxy;
    quint16 m_lastRequestedFile;

private slots:
    /// \brief This slot is called when current service will receive a response from the device.
    void responseReceived(const QVariant &var);

public:
    explicit S2RequestService(QObject *parent = nullptr);

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
