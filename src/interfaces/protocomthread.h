#pragma once

#include "../s2/s2datatypes.h"
#include "baseinterfacethread.h"
#include "protocomprivate.h"

#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <gen/datatypes.h>
#include <gen/error.h>

class LogClass;

namespace Interface
{

class ProtocomThread : public BaseInterfaceThread
{
    typedef QQueue<QByteArray> ByteQueue;
    Q_OBJECT
public:
    explicit ProtocomThread(QObject *parent = nullptr);
    ~ProtocomThread();

private:
    bool isFirstBlock;
    Proto::Commands m_commandSent;
    Proto::Commands m_responseReceived;

    void parseRequest(const CommandStruct &cmdStr);

    void parseResponse() override;

    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    void processFileFromDisk(DataTypes::FilesEnum fileNum);

    quint16 extractLength(const QByteArray &ba);
    void appendInt16(QByteArray &ba, quint16 size);

    bool isCommandExist(int cmd);
    // Если размер меньше MaxSegmenthLength то сегмент считается последним (единственным)
    inline bool isOneSegment(unsigned len);
    inline bool isSplitted(unsigned len);

    // TODO вынести в отдельный класс как static методы?
    QByteArray prepareOk(bool isStart, byte cmd);
    QByteArray prepareError();
    //    QByteArray prepareBlock(Proto::CommandStruct &cmdStr, Proto::Starters startByte = Proto::Starters::Request);
    QByteArray prepareBlock(Proto::Commands cmd, const QByteArray &data = QByteArray(),
        Proto::Starters startByte = Proto::Starters::Request);
    void writeBlock(Proto::Commands cmd, quint8 arg1, const QByteArray &arg2);

    void handleBitString(const QByteArray &ba, quint16 sigAddr);
#ifdef Q_OS_LINUX
    void handleUnixTime(const QByteArray &ba, quint16 sigAddr);
#endif
    template <std::size_t N> void handleBitStringArray(const QByteArray &ba, std::array<quint16, N> arr_addr);
    void handleBitStringArray(const QByteArray &ba, quint16 start_addr);
    void handleFloat(const QByteArray &ba, quint32 sigAddr);
    void handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount);
    void handleSinglePoint(const QByteArray &ba, const quint16 addr);
    void handleFile(QByteArray &ba, DataTypes::FilesEnum addr, DataTypes::FileFormat format);
    void handleInt(const byte num);
    void handleOk();
    void handleError(int errorCode = 0);
    void handleProgress(const quint64 count);
    void handleMaxProgress(const quint64 count);
    void handleRawBlock(const QByteArray &ba, quint32 blkNum);
    inline void handleCommand(const QByteArray &ba);
    void handleTechBlock(const QByteArray &ba, quint32 blkNum);

    const QMap<Interface::Commands, Proto::Commands> protoCommandMap {
        { C_ReqTime, Proto::ReadTime }, { C_ReqBSI, Proto::ReadBlkStartInfo }, //
        { C_ReqBSIExt, Proto::ReadBlkStartInfoExt },                           //
        { C_StartFirmwareUpgrade, Proto::WriteUpgrade },                       //
        { C_SetNewConfiguration, Proto::WriteBlkTech },                        // 1
        { C_WriteUserValues, Proto::WriteBlkData },                            // 1
        { C_EraseJournals, Proto::EraseTech },                                 // 1
        { C_ReqBitStrings, Proto::ReadProgress },                              //
        { C_EraseTechBlock, Proto::EraseTech },                                // 1
        { C_Test, Proto::Test },                                               //
        { C_WriteSingleCommand, Proto::WriteSingleCommand },                   // 1
        { C_ReqTuningCoef, Proto::ReadBlkAC },                                 //
        { C_WriteTuningCoef, Proto::WriteBlkAC },                              //
        { C_ReqBlkData, Proto::ReadBlkData },                                  //
        { C_ReqBlkDataA, Proto::ReadBlkDataA },                                //
        { C_ReqBlkDataTech, Proto::ReadBlkTech },                              //
        { C_ReqOscInfo, Proto::ReadBlkTech },                                  //
        { C_WriteBlkDataTech, Proto::WriteBlkTech },                           // 1
        { C_Reboot, Proto::WriteBlkCmd },                                      // 1
        //        { C_ReqAlarms, Proto::FakeReadAlarms },                                //
        { C_GetMode, Proto::ReadMode },           //
        { C_SetMode, Proto::WriteMode },          // 1
        { C_WriteHardware, Proto::WriteHardware } // 1
    };

signals:
    void writeDataAttempt(const QByteArray);

public slots:
    void processReadBytes(QByteArray &ba) override;
};

}
