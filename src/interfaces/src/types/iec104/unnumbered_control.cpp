#include "interfaces/types/iec104/unnumbered_control.h"

#include <QDebug>

namespace Iec104
{

std::uint32_t UnnumberedControl::getValue(const ControlFunc func, const ControlArg arg)
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
            throw ApciError(ApciError::InvalidControlFunc);
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
            throw ApciError(ApciError::InvalidControlFunc);
        }
    }
    else
        throw ApciError(ApciError::InvalidControlArg);
}

std::pair<ControlFunc, ControlArg> UnnumberedControl::fromValue(const std::uint32_t value)
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
        throw ApciError(ApciError::InvalidControlValue);
        ;
    }
}

void pringApciError(ApciError e)
{
    const QMap<ApciError, QString> apciErMap = { { ApciError::InvalidControlFunc, "Неправильная функция управления" },
        { ApciError::InvalidControlArg, "Неправильный аргумент управления" },
        { ApciError::InvalidControlValue, "Неправильное значение управления" },
        { ApciError::InvalidFrameFormat, "Некорректный формат фрейма" },
        { ApciError::InvalidDataLength, "Некорректная длина данных" },
        { ApciError::InvalidAsduSize, "Некорректная длина ASDU" },
        { ApciError::InvalidStartByte, "Неправильный стартовый байт" } };

    qDebug() << "Ошибка разбора APCI: " << apciErMap[e];
}

} // namespace Iec104
