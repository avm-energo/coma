#pragma once

#include "interfaces/types/iec104/control_block.h"

namespace Iec104
{

ControlBlock::ControlBlock(std::uint16_t sent, std::uint16_t received) noexcept : m_sent(sent), m_received(received)
{
}

ControlBlock::ControlBlock(const ControlBlock &lhs) noexcept
    : m_sent(lhs.m_sent.load()), m_received(lhs.m_received.load())
{
}

std::uint16_t ControlBlock::getSent() const noexcept
{
    return m_sent.load();
}

std::uint16_t ControlBlock::getReceived() const noexcept
{
    return m_received.load();
}

void ControlBlock::setSent(const std::uint16_t sent) noexcept
{
    m_sent.store(sent);
}

void ControlBlock::setReveived(const std::uint16_t received) noexcept
{
    m_received.store(received);
}

std::uint32_t ControlBlock::toInfoTransferFormat() const noexcept
{
    return (std::uint32_t((getReceived() << 1) & 0xFFFE) << 16) | ((getSent() << 1) & 0xFFFE);
}

std::uint32_t ControlBlock::toNumberedSupervisoryFunction() const noexcept
{
    return (std::uint32_t((getReceived() << 1) & 0xFFFE) << 16) | std::uint16_t(0x0001);
}

} // namespace Iec104
