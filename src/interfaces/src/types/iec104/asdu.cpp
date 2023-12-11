#include "interfaces/types/iec104/asdu.h"

#include <gen/std_ext.h>
#include <gen/stdfunc.h>

namespace Iec104
{

namespace helper
{
    inline constexpr auto byte_max = std::numeric_limits<std::uint8_t>::max();
}

ASDU::ASDU() noexcept = default;

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
    asdu.append(StdFunc::toByteArray(m_address));
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
    retVal.m_address = ((data[5] << 8) | data[4]);
    retVal.m_data = data.mid(asduHeaderSize);
    return retVal;
}

} // namespace Iec104
