#pragma once

#include <QByteArray>
#include <gen/stdfunc.h>
#include <interfaces/types/iec104/control_block.h>

namespace Iec104
{

constexpr inline std::size_t apciSize = controlBlockSize + 2; ///< APCI size.

/// \brief Application Protocol Control Information (APCI) class.
class APCI
{
private:
    ControlBlock m_ctrlBlock;
    std::uint8_t m_asduSize;

public:
    explicit APCI(const std::uint8_t asduSize = 0) noexcept;
    explicit APCI(const ControlBlock controlBlock, const std::uint8_t asduSize = 0) noexcept;

    void updateControlBlock(const FrameFormat fmt, const ControlFunc func, const ControlArg arg) noexcept;
    void updateControlBlock(const ControlBlock controlBlock) noexcept;
    void updateASDUSize(const std::uint8_t newSize) noexcept;

    tl::expected<QByteArray, ApciError> toByteArray() const noexcept;
    static tl::expected<APCI, ApciError> fromByteArray(const QByteArray &data) noexcept;
};

} // namespace Iec104
