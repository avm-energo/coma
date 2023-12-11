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

    /// \brief Updating the control block inside the APCI unit.
    void updateControlBlock(const ControlBlock controlBlock) noexcept;

    /// \brief Converting the APCI object to a byte array.
    tl::expected<QByteArray, ApciError> toByteArray() const noexcept;
    /// \brief Converting the received byte array to a APCI object.
    static tl::expected<APCI, ApciError> fromByteArray(const QByteArray &data) noexcept;
};

} // namespace Iec104
