#pragma once

#include <cstdint>
#include <gen/std_ext.h>

namespace Iec104
{

/// \brief Control function type.
enum class ControlFunc : std::uint8_t
{
    StartDataTransfer = 2, ///< STARTDT
    StopDataTransfer = 4,  ///< STOPDT
    TestFrame = 6          ///< TESTFR
};

/// \brief Control function argument type.
enum class ControlArg : std::uint8_t
{
    Activate = 0, ///< ACT
    Confirm = 1   ///< CON
};

/// \brief Compile-time calculated value for control block with U-Frame format.
template <ControlFunc func, ControlArg arg> //
struct UnnumberedControlValue
{
private:
    /*     U-Frame Function         7 6 5 4 3 2 1 0 Hexa Value
    Start Data Transfer Activation  0 0 0 0 0 1 1 1    0x07
    Stop Data Transfer Confirmation 0 0 0 0 1 0 1 1    0x0B
    Stop Data Transfer Activation   0 0 0 1 0 0 1 1    0x13
    Stop Data Transfer Confirmation 0 0 1 0 0 0 1 1    0x23
    Test Frame Activation           0 1 0 0 0 0 1 1    0x43
    Test Frame Confirmation         1 0 0 0 0 0 1 1    0x83 */

    /// \brief Algorithm of value calculation in compile-time.
    static constexpr std::uint32_t calculate() noexcept
    {
        std::uint32_t retVal = 1;
        retVal = retVal << std_ext::to_underlying(func);
        retVal = retVal << std_ext::to_underlying(arg);
        return (retVal | 3);
    }

public:
    static constexpr std::uint32_t value = calculate();
};

/// \brief Collection of compile-time and run-time static functions for calculate
/// unnumbered control function in control block with U-Frame format.
struct UnnumberedControl
{
    /// \brief Calculation the value for activation the start of data transfer.
    static constexpr inline std::uint32_t startDataTransferActivate() noexcept
    {
        return UnnumberedControlValue<ControlFunc::StartDataTransfer, ControlArg::Activate>::value; // 0x07
    }

    /// \brief Calculation the value for confirmation the start data transfer.
    static constexpr inline std::uint32_t startDataTransferConfirm() noexcept
    {
        return UnnumberedControlValue<ControlFunc::StartDataTransfer, ControlArg::Confirm>::value; // 0x0B
    }

    /// \brief Calculation the value for activation the stop of data transfer.
    static constexpr inline std::uint32_t stopDataTransferActivate() noexcept
    {
        return UnnumberedControlValue<ControlFunc::StopDataTransfer, ControlArg::Activate>::value; // 0x13
    }

    /// \brief Calculation the value for confirmation the stop of data transfer.
    static constexpr inline std::uint32_t stopDataTransferConfirm() noexcept
    {
        return UnnumberedControlValue<ControlFunc::StopDataTransfer, ControlArg::Confirm>::value; // 0x23
    }

    /// \brief Calculation the value for activation the test frame transfer.
    static constexpr inline std::uint32_t testFrameActivate() noexcept
    {
        return UnnumberedControlValue<ControlFunc::TestFrame, ControlArg::Activate>::value; // 0x43
    }

    /// \brief Calculation the value for confirmation the test frame transfer.
    static constexpr inline std::uint32_t testFrameConfirm() noexcept
    {
        return UnnumberedControlValue<ControlFunc::TestFrame, ControlArg::Confirm>::value; // 0x83
    }

    static inline std::uint32_t getValue(const ControlFunc func, const ControlArg arg) noexcept
    {
        if (arg == ControlArg::Activate)
        {
            switch (func)
            {
            case ControlFunc::StartDataTransfer:
                return startDataTransferActivate();
            case ControlFunc::StopDataTransfer:
                return stopDataTransferActivate();
            case ControlFunc::TestFrame:
                return testFrameActivate();
            }
        }
        else
        {
            switch (func)
            {
            case ControlFunc::StartDataTransfer:
                return startDataTransferConfirm();
            case ControlFunc::StopDataTransfer:
                return stopDataTransferConfirm();
            case ControlFunc::TestFrame:
                return testFrameConfirm();
            }
        }
    }
};

} // namespace Iec104
