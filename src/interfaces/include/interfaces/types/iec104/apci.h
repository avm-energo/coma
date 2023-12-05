#pragma once

#include <QByteArray>
#include <gen/stdfunc.h>
#include <interfaces/types/iec104/control_block.h>

namespace Iec104
{

constexpr inline std::uint8_t headerTag = 0x68;

/// \brief Application Protocol Control Information (APCI) class.
class APCI
{
private:
    ControlBlock m_ctrlBlock;
    std::uint8_t m_asduSize;

    QByteArray createHeader() const noexcept;

public:
    explicit APCI(const ControlBlock &controlBlock, const std::uint8_t asduSize = 0) noexcept;

    QByteArray toIFormatByteArray() const noexcept;
    QByteArray toSFormatByteArray() const noexcept;

    template <ControlFunc func, ControlArg arg> //
    QByteArray toUFormatByteArray() const noexcept;
};

} // namespace Iec104
