#pragma once

#include <gen/integers.h>

namespace Device
{

namespace detail
{
    constexpr inline u32 HTH_FNC = 0x00000200;     // частота не в допуске (Э)
    constexpr inline u32 HTH_LS = 0x00000100;      // сигналы малы (Э)
    constexpr inline u32 HTH_CONFIG = 0x00000080;  // в модуле нет конфигурации
    constexpr inline u32 HTH_REGPARS = 0x00000040; // в модуле нет настроечных параметров
    constexpr inline u32 HTH_ADC = 0x00000020;     // проблема с АЦП (Э)
    constexpr inline u32 HTH_1PPS = 0x00000010;    // нет сигнала 1PPS
    constexpr inline u32 HTH_REL = 0x00000008;     // неисправность выходных реле (Д)
    constexpr inline u32 HTH_TUPP = 0x00000004;    // перегрев модуля
} // namespace detail

/// \brief Класс-обёртка для состояния устройства.
class Health final
{
private:
    u32 m_health;

public:
    explicit Health() = delete;
    explicit Health(const u32 health) noexcept : m_health(health)
    {
    }

    /// \brief Проверка на наличие конфигурации в устройстве.
    inline bool isNoConfig() const noexcept
    {
        return (m_health & detail::HTH_CONFIG);
    }

    /// \brief Проверка на наличие регулировочных коэффициентов в устройстве.
    inline bool isNoTuneCoef() const noexcept
    {
        return (m_health & detail::HTH_REGPARS);
    }
};

} // namespace Device

using DeviceHealth = Device::Health;
