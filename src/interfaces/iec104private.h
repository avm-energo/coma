#pragma once
#include "interfacesettings.h"

#include <QObject>
#define STARTUPGROUP 2
#define BSIGROUP 1
#define TIMEGROUP 15
#define ALARMGROUP 3
#define MAINFLOATGROUP 4
#define MAINBITSTRINGGROUP 4

#define BASEADR104 205
#define I104_START 0x68

#define I104_I 0x00
#define I104_S 0x01
#define I104_U 0x03
#define I104_WRONG 0xFF

#define I104_W 8

#define I104_STARTDT_ACT 0x07
#define I104_STARTDT_CON 0x0B
#define I104_STOPDT_ACT 0x13
#define I104_STOPDT_CON 0x23
#define I104_TESTFR_ACT 0x43
#define I104_TESTFR_CON 0x83u

// определения возвращаемого значения функции isIncomeDataValid
#define I104_RCVNORM 0x00
#define I104_RCVSMSIZE 0x01
#define I104_RCVWRONG 0x02

#define SECTIONSIZE 2048
#define SEGMENTSIZE 230
#define OFFSET 13
#define FHSIZE 16
#define SYSTEM_JOUR_SIZE 65568

namespace Commands104
{
Q_NAMESPACE
// определения типа данных
// TYPE IDENTIFICATION
/**
 * \brief Message type IDs
 */
enum TypeId : char
{
    /// Single-point information
    M_SP_NA_1 = 1,
    ///
    M_SP_TA_1 = 2,
    /// Double-point information
    M_DP_NA_1 = 3,
    ///
    M_DP_TA_1 = 4,
    /// Step position information
    M_ST_NA_1 = 5,
    ///
    M_ST_TA_1 = 6,
    /// Bitstring of 32 bit
    M_BO_NA_1 = 7,
    ///
    M_BO_TA_1 = 8,
    /// Measured value, normalized value
    M_ME_NA_1 = 9,
    ///
    M_ME_TA_1 = 10,
    ///
    M_ME_NB_1 = 11,
    ///
    M_ME_TB_1 = 12,
    /// Measured value, short floating point value - измеренные данные с плавающей запятой
    M_ME_NC_1 = 13,
    ///
    M_ME_TC_1 = 14,
    ///
    M_IT_NA_1 = 15,
    ///
    M_IT_TA_1 = 16,
    ///
    M_EP_TA_1 = 17,
    ///
    M_EP_TB_1 = 18,
    ///
    M_EP_TC_1 = 19,
    ///
    M_PS_NA_1 = 20,
    ///
    M_ME_ND_1 = 21,
    /// Single-point information with time tag CP56Time2a
    M_SP_TB_1 = 30,
    /// Double-point information with time tag CP56Time2a
    M_DP_TB_1 = 31,
    /// Step position information with time tag CP56Time2a
    M_ST_TB_1 = 32,
    ///
    M_BO_TB_1 = 33,
    /// Measured value, normalized value with time tag CP56Time2a
    M_ME_TD_1 = 34,
    ///
    M_ME_TE_1 = 35,
    /// Measured value, short floating point value with time tag CP56Time2a  - измеренные данные с плавающей запятой
    M_ME_TF_1 = 36,
    /// Integrated totals value with time tag CP56Time2a
    M_IT_TB_1 = 37,
    ///
    M_EP_TD_1 = 38,
    ///
    M_EP_TE_1 = 39,
    ///
    M_EP_TF_1 = 40,
    ///
    S_IT_TC_1 = 41,
    /// Single command
    C_SC_NA_1 = 45,
    /// Double command
    C_DC_NA_1 = 46,
    /// Regulating step command
    C_RC_NA_1 = 47,
    /// Set point command, normalised value
    C_SE_NA_1 = 48,
    ///
    C_SE_NB_1 = 49,
    /// s
    C_SE_NC_1 = 50,
    /// Bitstring of 32 bit
    C_BO_NA_1 = 51,
    /// Single command with time tag CP56Time2a
    C_SC_TA_1 = 58,
    /// Double command with time tag CP56Time2a
    C_DC_TA_1 = 59,
    ///
    C_RC_TA_1 = 60,
    ///
    C_SE_TA_1 = 61,
    ///
    C_SE_TB_1 = 62,
    ///
    C_SE_TC_1 = 63,
    ///
    C_BO_TA_1 = 64,
    /// End of initialization - подтверждение окончания инициализации
    M_EI_NA_1 = 70,
    ///
    S_CH_NA_1 = 81,
    ///
    S_RP_NA_1 = 82,
    ///
    S_AR_NA_1 = 83,
    ///
    S_KR_NA_1 = 84,
    ///
    S_KS_NA_1 = 85,
    ///
    S_KC_NA_1 = 86,
    ///
    S_ER_NA_1 = 87,
    ///
    S_US_NA_1 = 90,
    ///
    S_UQ_NA_1 = 91,
    ///
    S_UR_NA_1 = 92,
    ///
    S_UK_NA_1 = 93,
    ///
    S_UA_NA_1 = 94,
    ///
    S_UC_NA_1 = 95,
    /// Interrrogation command
    C_IC_NA_1 = 100,
    /// Counter interrrogation command
    C_CI_NA_1 = 101,
    ///
    C_RD_NA_1 = 102,
    /// Clock syncronization command
    C_CS_NA_1 = 103,
    /// Test command
    C_TS_NA_1 = 104,
    /// Reset process command
    C_RP_NA_1 = 105,
    ///
    C_CD_NA_1 = 106,
    ///
    C_TS_TA_1 = 107,
    ///
    P_ME_NA_1 = 110,
    ///
    P_ME_NB_1 = 111,
    ///
    P_ME_NC_1 = 112,
    ///
    P_AC_NA_1 = 113,
    /// File ready - файл готов
    F_FR_NA_1 = 120,
    /// Section ready - секция готова
    F_SR_NA_1 = 121,
    /// Call file - запрос файла, секции
    F_SC_NA_1 = 122,
    /// Last section - последняя секция, последнй сегмент
    F_LS_NA_1 = 123,
    /// Confirm receive - подтверждение файла, секции
    F_AF_NA_1 = 124,
    /// Segment of the section - сегмент
    F_SG_NA_1 = 125,
    /// Directory
    F_DR_TA_1 = 126,
    F_SC_NB_1 = 127
};
Q_ENUM_NS(Commands104::TypeId)
// CAUSE OF TRANSMISSION: define causes

//#define COT_PERIODIC 1
//#define COT_BACKGROUND 2
//#define COT_SPONTANEOUS 3
//#define COT_INITIALISED 4
//#define COT_REQUEST 5
//#define COT_ACTIVATION 6
//#define COT_ACTIVATIONCONFIRM 7
//#define COT_DEACTIVATION 8
//#define COT_DEACTIVATIONCONFIRM 9
//#define COT_ACTIVATIONTERMINATION 10
//#define COT_REMOTECOMMAND 11
//#define COT_LOCALCOMMAND 12
//#define COT_FILETRANSFER 13
//#define COT_INTERROGATION 20

enum CommandRegisters
{
    SetNewConfigurationReg = 801,
    StartFirmwareUpgradeReg = 802,
    StartWorkingChannelReg = 803,
    // Моветон EraseJournalsReg = 806,
    SetStartupValuesReg = 900,
    ClearStartupValuesReg = 905,
};

enum Commands
{
    CM104_REQGROUP,
    CM104_COM51,
    CM104_COM45,
    CM104_COM50,
    CM104_REQFILE,
    CM104_REQCONFIGFILE,
    CM104_WRITEFILE
};

struct CommandStruct
{
    Commands cmd;
    quint32 uintarg;
    float flarg;
    QByteArray ba;
};

struct QualifierVariableStructute
{
    unsigned char Number; // number of Informational Objects
    unsigned char SQ;     // Single <0> or Series <1> of Objects
};

struct CauseOfTransmission
{
    unsigned char cause;     // <0..63> cause number
    unsigned char confirm;   // <0> - positive , <1> - negative
    unsigned char test;      // <0> - not a test, <1> - test
    unsigned char initiator; // number of initiating address
};

struct DataUnitIdentifier
{
    TypeId typeIdent;
    QualifierVariableStructute qualifier;
    CauseOfTransmission cause;
    quint8 commonAdrASDU;
};

struct Iec104Group : BaseGroup<Commands, TypeId>
{
    Iec104Group() = default;
    Iec104Group(QDomElement domElement) : BaseGroup<Commands, TypeId>(domElement)
    {
    }
    // NOTE Need more fileds?
};

}
Q_DECLARE_METATYPE(InterfaceInfo<Commands104::Iec104Group>)
// Q_DECLARE_METATYPE(Commands104::Iec104Group)
Q_DECLARE_METATYPE(Commands104::CommandStruct)
