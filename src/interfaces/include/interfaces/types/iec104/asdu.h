#pragma once

#include <QByteArray>
#include <QObject>
#include <gen/uint24.h>
#include <limits>

namespace Iec104
{
Q_NAMESPACE

/// \brief Data type in message, type identification
/// \see https://infosys.beckhoff.com/english.php?content=../content/1033/tf6500_tc3_iec60870_5_10x/984447883.html
enum class MessageDataType : std::uint8_t
{
    M_SP_NA_1 = 1,  ///< Single-point information.
    M_SP_TA_1 = 2,  ///< Single-point information with time tag.
    M_DP_NA_1 = 3,  ///< Double-point information.
    M_DP_TA_1 = 4,  ///< Double-point information with time tag.
    M_ST_NA_1 = 5,  ///< Step position information.
    M_ST_TA_1 = 6,  ///< Step position information with time tag.
    M_BO_NA_1 = 7,  ///< Bitstring of 32 bit.
    M_BO_TA_1 = 8,  ///< Bitstring of 32 bit with time tag.
    M_ME_NA_1 = 9,  ///< Measured value, normalized value.
    M_ME_TA_1 = 10, ///< Measured value, normalized value with time tag.
    M_ME_NB_1 = 11, ///< Measured value, scaled value.
    M_ME_TB_1 = 12, ///< Measured value, scaled value wit time tag.
    M_ME_NC_1 = 13, ///< Measured value, short floating point value - измеренные данные с плавающей запятой.
    M_ME_TC_1 = 14, ///< Measured value, short floating point number with time tag.
    M_IT_NA_1 = 15, ///< Integrated totals.
    M_IT_TA_1 = 16, ///< Integrated totals with time tag.
    M_EP_TA_1 = 17, ///< Event of protection equipment with time tag.
    M_EP_TB_1 = 18, ///< Packed start events of protection equipment with time tag.
    M_EP_TC_1 = 19, ///< Packed output circuit information of protection equipment with time tag.
    M_PS_NA_1 = 20, ///< Packed single point information with status change detection.
    M_ME_ND_1 = 21, ///< Measured value, normalized value without quality descriptor.
    M_SP_TB_1 = 30, ///< Single-point information with time tag CP56Time2a.
    M_DP_TB_1 = 31, ///< Double-point information with time tag CP56Time2a.
    M_ST_TB_1 = 32, ///< Step position information with time tag CP56Time2a.
    M_BO_TB_1 = 33, ///< Bitstring of 32 bit with time tag CP56Time2a.
    M_ME_TD_1 = 34, ///< Measured value, normalized value with time tag CP56Time2a.
    M_ME_TE_1 = 35, ///< Measured value, scaled value with time tag CP56Time2a.
    M_ME_TF_1 = 36, ///< Measured value, short floating point value with time tag CP56Time2a.
    M_IT_TB_1 = 37, ///< Integrated totals value with time tag CP56Time2a.
    M_EP_TD_1 = 38, ///< Event of protection equipment with time tag CP56Time2a.
    M_EP_TE_1 = 39, ///< Packed start events of protection equipment with time tag CP56Time2a.
    M_EP_TF_1 = 40, ///< Packed output circuit information of protection equipment with time tag CP56Time2a.
    S_IT_TC_1 = 41, ///< TODO: possible wrong type?
    C_SC_NA_1 = 45, ///< Single command.
    C_DC_NA_1 = 46, ///< Double command.
    C_RC_NA_1 = 47, ///< Regulating step command.
    C_SE_NA_1 = 48, ///< Set point command, normalised value.
    C_SE_NB_1 = 49, ///< Set point command, scaled value.
    C_SE_NC_1 = 50, ///< Set point command, short floating point number.
    C_BO_NA_1 = 51, ///< Bitstring of 32 bit.
    C_SC_TA_1 = 58, ///< Single command with time tag CP56Time2a.
    C_DC_TA_1 = 59, ///< Double command with time tag CP56Time2a.
    C_RC_TA_1 = 60, ///< Regulating step command with time tag CP56Time2a.
    C_SE_TA_1 = 61, ///< Measured value, normalised value command with time tag CP56Time2a.
    C_SE_TB_1 = 62, ///< Measured value, scaled value command with time tag CP56Time2a.
    C_SE_TC_1 = 63, ///< Measured value, short floating point number command with time tag CP56Time2a.
    C_BO_TA_1 = 64, ///< Bitstring of 32 bit command with time tag CP56Time2a.
    M_EI_NA_1 = 70, ///< End of initialization - подтверждение окончания инициализации.
    C_IC_NA_1 = 100, ///< Interrrogation command.
    C_CI_NA_1 = 101, ///< Counter interrrogation command.
    C_RD_NA_1 = 102, ///< Read command.
    C_CS_NA_1 = 103, ///< Clock syncronization command.
    C_TS_NA_1 = 104, ///< Test command.
    C_RP_NA_1 = 105, ///< Reset process command.
    C_CD_NA_1 = 106, ///< Delay acquisition command.
    C_TS_TA_1 = 107, ///< Test command with time tag CP56Time2a.
    P_ME_NA_1 = 110, ///< Parameter of measured values, normalized value.
    P_ME_NB_1 = 111, ///< Parameter of measured values, scaled value.
    P_ME_NC_1 = 112, ///< Parameter of measured values, short floating point number.
    P_AC_NA_1 = 113, ///< Parameter activation.
    F_FR_NA_1 = 120, ///< File ready - файл готов.
    F_SR_NA_1 = 121, ///< Section ready - секция готова.
    F_SC_NA_1 = 122, ///< Call directory, select file, call file, call section - запрос файла, секции.
    F_LS_NA_1 = 123, ///< Last section, last segment - последняя секция, последнй сегмент.
    F_AF_NA_1 = 124, ///< Confirm receive, ACK file, ACK section - подтверждение файла, секции.
    F_SG_NA_1 = 125, ///< Segment of the section - сегмент.
    F_DR_TA_1 = 126, ///< Directory.
    F_SC_NB_1 = 127  ///< 127..255 - Reserved (user area).
};
Q_ENUM_NS(Iec104::MessageDataType)

/// \brief Structure qualifier type specifies how
/// information objects or elements are addressed.
enum class StructureQualifier : std::uint8_t
{
    Sequence = 0, ///< Sequence of information objects.
    Single = 1    ///< Single information object.
};

/// \brief The confirmation of an activation
/// requested by a primary application function.
enum class Confirmation : std::uint8_t
{
    Positive = 0, ///< Positive confirmation.
    Negative = 1  ///< Negative confirmation.
};

/// \brief Cause of transmission (COT) is used to control
/// the routing of messages both on the communication network.
enum class CauseOfTransmission : std::uint8_t
{
    GroupRequest = 0,       ///< Group request.
    Periodic = 1,           ///< Cyclic data.
    Background,             ///< Background scan.
    Spontaneous = 3,        ///< Spontaneous data (спорадика).
    Initialized,            ///< End of initialization.
    Request,                ///< Read request.
    Activation = 6,         ///< Command activation.
    ActivationConfirm,      ///< Confirmation of command activation.
    Deactivation,           ///< Command abortion.
    DeactivationConfirm,    ///< Confirmation of command abortion.
    ActivationTermination,  ///< Termination of command activation.
    RemoteCommand,          ///< Response due to remote command.
    LocalCommand,           ///< Response due to local command.
    FileTransfering = 13,   ///< File access.
    Interrogation = 20,     ///< Station interrogation (general).
    CounterInterrogate = 37 ///< Counter interrogation (general).
};

inline constexpr auto maxElements = std::numeric_limits<std::uint8_t>::max() >> 1;
inline constexpr auto asduHeaderSize = 6;

/// \brief ASDU representation of the IEC-60870-5-104 protocol.
class ASDU
{
public:
    MessageDataType m_msgType;
    StructureQualifier m_qualifier;
    std::uint8_t m_elements;
    bool m_isTest;
    Confirmation m_confirmation;
    CauseOfTransmission m_cause;
    std::uint8_t m_originatorAddr;
    std::uint16_t m_bsAddress;
    QByteArray m_data;

    /// \brief Default c-tor.
    explicit ASDU() noexcept;
    /// \brief C-tor with params.
    explicit ASDU(const std::uint16_t bsAddress) noexcept;

    /// \brief Writing a bitstring data.
    void setRequestData(const uint24 address, const std::uint32_t data) noexcept;
    /// \brief Writing a float data.
    void setRequestData(const uint24 address, const float data) noexcept;
    /// \brief Writing a single command data.
    void setRequestData(const uint24 address, const bool data) noexcept;
    /// \brief Requesting an interrogate group.
    void setRequestData(const std::uint8_t group) noexcept;

    /// \brief Converting the ASDU object to a byte array.
    QByteArray toByteArray() const noexcept;
    /// \brief Converting the received byte array to an ASDU object.
    static ASDU fromByteArray(const QByteArray &data) noexcept;
};

} // namespace Iec104
