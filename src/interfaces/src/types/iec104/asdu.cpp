#include "interfaces/types/iec104/asdu.h"

#include <gen/datatypes.h>
#include <gen/std_ext.h>
#include <gen/stdfunc.h>

namespace Iec104
{

namespace helper
{
    inline constexpr auto byte_max = std::numeric_limits<std::uint8_t>::max();
} // namespace helper

ASDU::ASDU() noexcept
    : m_qualifier(StructureQualifier::Sequence)
    , m_elements(1)
    , m_isTest(false)
    , m_confirmation(Confirmation::Positive)
    , m_cause(CauseOfTransmission::Activation)
    , m_originatorAddr(0)
{
}

ASDU::ASDU(const std::uint16_t bsAddress) noexcept : ASDU()
{
    m_bsAddress = bsAddress;
}

void ASDU::setRequestData(const uint24 address, const std::uint32_t data) noexcept
{
    m_msgType = MessageDataType::C_BO_NA_1;
    m_data = StdFunc::toByteArray(address);
    m_data.append(StdFunc::toByteArray(data));
}

void ASDU::setRequestData(const uint24 address, const float data) noexcept
{
    m_msgType = MessageDataType::C_SE_NC_1;
    m_data = StdFunc::toByteArray(address);
    m_data.append(StdFunc::toByteArray(data));
    m_data.append('\x00');
}

void ASDU::setRequestData(const uint24 address, const bool data) noexcept
{
    m_msgType = MessageDataType::C_SC_NA_1;
    m_data = StdFunc::toByteArray(address);
    m_data.append(data ? '\x01' : '\x00');
}

void ASDU::setRequestData(const std::uint8_t group) noexcept
{
    m_msgType = MessageDataType::C_IC_NA_1;
    uint24 nullAddress { 0 };
    m_data = StdFunc::toByteArray(nullAddress);
    m_data.append(group + 20);
}

QByteArray ASDU::toByteArray() const noexcept
{
    QByteArray asdu;
    asdu.append(std_ext::to_underlying(m_msgType));
    std::uint8_t value { std_ext::to_underlying(m_qualifier) };
    if (value)
        value = m_elements | (value << 7);
    else
        value = m_elements & (helper::byte_max >> 1);
    asdu.append(value);

    value = std_ext::to_underlying(m_confirmation);
    if (value)
        value = value << 6;
    value = value | std_ext::to_underlying(m_cause);
    if (m_isTest)
        value = value | (1 << 7);
    else
        value = value & helper::byte_max >> 1;
    asdu.append(value);

    asdu.append(m_originatorAddr);
    asdu.append(StdFunc::toByteArray(m_bsAddress));
    if (!m_data.isEmpty())
        asdu.append(m_data);
    return asdu;
}

ASDU ASDU::fromByteArray(const QByteArray &data) noexcept
{
    ASDU retVal;
    retVal.m_msgType = static_cast<MessageDataType>(data[0]);
    retVal.m_qualifier = static_cast<StructureQualifier>(data[1] >> 7);
    retVal.m_elements = (data[1] & maxElements);
    retVal.m_isTest = (data[2] >> 7);
    retVal.m_confirmation = static_cast<Confirmation>((data[2] << 1) >> 7);
    retVal.m_cause = static_cast<CauseOfTransmission>(data[2] & (helper::byte_max >> 2));
    retVal.m_originatorAddr = data[3];
    retVal.m_bsAddress = ((std::uint8_t(data[5]) << 8) | std::uint8_t(data[4]));
    retVal.m_data = data.mid(asduHeaderSize);
    return retVal;
}

} // namespace Iec104
