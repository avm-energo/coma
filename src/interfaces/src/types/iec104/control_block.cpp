#pragma once

#include "interfaces/types/iec104/control_block.h"

namespace Iec104
{

ControlBlock::ControlBlock(const FrameFormat fmt, std::uint16_t sent, std::uint16_t received) noexcept
    : sent(sent), received(received), format(fmt), func(ControlFunc::StartDataTransfer), arg(ControlArg::Activate)
{
}

ControlBlock::ControlBlock(const ControlBlock &rhs) noexcept
    : sent(rhs.sent), received(rhs.received), format(rhs.format), func(rhs.func), arg(rhs.arg)
{
}

const ControlBlock &ControlBlock::operator=(const ControlBlock &rhs) noexcept
{
    sent = rhs.sent;
    received = rhs.received;
    format = rhs.format;
    func = rhs.func;
    arg = rhs.arg;
    return *this;
}

std::uint32_t ControlBlock::toInfoTransferFormat() const noexcept
{
    return (std::uint32_t(received << 1) << 16) | (sent << 1);
}

std::uint32_t ControlBlock::toNumberedSupervisoryFunction() const noexcept
{
    return (std::uint32_t(received << 1) << 16) | std::uint16_t(0x0001);
}

tl::expected<std::uint32_t, ControlBlockError> ControlBlock::toUnnumberedControlFunction() const noexcept
{
    return UnnumberedControl::getValue(func, arg);
}

tl::expected<std::uint32_t, ControlBlockError> ControlBlock::data() const noexcept
{
    switch (format)
    {
    case FrameFormat::Information:
        return toInfoTransferFormat();
    case FrameFormat::Supervisory:
        return toNumberedSupervisoryFunction();
    case FrameFormat::Unnumbered:
        return toUnnumberedControlFunction();
    default:
        return tl::unexpected(ControlBlockError::UndefinedFrameFormat);
    }
}

} // namespace Iec104
