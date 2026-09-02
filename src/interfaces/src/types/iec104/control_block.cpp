#include "interfaces/types/iec104/control_block.h"

namespace Iec104
{

ControlBlock::ControlBlock(const FrameFormat fmt, const std::uint16_t k, const std::uint16_t w) noexcept
    : m_counters(k, w)
    , m_format(fmt)
    , m_func(ControlFunc::StartDataTransfer)
    , m_arg(ControlArg::Activate)
{
}

ControlBlock::ControlBlock(const ControlBlock &rhs) noexcept
    : m_counters(rhs.m_counters)
    , m_format(rhs.m_format)
    , m_func(rhs.m_func)
    , m_arg(rhs.m_arg)
{
}

const ControlBlock &ControlBlock::operator=(const ControlBlock &rhs) noexcept
{
    m_counters = rhs.m_counters;
    m_format = rhs.m_format;
    m_func = rhs.m_func;
    m_arg = rhs.m_arg;
    return *this;
}

bool operator==(const ControlBlock &lhs, const ControlBlock &rhs) noexcept
{
    return ((lhs.m_counters == rhs.m_counters) && (lhs.m_format == rhs.m_format) && (lhs.m_func == rhs.m_func)
        && (lhs.m_arg == rhs.m_arg));
}

bool operator!=(const ControlBlock &lhs, const ControlBlock &rhs) noexcept
{
    return ((lhs.m_counters != rhs.m_counters) || (lhs.m_format != rhs.m_format) || (lhs.m_func != rhs.m_func)
        || (lhs.m_arg != rhs.m_arg));
}

std::uint32_t ControlBlock::toInfoTransferFormat() const noexcept
{
    return m_counters.toInfoTransferFormat();
}

std::uint32_t ControlBlock::toNumberedSupervisoryFunction() const noexcept
{
    return m_counters.toNumberedSupervisoryFunction();
}

std::uint32_t ControlBlock::toUnnumberedControlFunction() const
{
    try
    {
        return UnnumberedControl::getValue(m_func, m_arg);
    }
    catch (const ApciError &e)
    {
        throw;
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        throw;
    }
}

std::uint32_t ControlBlock::data() const
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
        throw ApciError(ApciError::InvalidFrameFormat);
    }
}

ControlBlock ControlBlock::fromData(const std::uint32_t data)
{
    try
    {
        ControlBlock retVal;
        auto fmt = static_cast<FrameFormat>(data & std_ext::to_underlying(FrameFormat::Unnumbered));
        std::uint16_t lopart = data & std::numeric_limits<std::uint16_t>::max(), hipart = data >> 16;
        switch (fmt)
        {
        case FrameFormat::Unnumbered:
        {
            retVal.m_format = fmt;
            auto value = UnnumberedControl::fromValue(data);
            retVal.m_func = value.first;
            retVal.m_arg = value.second;
            break;
        }
        case FrameFormat::Supervisory:
            if ((hipart & 1) == 0 && lopart == 1)
            {
                retVal.m_format = fmt;
                retVal.m_counters.setSupervisory(data);
                break;
            }
            else
                throw ApciError(ApciError::InvalidFrameFormat);
        default:
            if (((hipart & 1) == 0) && ((lopart & 1) == 0))
            {
                retVal.m_format = FrameFormat::Information;
                retVal.m_counters.setInformation(data);
                break;
            }
            else
                throw ApciError(ApciError::InvalidFrameFormat);
        }
        return retVal;
    }
    catch (const ApciError &e)
    {
        throw;
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        throw;
    }
}

} // namespace Iec104
