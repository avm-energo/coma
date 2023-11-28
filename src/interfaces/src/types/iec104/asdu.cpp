#include <gen/std_ext.h>
#include <gen/stdfunc.h>
#include <interfaces/types/iec104/asdu.h>

namespace Iec104
{

namespace helper
{
    inline constexpr auto byte_max = std::numeric_limits<std::uint8_t>::max();
}

QByteArray ASDU::toByteArray() const noexcept
{
    QByteArray asdu;
    asdu.append(std_ext::to_underlying(m_msgType));
    std::uint8_t value { std_ext::to_underlying(m_qualifier) };
    if (value)
        value = m_elements | (value << 7);
    else
        value = m_elements & helper::byte_max >> 1;
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

} // namespace Iec104
