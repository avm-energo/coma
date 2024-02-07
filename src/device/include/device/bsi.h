#include <gen/datatypes.h>

namespace Device
{

/// \brief Структура, описывающая блок базовой информации (BSI) подключенного устройства.
struct BlockStartupInfo final
{
    u32 MTypeB;     ///< Тип базовой платы.
    u32 MTypeM;     ///< Тип мезонинной платы.
    u32 HwverB;     ///< Аппаратная версия базовой платы.
    u32 Fwver;      ///< Версия ВПО.
    u32 Rst;        ///<
    u32 RstCount;   ///< Счётчик перезапусков устройства.
    u32 UIDLow;     ///< UID, младшая часть.
    u32 UIDMid;     ///< UID, средняя часть.
    u32 UIDHigh;    ///< UID, старшая часть.
    u32 SerialNumB; ///< Серийный номер базовой платы.
    u32 SerialNumM; ///< Серийный номер мезонинной платы.
    u32 HwverM;     ///< Аппаратная версия мезонинной платы.
    u32 SerialNum;  ///< Серийный номер устройства.
    u32 Cfcrc;      ///< Контрольная сумма регулировочных коэффициентов.
    u32 Hth;        ///< Состояние устройства.
};

/// \brief Класс для работы с блоком BSI.
class BsiHolder final : public QObject
{
private:
    BlockStartupInfo m_bsi;

public:
    /// \brief Default c-tor.
    explicit BsiHolder(QObject *parent = nullptr) noexcept;

    const BlockStartupInfo &getBSI() const noexcept;
    u16 getDeviceType() const noexcept;
    QString getDeviceName() const noexcept;
    QString getUID() const noexcept;

    bool isOutdatedFirmware(const u32 configVersion) const noexcept;

public slots:
    void update(const DataTypes::BitStringStruct &value);
};

} // namespace Device
