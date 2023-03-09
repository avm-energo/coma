#pragma once

#include <QObject>

constexpr quint32 BSIGROUP = 1;
constexpr quint32 STARTUPGROUP = 2;
constexpr quint32 ALARMGROUP = 3;
constexpr quint32 MAINFLOATGROUP = 4;
constexpr quint32 MAINBITSTRINGGROUP = 4;
constexpr quint32 TIMEGROUP = 15;

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

/// \brief Data type in message, type identification
/// \see https://infosys.beckhoff.com/english.php?content=../content/1033/tf6500_tc3_iec60870_5_10x/984447883.html
enum TypeId : char
{
    M_SP_NA_1 = 1,  ///< Single-point information
    M_SP_TA_1 = 2,  ///< Single-point information with time tag
    M_DP_NA_1 = 3,  ///< Double-point information
    M_DP_TA_1 = 4,  ///< Double-point information with time tag
    M_ST_NA_1 = 5,  ///< Step position information
    M_ST_TA_1 = 6,  ///< Step position information with time tag
    M_BO_NA_1 = 7,  ///< Bitstring of 32 bit
    M_BO_TA_1 = 8,  ///< Bitstring of 32 bit with time tag
    M_ME_NA_1 = 9,  ///< Measured value, normalized value
    M_ME_TA_1 = 10, ///< Measured value, normalized value with time tag
    M_ME_NB_1 = 11, ///< Measured value, scaled value
    M_ME_TB_1 = 12, ///< Measured value, scaled value wit time tag
    M_ME_NC_1 = 13, ///< Measured value, short floating point value - измеренные данные с плавающей запятой
    M_ME_TC_1 = 14, ///< Measured value, short floating point number with time tag
    M_IT_NA_1 = 15, ///< Integrated totals
    M_IT_TA_1 = 16, ///< Integrated totals with time tag
    M_EP_TA_1 = 17, ///< Event of protection equipment with time tag
    M_EP_TB_1 = 18, ///< Packed start events of protection equipment with time tag
    M_EP_TC_1 = 19, ///< Packed output circuit information of protection equipment with time tag
    M_PS_NA_1 = 20, ///< Packed single point information with status change detection
    M_ME_ND_1 = 21, ///< Measured value, normalized value without quality descriptor
    M_SP_TB_1 = 30, ///< Single-point information with time tag CP56Time2a
    M_DP_TB_1 = 31, ///< Double-point information with time tag CP56Time2a
    M_ST_TB_1 = 32, ///< Step position information with time tag CP56Time2a
    M_BO_TB_1 = 33, ///< Bitstring of 32 bit with time tag CP56Time2a
    M_ME_TD_1 = 34, ///< Measured value, normalized value with time tag CP56Time2a
    M_ME_TE_1 = 35, ///< Measured value, scaled value with time tag CP56Time2a
    M_ME_TF_1 = 36, ///< Measured value, short floating point value with time tag CP56Time2a
    M_IT_TB_1 = 37, ///< Integrated totals value with time tag CP56Time2a
    M_EP_TD_1 = 38, ///< Event of protection equipment with time tag CP56Time2a
    M_EP_TE_1 = 39, ///< Packed start events of protection equipment with time tag CP56Time2a
    M_EP_TF_1 = 40, ///< Packed output circuit information of protection equipment with time tag CP56Time2a
    S_IT_TC_1 = 41, ///< TODO: possible wrong type?
    C_SC_NA_1 = 45, ///< Single command
    C_DC_NA_1 = 46, ///< Double command
    C_RC_NA_1 = 47, ///< Regulating step command
    C_SE_NA_1 = 48, ///< Set point command, normalised value
    C_SE_NB_1 = 49, ///< Set point command, scaled value
    C_SE_NC_1 = 50, ///< Set point command, short floating point number
    C_BO_NA_1 = 51, ///< Bitstring of 32 bit
    C_SC_TA_1 = 58, ///< Single command with time tag CP56Time2a
    C_DC_TA_1 = 59, ///< Double command with time tag CP56Time2a
    C_RC_TA_1 = 60, ///< Regulating step command with time tag CP56Time2a
    C_SE_TA_1 = 61, ///< Measured value, normalised value command with time tag CP56Time2a
    C_SE_TB_1 = 62, ///< Measured value, scaled value command with time tag CP56Time2a
    C_SE_TC_1 = 63, ///< Measured value, short floating point number command with time tag CP56Time2a
    C_BO_TA_1 = 64, ///< Bitstring of 32 bit command with time tag CP56Time2a
    M_EI_NA_1 = 70, ///< End of initialization - подтверждение окончания инициализации
    S_CH_NA_1 = 81,  ///<
    S_RP_NA_1 = 82,  ///<
    S_AR_NA_1 = 83,  ///<
    S_KR_NA_1 = 84,  ///<
    S_KS_NA_1 = 85,  ///<
    S_KC_NA_1 = 86,  ///<
    S_ER_NA_1 = 87,  ///<
    S_US_NA_1 = 90,  ///<
    S_UQ_NA_1 = 91,  ///<
    S_UR_NA_1 = 92,  ///<
    S_UK_NA_1 = 93,  ///<
    S_UA_NA_1 = 94,  ///<
    S_UC_NA_1 = 95,  ///<
    C_IC_NA_1 = 100, ///< Interrrogation command
    C_CI_NA_1 = 101, ///< Counter interrrogation command
    C_RD_NA_1 = 102, ///< Read command
    C_CS_NA_1 = 103, ///< Clock syncronization command
    C_TS_NA_1 = 104, ///< Test command
    C_RP_NA_1 = 105, ///< Reset process command
    C_CD_NA_1 = 106, ///< Delay acquisition command
    C_TS_TA_1 = 107, ///< Test command with time tag CP56Time2a
    P_ME_NA_1 = 110, ///< Parameter of measured values, normalized value
    P_ME_NB_1 = 111, ///< Parameter of measured values, scaled value
    P_ME_NC_1 = 112, ///< Parameter of measured values, short floating point number
    P_AC_NA_1 = 113, ///< Parameter activation
    F_FR_NA_1 = 120, ///< File ready - файл готов
    F_SR_NA_1 = 121, ///< Section ready - секция готова
    F_SC_NA_1 = 122, ///< Call directory, select file, call file, call section - запрос файла, секции
    F_LS_NA_1 = 123, ///< Last section, last segment - последняя секция, последнй сегмент
    F_AF_NA_1 = 124, ///< Confirm receive, ACK file, ACK section - подтверждение файла, секции
    F_SG_NA_1 = 125, ///< Segment of the section - сегмент
    F_DR_TA_1 = 126, ///< Directory
    F_SC_NB_1 = 127  ///< 127..255 - Reserved (user area)
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

enum CommandRegisters : quint32
{
    SetNewConfigurationReg = 801,
    StartFirmwareUpgradeReg = 802,
    StartWorkingChannelReg = 803,
    // Моветон EraseJournalsReg = 806,
    SetStartupValuesReg = 900,   ///< Задать начальные значения по всем фазам
    SetStartupPhaseA = 901,      ///< Задать начальные значения по фазе A
    SetStartupPhaseB = 902,      ///< Задать начальные значения по фазе B
    SetStartupPhaseC = 903,      ///< Задать начальные значения по фазе C
    SetStartupUnbounced = 904,   ///< Задать начальные значения по току небаланса
    ClearStartupValuesReg = 905, ///< Сбросить начальные значения по всем фазам
    ClearStartupSetError = 906,  ///< Сбросить ошибку задания начальных значений
    SetTransOff = 907            ///< Послать команду "Трансфоратор отключён"
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
    quint32 address;
    float flarg;
    bool blarg;
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

}
Q_DECLARE_METATYPE(Commands104::CommandStruct)
