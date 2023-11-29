#include <interfaces/types/iec104/apci.h>

namespace Iec104
{

APCI::APCI(const ControlBlock &controlBlock, const std::uint8_t asduSize) noexcept : m_asduSize(asduSize)
{
    m_ctrlBlock.received.store(controlBlock.received.load());
    m_ctrlBlock.sent.store(controlBlock.sent.load());
}

QByteArray APCI::toIFormatByteArray() const noexcept
{
    QByteArray apci;
    apci.append(headerTag);
    apci.append(m_asduSize + sizeof(ControlBlock));
    auto ctrlData { m_ctrlBlock.toInfoTransferFormat() };
    apci.append(StdFunc::toByteArray(ctrlData));
    return apci;
}

QByteArray APCI::toSFormatByteArray() const noexcept
{
    QByteArray apci;
    apci.append(headerTag);
    apci.append(m_asduSize + sizeof(ControlBlock));
    auto ctrlData { m_ctrlBlock.toNumberedSupervisoryFunction() };
    apci.append(StdFunc::toByteArray(ctrlData));
    return apci;
}

template <ControlFunc func, ControlArg arg> //
QByteArray APCI::toUFormatByteArray() const noexcept
{
    QByteArray apci;
    apci.append(headerTag);
    apci.append(m_asduSize + sizeof(ControlBlock));
    std::uint32_t ctrlData { m_ctrlBlock.toUnnumberedControlFunction<func, arg>() };
    apci.append(StdFunc::toByteArray(ctrlData));
    return apci;
}

} // namespace Iec104
