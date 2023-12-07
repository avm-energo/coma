#include "interfaces/types/iec104/apci.h"

namespace Iec104
{

APCI::APCI(const FrameFormat format, const std::uint8_t asduSize) noexcept : m_ctrlBlock(format), m_asduSize(asduSize)
{
}

APCI::APCI(const ControlBlock controlBlock, const std::uint8_t asduSize) noexcept
    : m_ctrlBlock(controlBlock), m_asduSize(asduSize)
{
}

void APCI::updateControlBlock(const FrameFormat fmt, const ControlFunc func, const ControlArg arg) noexcept
{
    m_ctrlBlock.format = fmt;
    m_ctrlBlock.func = func;
    m_ctrlBlock.arg = arg;
}

void APCI::updateControlBlock(const ControlBlock controlBlock) noexcept
{
    m_ctrlBlock = controlBlock;
}

QByteArray APCI::toByteArray() const noexcept
{
    auto ctrlData = m_ctrlBlock.data();
    if (ctrlData.has_value())
    {
        QByteArray apci;
        apci.append(headerTag);
        apci.append(m_asduSize + sizeof(std::uint32_t));
        apci.append(StdFunc::toByteArray(*ctrlData));
        return apci;
    }
    else
    {
        // TODO: error dispatching
        return QByteArray {};
    }
}

} // namespace Iec104
