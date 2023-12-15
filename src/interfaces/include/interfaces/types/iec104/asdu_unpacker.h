#pragma once

#include <interfaces/types/common_types.h>
#include <interfaces/types/iec104/asdu.h>

namespace Iec104
{

class ASDUUnpacker : public QObject
{
    Q_OBJECT
private:
    std::uint32_t m_objAddress, m_index, m_dataSize;

    void parseFloat(const QByteArray &data) noexcept;
    void parseBitString(const QByteArray &data) noexcept;

public:
    explicit ASDUUnpacker(QObject *parent = nullptr) noexcept;
    void unpack(const ASDU &asdu) noexcept;

signals:
    void unpacked(const Interface::DeviceResponse &data);
};

} // namespace Iec104
