#include "interfaces/types/iec104/unnumbered_control.h"

#include <QDebug>

namespace Iec104
{

tl::expected<std::uint32_t, ApciError> //
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
            return tl::unexpected(ApciError::InvalidControlFunc);
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
            return tl::unexpected(ApciError::InvalidControlFunc);
        }
    }
    else
        return tl::unexpected(ApciError::InvalidControlArg);
}

tl::expected<std::pair<ControlFunc, ControlArg>, ApciError> //
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
        return tl::unexpected(ApciError::InvalidControlValue);
    }
}

} // namespace Iec104
