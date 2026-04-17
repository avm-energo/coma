#pragma once

#include <avm-gen/integers.h>

#include <QObject>
#include <array>
#include <bitset>

namespace Device
{

constexpr quint16 bsiStartReg = 1;
constexpr quint16 bsiCountRegs = 15;

/// \brief Именованные индексы массива BlockStartupInfo.
/// \details Порядок совпадает с порядком регистров, передаваемых устройством
///          (смещение от bsiStartReg). Значения соответствуют тегу <offset>
///          в секции <bsi> XML-конфигурации.
enum BsiIndexes : u32
{
    MTypeB = 0,      ///< Тип базовой платы.
    MTypeM = 1,      ///< Тип мезонинной платы.
    HwverB = 2,      ///< Аппаратная версия базовой платы.
    Fwver = 3,       ///< Версия ВПО.
    Rst = 4,         ///< Причина последнего сброса.
    RstCount = 5,    ///< Счётчик перезапусков устройства.
    UIDLow = 6,      ///< UID, младшая часть.
    UIDMid = 7,      ///< UID, средняя часть.
    UIDHigh = 8,     ///< UID, старшая часть.
    SerialNumB = 9,  ///< Серийный номер базовой платы.
    SerialNumM = 10, ///< Серийный номер мезонинной платы.
    HwverM = 11,     ///< Аппаратная версия мезонинной платы.
    SerialNum = 12,  ///< Серийный номер устройства.
    Cfcrc = 13,      ///< Контрольная сумма регулировочных коэффициентов.
    Hth = 14,        ///< Состояние устройства.
};

/// \brief Блок базовой информации (BSI) — массив из bsiCountRegs значений u32,
///        индексируемый через BsiIndexes.
using BlockStartupInfo = std::array<u32, bsiCountRegs>;

class Bsi : public QObject
{
    Q_OBJECT
public:
    explicit Bsi();
    explicit Bsi(const Bsi &other);
    void clearBsi();
    void clearBsiFill();
    void setData(BsiIndexes index, u32 value);
    void setData(u32 index, u32 value);
    u32 data(BsiIndexes index) const;
    u32 data(u32 index) const;
    bool isFilled();
    Bsi &operator=(const Bsi &other);
    [[nodiscard]] QByteArray toByteArray() const;
    void fromByteArray(const QByteArray &ba);
    u32 size() const;

signals:
    void bsiIsFilled();

private:
    BlockStartupInfo m_bsi;
    std::bitset<bsiCountRegs> m_fillBits;
};

} // namespace Device
