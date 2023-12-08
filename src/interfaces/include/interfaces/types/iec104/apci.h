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
public:
    ControlBlock m_ctrlBlock;
    std::uint8_t m_asduSize;

    /// \brief Default c-tor.
    explicit APCI(const std::uint8_t asduSize = 0) noexcept;
    /// \brief Param c-tor.
    explicit APCI(const ControlBlock controlBlock, const std::uint8_t asduSize = 0) noexcept;
    /// \brief Assignment operator.
    const APCI &operator=(const APCI &rhs) noexcept;

    void updateControlBlock(const ControlBlock controlBlock) noexcept;

    tl::expected<QByteArray, ApciError> toByteArray() const noexcept;
    static tl::expected<APCI, ApciError> fromByteArray(const QByteArray &data) noexcept;
};

} // namespace Iec104
