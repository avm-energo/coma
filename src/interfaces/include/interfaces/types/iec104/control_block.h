#pragma once

#include <avm-gen/error.h>
#include <interfaces/types/iec104/unnumbered_control.h>

#include <QByteArray>
#include <memory>

namespace Iec104
{

/// \brief The frame format type.
enum class FrameFormat : std::uint16_t
{
    Information = 0,
    Supervisory = 1,
    Unnumbered = 3
};

constexpr inline std::size_t controlBlockSize = sizeof(std::uint32_t);
constexpr inline auto controlMax = std::numeric_limits<std::uint16_t>::max() >> 1;

/// \brief The data format control block structure.
class ControlBlock
{
private:
    /// \brief Converting saved data to I-format (information transfer format).
    std::uint32_t toInfoTransferFormat() const noexcept;
    /// \brief Converting saved data to S-format (supervisory control format).
    std::uint32_t toNumberedSupervisoryFunction() const noexcept;
    /// \brief Converting saved data to U-format (unnumbered control format).
    std::uint32_t toUnnumberedControlFunction() const ;

public:
    std::uint16_t m_sent, m_received;
    FrameFormat m_format;
    ControlFunc m_func;
    ControlArg m_arg;

    /// \brief Default c-tor.
    explicit ControlBlock(const FrameFormat fmt = FrameFormat::Information, //
        const std::uint16_t sent = 0, const std::uint16_t received = 0) noexcept;
    /// \brief Copy c-tor.
    ControlBlock(const ControlBlock &rhs) noexcept;
    /// \brief Assignment operator.
    const ControlBlock &operator=(const ControlBlock &rhs) noexcept;
    /// \brief Condition operator.
    friend bool operator==(const ControlBlock &lhs, const ControlBlock &rhs) noexcept;
    /// \brief Not condition operator.
    friend bool operator!=(const ControlBlock &lhs, const ControlBlock &rhs) noexcept;

    /// \brief Converting the stored control block data to a protocol representation.
    std::uint32_t data() const ;
    /// \brief Converting the received byte array to a control block object.
    static ControlBlock fromData(const std::uint32_t data) ;
};

} // namespace Iec104

using SharedControlBlock = std::shared_ptr<Iec104::ControlBlock>;
