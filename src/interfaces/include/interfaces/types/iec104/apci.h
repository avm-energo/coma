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

public:
    explicit APCI(const FrameFormat format, const std::uint8_t asduSize = 0) noexcept;
    explicit APCI(const ControlBlock controlBlock, const std::uint8_t asduSize = 0) noexcept;

    void updateControlBlock(const FrameFormat fmt, const ControlFunc func, const ControlArg arg) noexcept;
    void updateControlBlock(const ControlBlock controlBlock) noexcept;

    QByteArray toByteArray() const noexcept;
};

} // namespace Iec104
