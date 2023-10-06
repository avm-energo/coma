#pragma once

#define PROTOCOM_DEBUG

#include <interfaces/parsers/baseprotocolparser.h>
#include <interfaces/types/protocom_types.h>

namespace Interface
{

class ProtocomParser : public BaseProtocolParser
{
    Q_OBJECT
public:
    explicit ProtocomParser(RequestQueue &queue, QObject *parent = nullptr);
    ~ProtocomParser();

public slots:
    void processReadBytes(QByteArray ba) override;

private:
    bool isFirstBlock;
    Proto::Commands m_commandSent;
    Proto::Commands m_responseReceived;
    QList<QByteArray> m_longBlockChunks;
    int m_sentBytesCount;

    /// \brief Anonymous structure for representing type of the connected board.
    struct
    {
        quint8 mTypeB = 0;
        quint8 mTypeM = 0;
        bool isEmpty()
        {
            return ((mTypeB == 0) || (mTypeM == 0));
        }
    } boardType;

    void parseRequest(const CommandStruct &cmdStr) override;
    void parseResponse() override;

    void writeLog(const QByteArray &ba, Interface::Direction dir = Interface::NoDirection);
    void appendInt16(QByteArray &ba, quint16 data);
    bool isOneSegment(quint16 length);
    bool isSplitted(quint16 length);
    bool isValidIncomingData(const QByteArray &data);
    void processFileFromDisk(S2::FilesEnum fileNum);
    void progressFile(const QByteArray &data);

    QByteArray prepareOk(bool isStart, byte cmd);
    QByteArray prepareError();
    QByteArray prepareBlock(Proto::Commands cmd, const QByteArray &data = QByteArray(),
        Proto::Starters startByte = Proto::Starters::Request);
    void writeBlock(Proto::Commands cmd, const QByteArray &arg2);

#ifdef Q_OS_LINUX
    void processUnixTime(const QByteArray &ba);
#endif
    void processU32(const QByteArray &ba, quint16 startAddr);
    void processFloat(const QByteArray &ba, quint32 startAddr);
    void processInt(const byte num);
    void processSinglePoint(const QByteArray &ba, const quint16 startAddr);
    void processOk();
    void processError(int errorCode = 0);
    void processBlock(const QByteArray &ba, quint32 blkNum);
    void processTechBlock(const QByteArray &ba, quint32 blkNum);

    const QMap<Interface::Commands, Proto::Commands> protoCommandMap {
        { Commands::C_ReqTime, Proto::ReadTime },                      // 12
        { Commands::C_ReqBSI, Proto::ReadBlkStartInfo },               // 12
        { Commands::C_ReqBSIExt, Proto::ReadBlkStartInfoExt },         // 12
        { Commands::C_StartFirmwareUpgrade, Proto::WriteUpgrade },     // 12
        { Commands::C_SetNewConfiguration, Proto::WriteBlkTech },      // 12
        { Commands::C_WriteUserValues, Proto::WriteBlkData },          // 12
        { Commands::C_EraseJournals, Proto::EraseTech },               // 12
        { Commands::C_ReqProgress, Proto::ReadProgress },              // 12
        { Commands::C_EraseTechBlock, Proto::EraseTech },              // 12
        { Commands::C_Test, Proto::Test },                             // 12
        { Commands::C_WriteSingleCommand, Proto::WriteSingleCommand }, // 12
        { Commands::C_ReqTuningCoef, Proto::ReadBlkAC },               // 12
        { Commands::C_WriteTuningCoef, Proto::WriteBlkAC },            // 12
        { Commands::C_ReqBlkData, Proto::ReadBlkData },                // 12
        { Commands::C_ReqBlkDataA, Proto::ReadBlkDataA },              // 12
        { Commands::C_ReqBlkDataTech, Proto::ReadBlkTech },            // 12
        { Commands::C_ReqOscInfo, Proto::ReadBlkTech },                // 12
        { Commands::C_WriteBlkDataTech, Proto::WriteBlkTech },         // 12
        { Commands::C_Reboot, Proto::WriteBlkCmd },                    // 12
        { Commands::C_GetMode, Proto::ReadMode },                      // 12
        { Commands::C_SetMode, Proto::WriteMode },                     // 12
        { Commands::C_WriteHardware, Proto::WriteHardware }            // 12
    };
};

}
