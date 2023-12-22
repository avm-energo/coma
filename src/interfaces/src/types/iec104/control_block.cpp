#include "interfaces/types/iec104/control_block.h"

namespace Iec104
{

ControlBlock::ControlBlock(const FrameFormat fmt, std::uint16_t sent, std::uint16_t received) noexcept
    : m_sent(sent)
    , m_received(received)
    , m_format(fmt)
    , m_func(ControlFunc::StartDataTransfer)
    , m_arg(ControlArg::Activate)
{
}

ControlBlock::ControlBlock(const ControlBlock &rhs) noexcept
    : m_sent(rhs.m_sent), m_received(rhs.m_received), m_format(rhs.m_format), m_func(rhs.m_func), m_arg(rhs.m_arg)
{
}

const ControlBlock &ControlBlock::operator=(const ControlBlock &rhs) noexcept
{
    m_sent = rhs.m_sent;
    m_received = rhs.m_received;
    m_format = rhs.m_format;
    m_func = rhs.m_func;
    m_arg = rhs.m_arg;
    return *this;
}

std::uint32_t ControlBlock::toInfoTransferFormat() const noexcept
{
    return (std::uint32_t(m_received << 1) << 16) | (m_sent << 1);
}

std::uint32_t ControlBlock::toNumberedSupervisoryFunction() const noexcept
{
    return (std::uint32_t(m_received << 1) << 16) | std::uint16_t(0x0001);
}

tl::expected<std::uint32_t, ApciError> ControlBlock::toUnnumberedControlFunction() const noexcept
{
    return UnnumberedControl::getValue(m_func, m_arg);
}

tl::expected<std::uint32_t, ApciError> ControlBlock::data() const noexcept
{
    switch (m_format)
    {
    case FrameFormat::Information:
        return toInfoTransferFormat();
    case FrameFormat::Supervisory:
        return toNumberedSupervisoryFunction();
    case FrameFormat::Unnumbered:
        return toUnnumberedControlFunction();
    default:
        return tl::unexpected(ApciError::InvalidFrameFormat);
    }
}

tl::expected<ControlBlock, ApciError> ControlBlock::fromData(const std::uint32_t data) noexcept
{
    ControlBlock retVal;
    auto fmt = static_cast<FrameFormat>(data & std_ext::to_underlying(FrameFormat::Unnumbered));
    std::uint16_t lopart = data & std::numeric_limits<std::uint16_t>::max(), hipart = data >> 16;
    switch (fmt)
    {
    case FrameFormat::Unnumbered:
        retVal.m_format = fmt;
        if (auto value = UnnumberedControl::fromValue(data); value.has_value())
        {
            retVal.m_func = value->first;
            retVal.m_arg = value->second;
            break;
        }
        else
            return tl::unexpected(value.error());
    case FrameFormat::Supervisory:
        if ((hipart & 1) == 0 && lopart == 1)
        {
            retVal.m_format = fmt;
            retVal.m_received = hipart >> 1;
            break;
        }
        else
            return tl::unexpected(ApciError::InvalidFrameFormat);
    default:
        if (((hipart & 1) == 0) && ((lopart & 1) == 0))
        {
            retVal.m_format = FrameFormat::Information;
            retVal.m_sent = lopart >> 1;
            retVal.m_received = hipart >> 1;
            break;
        }
        else
            return tl::unexpected(ApciError::InvalidFrameFormat);
    }
    return tl::expected<ControlBlock, ApciError>(retVal);
}

} // namespace Iec104
