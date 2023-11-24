#pragma once

#include <bitset>
#include <cstdint>

namespace Iec104
{

/// \brief Control function type.
enum class ControlFunc : std::uint8_t
{
    StartDataTransfer = 0, ///< STARTDT
    StopDataTransfer,      ///< STOPDT
    TestFrame              ///< TESTFR
};

/// \brief Control function argument type.
enum class ControlArg : std::uint8_t
{
    Activate = 0, ///< ACT
    Confirm       ///< CON
};

/// \brief The data format control block structure.
struct ControlBlock
{
    std::uint16_t sent, received;

    /// \brief I-format
    std::uint32_t toInfoTransferFormat() const noexcept
    {
        return (std::uint32_t((received << 1) & 0xFFFE) << 16) | ((sent << 1) & 0xFFFE);
    }

    /// \brief S-format
    std::uint32_t toNumberedSupervisoryFunction() const noexcept
    {
        return (std::uint32_t((received << 1) & 0xFFFE) << 16) | std::uint16_t(0x0001);
    }

    /*     U-Frame Function         7 6 5 4 3 2 1 0 Hexa Value
    Test Frame Activation           0 1 0 0 0 0 1 1    0x43
    Test Frame Confirmation         1 0 0 0 0 0 1 1    0x83
    Stop Data Transfer Activation   0 0 0 1 0 0 1 1    0x13
    Stop Data Transfer Confirmation 0 0 1 0 0 0 1 1    0x23
    Start Data Transfer Activation  0 0 0 0 0 1 1 1    0x07
    Stop Data Transfer Confirmation 0 0 0 0 1 0 1 1    0x0B */

    /// \brief U-format
    template <ControlFunc func, ControlArg arg> //
    constexpr std::uint32_t toUnnumberedControlFunction() const noexcept
    {
        std::uint32_t retVal = 1;
        switch (func)
        {
        case ControlFunc::StartDataTransfer:
            retVal = retVal << 2;
            break;
        case ControlFunc::StopDataTransfer:
            retVal = retVal << 4;
            break;
        case ControlFunc::TestFrame:
            retVal = retVal << 6;
            break;
        }
        if constexpr (arg == ControlArg::Confirm)
            retVal = retVal << 1;
        return (retVal | 3);
    }

    constexpr std::uint32_t startDataTransferActivate() const noexcept
    {
        return (1 << 2) | 3; // 0x07
    }

    constexpr std::uint32_t startDataTransferConfirm() const noexcept
    {
        return (1 << 3) | 3; // 0x0B
    }

    constexpr std::uint32_t stopDataTransferActivate() const noexcept
    {
        return (1 << 4) | 3; // 0x13
    }

    constexpr std::uint32_t stopDataTransferConfirm() const noexcept
    {
        return (1 << 5) | 3; // 0x23
    }

    constexpr std::uint32_t testFrameActivate() const noexcept
    {
        return (1 << 6) | 3; // 0x43
    }

    constexpr std::uint32_t testFrameConfirm() const noexcept
    {
        return (1 << 7) | 3; // 0x83
    }
};

} // namespace Iec104
