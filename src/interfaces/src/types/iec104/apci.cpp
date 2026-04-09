#include "interfaces/types/iec104/apci.h"

namespace Iec104
{

constexpr inline std::uint8_t headerTag = 0x68;            ///< Header start for protocol.
constexpr inline std::uint8_t maxAsduSize
    = std::numeric_limits<std::uint8_t>::max() - apciSize; ///< Max ASDU size for protocol.

APCI::APCI(const std::uint8_t asduSize) noexcept : m_asduSize(asduSize) { }

APCI::APCI(const ControlBlock controlBlock, const std::uint8_t asduSize) noexcept
    : m_ctrlBlock(controlBlock)
    , m_asduSize(asduSize)
{
}

APCI::APCI(const APCI &rhs) noexcept : m_ctrlBlock(rhs.m_ctrlBlock), m_asduSize(rhs.m_asduSize) { }

APCI::APCI(APCI &&rhs) noexcept : m_ctrlBlock(std::move(rhs.m_ctrlBlock)), m_asduSize(std::move(rhs.m_asduSize)) { }

const APCI &APCI::operator=(const APCI &rhs) noexcept
{
    m_ctrlBlock = rhs.m_ctrlBlock;
    m_asduSize = rhs.m_asduSize;
    return *this;
}

void APCI::updateControlBlock(const ControlBlock controlBlock) noexcept
{
    m_ctrlBlock = controlBlock;
}

QByteArray APCI::toByteArray() const
{
    try
    {
        if (m_asduSize > maxAsduSize)
            throw ApciError::InvalidAsduSize;
        auto ctrlData = m_ctrlBlock.data();
        QByteArray apci;
        apci.append(headerTag);
        apci.append(m_asduSize + controlBlockSize);
        apci.append(StdFunc::toByteArray(ctrlData));
        return apci;
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        throw;
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        throw;
    }
}

APCI APCI::fromByteArray(const QByteArray &data)
{
    try
    {
        if (data.size() != apciSize)
            throw ApciError(ApciError::InvalidDataLength);
        else if (data[0] != headerTag)
            throw ApciError(ApciError::InvalidStartByte);
        auto ctrlBytes = data.right(controlBlockSize);
        auto ctrlData = *reinterpret_cast<std::uint32_t *>(ctrlBytes.data());
        auto ctrlBlock = ControlBlock::fromData(ctrlData);
        std::uint8_t asduSize = data[1] - controlBlockSize;
        return APCI { ctrlBlock, asduSize };
    }
    catch (const ApciError &e)
    {
        pringApciError(e);
        throw;
    }
    catch (const std::exception &e)
    {
        qDebug() << "Unhandled exception: " << e.what();
        throw;
    }
}

bool operator!=(const APCI &lhs, const APCI &rhs) noexcept
{
    return ((lhs.m_ctrlBlock != rhs.m_ctrlBlock) || (lhs.m_asduSize != rhs.m_asduSize));
}

} // namespace Iec104
