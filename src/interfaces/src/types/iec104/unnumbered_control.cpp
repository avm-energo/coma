#pragma once

#include "interfaces/types/iec104/unnumbered_control.h"

#include <QDebug>

namespace Iec104
{

tl::expected<std::uint32_t, ControlBlockError> //
UnnumberedControl::getValue(const ControlFunc func, const ControlArg arg) noexcept
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
        default:
            return tl::unexpected(ControlBlockError::UndefinedControlFunc);
        }
    }
    else if (arg == ControlArg::Confirm)
    {
        switch (func)
        {
        case ControlFunc::StartDataTransfer:
            return startDataTransferConfirm();
        case ControlFunc::StopDataTransfer:
            return stopDataTransferConfirm();
        case ControlFunc::TestFrame:
            return testFrameConfirm();
        default:
            return tl::unexpected(ControlBlockError::UndefinedControlFunc);
        }
    }
    else
        return tl::unexpected(ControlBlockError::UndefinedControlArg);
}

tl::expected<std::pair<ControlFunc, ControlArg>, ControlBlockError> //
UnnumberedControl::fromValue(const std::uint32_t value) noexcept
{
    switch (value)
    {
    case startDataTransferActivate():
        return std::pair { ControlFunc::StartDataTransfer, ControlArg::Activate };
    case startDataTransferConfirm():
        return std::pair { ControlFunc::StartDataTransfer, ControlArg::Confirm };
    case stopDataTransferActivate():
        return std::pair { ControlFunc::StopDataTransfer, ControlArg::Activate };
    case stopDataTransferConfirm():
        return std::pair { ControlFunc::StopDataTransfer, ControlArg::Confirm };
    case testFrameActivate():
        return std::pair { ControlFunc::TestFrame, ControlArg::Activate };
    case testFrameConfirm():
        return std::pair { ControlFunc::TestFrame, ControlArg::Confirm };
    default:
        qWarning() << "Undefined value, not U-format frame received";
        return tl::unexpected(ControlBlockError::UndefinedControlValue);
    }
}

} // namespace Iec104
