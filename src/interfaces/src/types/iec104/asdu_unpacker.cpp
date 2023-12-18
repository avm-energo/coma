#include "interfaces/types/iec104/asdu_unpacker.h"

#include <gen/datatypes.h>
#include <gen/std_ext.h>
#include <gen/stdfunc.h>

namespace Iec104
{

ASDUUnpacker::ASDUUnpacker(QObject *parent) noexcept
    : QObject(parent), m_objAddress(0), m_index(0), m_dataSize(0), m_isRunning(false)
{
}

void ASDUUnpacker::unpack(const ASDU &asdu) noexcept
{
    const auto &dataRef = asdu.m_data;
    m_objAddress = 0, m_index = 0, m_dataSize = dataRef.size();
    m_isRunning = true;
    for (auto i = 0; (i < asdu.m_elements) && (m_index < m_dataSize) && m_isRunning; ++i)
    {
        if ((i == 0) || (asdu.m_qualifier == StructureQualifier::Sequence))
        {
            Q_ASSERT(m_dataSize >= (m_index + 3));
            m_objAddress = std::uint8_t(dataRef[m_index++]);
            m_objAddress |= std::uint8_t(dataRef[m_index++]) << 8;
            m_objAddress |= std::uint8_t(dataRef[m_index++]) << 16;
        }
        else
            m_objAddress++;
        switch (asdu.m_msgType)
        {
        case MessageDataType::M_ME_NC_1:
            parseFloat(dataRef);
            break;
        case MessageDataType::M_BO_NA_1:
            parseBitString(dataRef);
            break;
        default:
            /// TODO: Realise all?
            stopUnpacking();
            break;
        }
    }
}

void ASDUUnpacker::stopUnpacking() noexcept
{
    m_isRunning = false;
}

void ASDUUnpacker::parseFloat(const QByteArray &data) noexcept
{
    constexpr auto parsedDataSize = sizeof(float) + sizeof(std::uint8_t);
    if (m_dataSize >= (m_index + parsedDataSize))
    {
        DataTypes::FloatStruct signal;
        signal.sigAdr = m_objAddress;
        signal.sigVal = *reinterpret_cast<const float *>(&data.data()[m_index]);
        m_index += sizeof(float);
        signal.sigQuality = std::uint8_t(data[m_index++]);
        emit unpackedObjectAddress(m_objAddress);
        emit unpacked(signal);
    }
    else
        stopUnpacking();
}

void ASDUUnpacker::parseBitString(const QByteArray &data) noexcept
{
    constexpr auto parsedDataSize = sizeof(std::uint32_t) + sizeof(std::uint8_t);
    if (m_dataSize >= (m_index + parsedDataSize))
    {
        DataTypes::BitStringStruct signal;
        signal.sigAdr = m_objAddress;
        signal.sigVal = *reinterpret_cast<const std::uint32_t *>(&data.data()[m_index]);
        m_index += sizeof(std::uint32_t);
        signal.sigQuality = std::uint8_t(data[m_index++]);
        emit unpackedObjectAddress(m_objAddress);
        emit unpacked(signal);
    }
    else
        stopUnpacking();
}

} // namespace Iec104
