#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/asdu.h>

namespace Iec104
{

/// \brief The unpacker adapter class for ASDU objects.
class ASDUUnpacker : public QObject
{
    Q_OBJECT
private:
    std::uint32_t m_objAddress, m_index, m_dataSize;
    bool m_isRunning;

    /// \brief Parsing a float data from the received ASDU object.
    void parseFloat(const QByteArray &data) noexcept;
    /// \brief Parsing a bitstring data from the received ASDU object.
    void parseBitString(const QByteArray &data) noexcept;

public:
    /// \brief Default c-tor.
    explicit ASDUUnpacker(QObject *parent = nullptr) noexcept;

    /// \brief Running unpack process the received ASDU object.
    void unpack(const ASDU &asdu) noexcept;
    /// \brief Stopping unpack process.
    void stopUnpacking() noexcept;

signals:
    /// \brief Signal for sending an unpacked data from the received ASDU object.
    void unpacked(const Interface::DeviceResponse &data);
    /// \brief Signal for sending the unpacked object address.
    void unpackedObjectAddress(const std::uint32_t address);
};

} // namespace Iec104
