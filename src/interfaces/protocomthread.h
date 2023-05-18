#pragma once

#include "baseinterfacethread.h"
#include "protocomprivate.h"

namespace Interface
{

class ProtocomThread : public BaseInterfaceThread
{
    //    typedef QQueue<QByteArray> ByteQueue;
    Q_OBJECT
public:
    explicit ProtocomThread(QObject *parent = nullptr);
    ~ProtocomThread();

private:
    bool isFirstBlock;
    Proto::Commands m_commandSent;
    Proto::Commands m_responseReceived;
    QList<QByteArray> m_longBlockChunks;
    int m_sentBytesCount;

    void parseRequest(const CommandStruct &cmdStr) override;

    void parseResponse() override;

    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    void appendInt16(QByteArray &ba, quint16 data);

    inline bool isOneSegment(unsigned len);
    inline bool isSplitted(unsigned len);

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
        { C_ReqTime, Proto::ReadTime },                      // 12
        { C_ReqBSI, Proto::ReadBlkStartInfo },               // 12
        { C_ReqBSIExt, Proto::ReadBlkStartInfoExt },         // 12
        { C_StartFirmwareUpgrade, Proto::WriteUpgrade },     // 12
        { C_SetNewConfiguration, Proto::WriteBlkTech },      // 12
        { C_WriteUserValues, Proto::WriteBlkData },          // 12
        { C_EraseJournals, Proto::EraseTech },               // 12
        { C_ReqProgress, Proto::ReadProgress },              // 12
        { C_EraseTechBlock, Proto::EraseTech },              // 12
        { C_Test, Proto::Test },                             // 12
        { C_WriteSingleCommand, Proto::WriteSingleCommand }, // 12
        { C_ReqTuningCoef, Proto::ReadBlkAC },               // 12
        { C_WriteTuningCoef, Proto::WriteBlkAC },            // 12
        { C_ReqBlkData, Proto::ReadBlkData },                // 12
        { C_ReqBlkDataA, Proto::ReadBlkDataA },              // 12
        { C_ReqBlkDataTech, Proto::ReadBlkTech },            // 12
        { C_ReqOscInfo, Proto::ReadBlkTech },                // 12
        { C_WriteBlkDataTech, Proto::WriteBlkTech },         // 12
        { C_Reboot, Proto::WriteBlkCmd },                    // 12
        { C_GetMode, Proto::ReadMode },                      // 1
        { C_SetMode, Proto::WriteMode },                     // 12
        { C_WriteHardware, Proto::WriteHardware }            // 12
    };

signals:

public slots:
    void processReadBytes(QByteArray ba) override;
};
}
