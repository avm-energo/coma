#pragma once

#include <QByteArray>
#include <atomic>
#include <interfaces/types/iec104/unnumbered_control.h>
#include <memory>

namespace Iec104
{

enum class FrameFormat : std::uint16_t
{
    Information = 0,
    Supervisory = 1,
    Unnumbered = 3
};

/// \brief The data format control block structure.
class ControlBlock
{
private:
    std::atomic_uint16_t m_sent, m_received;

public:
    explicit ControlBlock(std::uint16_t sent = 0, std::uint16_t received = 0) noexcept;
    explicit ControlBlock(const ControlBlock &lhs) noexcept;

    std::uint16_t getSent() const noexcept;
    std::uint16_t getReceived() const noexcept;
    void setSent(const std::uint16_t sent) noexcept;
    void setReveived(const std::uint16_t received) noexcept;

    /// \brief I-format
    std::uint32_t toInfoTransferFormat() const noexcept;

    /// \brief S-format
    std::uint32_t toNumberedSupervisoryFunction() const noexcept;

    /// \brief U-format
};

} // namespace Iec104

using SharedControlBlock = std::shared_ptr<Iec104::ControlBlock>;
