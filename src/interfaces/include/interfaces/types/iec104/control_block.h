#pragma once

#include "interfaces/types/iec104/rscounter.h"
#include <interfaces/types/iec104/unnumbered_control.h>
#include <libavm-gen/error.h>

#include <QByteArray>
#include <memory>

namespace Iec104
{

/// \brief Формат поля.
/// \see ГОСТ Р МЭК 60870-5-104-2004
enum class FrameFormat : std::uint16_t
{
    Information = 0, ///< Информация. 0 в первом бите.
    Supervisory = 1, ///< Функция контроля. 0 во втором бите и 1 в первом.
    Unnumbered = 3   ///< Функция упраления. 1 во втором бите и 1 в первом.
};

constexpr inline std::size_t controlBlockSize = sizeof(std::uint32_t);

/// \brief Контрольный блок пакета.
/// \details Включает в себя:
/// - 1. Счетчики - RSCounter;
/// - 2. Формат пакета - I, S, U;
/// - 3. Контрольную функцию и аргумент для U-пакетов.
class ControlBlock
{
    /// \brief Converting saved data to I-format (information transfer format).
    std::uint32_t toInfoTransferFormat() const noexcept;
    /// \brief Converting saved data to S-format (supervisory control format).
    std::uint32_t toNumberedSupervisoryFunction() const noexcept;
    /// \brief Converting saved data to U-format (unnumbered control format).
    std::uint32_t toUnnumberedControlFunction() const ;

public:
    RSCounter m_counters;
    FrameFormat m_format;

    // Для U-пакета.
    ControlFunc m_func;
    ControlArg m_arg;

    /// \brief Default c-tor.
    explicit ControlBlock(const FrameFormat fmt = FrameFormat::Information, const std::uint16_t k = 12,
        const std::uint16_t w = 8) noexcept;
    /// \brief Copy c-tor.
    ControlBlock(const ControlBlock &rhs) noexcept;
    /// \brief Assignment operator.
    const ControlBlock &operator=(const ControlBlock &rhs) noexcept;
    /// \brief Condition operator.
    friend bool operator==(const ControlBlock &lhs, const ControlBlock &rhs) noexcept;
    /// \brief Not condition operator.
    friend bool operator!=(const ControlBlock &lhs, const ControlBlock &rhs) noexcept;

    /// \brief Converting the stored control block data to a protocol representation.
    std::uint32_t data() const ;
    /// \brief Converting the received byte array to a control block object.
    static ControlBlock fromData(const std::uint32_t data) ;
};

} // namespace Iec104

using SharedControlBlock = std::shared_ptr<Iec104::ControlBlock>;
