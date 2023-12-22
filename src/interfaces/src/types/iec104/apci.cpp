#include "interfaces/types/iec104/apci.h"

namespace Iec104
{

constexpr inline std::uint8_t headerTag = 0x68; ///< Header start for protocol.
constexpr inline std::uint8_t maxAsduSize
    = std::numeric_limits<std::uint8_t>::max() - apciSize; ///< Max ASDU size for protocol.

APCI::APCI(const std::uint8_t asduSize) noexcept : m_asduSize(asduSize)
{
}

APCI::APCI(const ControlBlock controlBlock, const std::uint8_t asduSize) noexcept
    : m_ctrlBlock(controlBlock), m_asduSize(asduSize)
{
}

APCI::APCI(const APCI &rhs) noexcept : m_ctrlBlock(rhs.m_ctrlBlock), m_asduSize(rhs.m_asduSize)
{
}

APCI::APCI(APCI &&rhs) noexcept : m_ctrlBlock(std::move(rhs.m_ctrlBlock)), m_asduSize(std::move(rhs.m_asduSize))
{
}

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

tl::expected<QByteArray, ApciError> APCI::toByteArray() const noexcept
{
    if (m_asduSize > maxAsduSize)
        return tl::unexpected(ApciError::InvalidAsduSize);
    auto ctrlData = m_ctrlBlock.data();
    if (ctrlData.has_value())
    {
        QByteArray apci;
        apci.append(headerTag);
        apci.append(m_asduSize + controlBlockSize);
        apci.append(StdFunc::toByteArray(*ctrlData));
        return apci;
    }
    else
        return tl::unexpected(ctrlData.error());
}

tl::expected<APCI, ApciError> APCI::fromByteArray(const QByteArray &data) noexcept
{
    if (data.size() != apciSize)
        return tl::unexpected(ApciError::InvalidDataLength);
    else if (data[0] != headerTag)
        return tl::unexpected(ApciError::InvalidStartByte);
    else
    {
        auto ctrlBytes = data.right(controlBlockSize);
        auto ctrlData = *reinterpret_cast<std::uint32_t *>(ctrlBytes.data());
        auto ctrlBlock = ControlBlock::fromData(ctrlData);
        if (ctrlBlock.has_value())
        {
            std::uint8_t asduSize = data[1] - controlBlockSize;
            return APCI { ctrlBlock.value(), asduSize };
        }
        else
            return tl::unexpected(ctrlBlock.error());
    }
}

} // namespace Iec104
