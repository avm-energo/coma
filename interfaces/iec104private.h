#pragma once

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

// определения типа данных
// TYPE IDENTIFICATION

#define M_SP_NA_1 1  // Single-point information
#define M_DP_NA_1 3  // Double-point information
#define M_ST_NA_1 5  // Step position information
#define M_BO_NA_1 7  // Bitstring of 32 bit
#define M_ME_NA_1 9  // Measured value, normalized value
#define M_ME_NC_1 13 // Measured value, short floating point value
#define M_SP_TB_1 30 // Single-point information with time tag CP56Time2a
#define M_DP_TB_1 31 // Double-point information with time tag CP56Time2a
#define M_ST_TB_1 32 // Step position information with time tag CP56Time2a
#define M_ME_TD_1 34 // Measured value, normalized value with time tag CP56Time2a
#define M_ME_TF_1 36 // Measured value, short floating point value with time tag CP56Time2a
#define M_IT_TB_1 37 // Integrated totals value with time tag CP56Time2a
#define C_SC_NA_1 45 // Single command
#define C_DC_NA_1 46 // Double command
#define C_RC_NA_1 47 // Regulating step command
#define C_SE_NA_1 48 // Set point command, normalised value
#define C_SE_NC_1 50
#define C_BO_NA_1 51  // Bitstring of 32 bit
#define C_SC_TA_1 58  // Single command with time tag CP56Time2a
#define C_DC_TA_1 59  // Double command with time tag CP56Time2a
#define M_EI_NA_1 70  // End of initialization
#define C_IC_NA_1 100 // Interrrogation command
#define C_CI_NA_1 101 // Counter interrrogation command
#define C_CS_NA_1 103 // Clock syncronization command
#define C_TS_NB_1 104 // Test command
#define C_RP_NC_1 105 // Reset process command

#define F_FR_NA_1 120 // File ready
#define F_SR_NA_1 121 // Section ready
#define F_SC_NA_1 122 // Call file
#define F_LS_NA_1 123 // Last section
#define F_AF_NA_1 124 // Confirm receive
#define F_SG_NA_1 125 // Segment of the section
#define F_DR_TA_1 126 // Directory

// CAUSE OF TRANSMISSION: define causes

#define COT_PERIODIC 1
#define COT_BACKGROUND 2
#define COT_SPONTANEOUS 3
#define COT_INITIALISED 4
#define COT_REQUEST 5
#define COT_ACTIVATION 6
#define COT_ACTIVATIONCONFIRM 7
#define COT_DEACTIVATION 8
#define COT_DEACTIVATIONCONFIRM 9
#define COT_ACTIVATIONTERMINATION 10
#define COT_REMOTECOMMAND 11
#define COT_LOCALCOMMAND 12
#define COT_FILETRANSFER 13
#define COT_INTERROGATION 20

#define SECTIONSIZE 2048
#define SEGMENTSIZE 230
#define OFFSET 13
#define FHSIZE 16
#define SYSTEM_JOUR_SIZE 65568

namespace Commands104
{
enum CommandRegisters
{
    SetNewConfigurationReg = 801,
    StartFirmwareUpgradeReg = 802,
    StartWorkingChannelReg = 803,
    EraseJournalsReg = 806,
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

}
Q_DECLARE_METATYPE(Commands104::CommandStruct)
