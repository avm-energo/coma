#pragma once

#include "../gen/datatypes.h"
#include "../gen/error.h"
#include "protocomprivate.h"

#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
class LogClass;
class ProtocomThread : public QObject
{
    typedef QQueue<QByteArray> ByteQueue;
    Q_OBJECT
public:
    explicit ProtocomThread(QObject *parent = nullptr);
    ~ProtocomThread();

    void setReadDataChunk(const QByteArray &readDataChunk);
    void appendReadDataChunk(const QByteArray &readDataChunk);
    void wakeUp();

    void parse();

    void clear();

private:
    QByteArray m_readData;
    bool isCommandRequested = false;
    quint64 progress = 0;
    void finish(Error::Msg msg);

    void parseResponse(QByteArray ba);
    void parseRequest(const Proto::CommandStruct &cmdStr);
    void handle(const Proto::Commands cmd);

    QMutex _mutex;

    QWaitCondition _waiter;
    void writeLog(QByteArray ba, Proto::Direction dir = Proto::NoDirection);
    void writeLog(Error::Msg msg, Proto::Direction dir = Proto::NoDirection)
    {
        writeLog(QVariant::fromValue(msg).toByteArray(), dir);
    }

    Proto::CommandStruct m_currentCommand;
    QPair<quint64, QByteArray> m_buffer;
    void checkQueue();
    void fileHelper(DataTypes::FilesEnum fileNum);

    quint16 extractLength(const QByteArray &ba);
    void appendInt16(QByteArray &ba, quint16 size);

    bool isCommandExist(int cmd);
    // Если размер меньше MaxSegmenthLength то сегмент считается последним (единственным)
    inline bool isOneSegment(unsigned len);
    inline bool isSplitted(unsigned len);

    // TODO вынести в отдельный класс как static методы?
    QByteArray prepareOk(bool isStart, byte cmd);
    QByteArray prepareError();
    QByteArray prepareBlock(Proto::CommandStruct &cmdStr, Proto::Starters startByte = Proto::Starters::Request);
    QByteArray prepareBlock(
        Proto::Commands cmd, QByteArray &data, Proto::Starters startByte = Proto::Starters::Request);
    ByteQueue prepareLongBlk(Proto::CommandStruct &cmdStr);

    void handleBitString(const QByteArray &ba, quint16 sigAddr);
#ifdef __linux
    void handleUnixTime(const QByteArray &ba, quint16 sigAddr);
#endif
    template <std::size_t N> void handleBitStringArray(const QByteArray &ba, std::array<quint16, N> arr_addr);
    void handleBitStringArray(const QByteArray &ba, quint16 start_addr);
    void handleFloat(const QByteArray &ba, quint32 sigAddr);
    void handleFloatArray(const QByteArray &ba, quint32 sigAddr, quint32 sigCount);
    void handleSinglePoint(const QByteArray &ba, const quint16 addr);
    void handleFile(QByteArray &ba, DataTypes::FilesEnum addr, Queries::FileFormat format);
    void handleInt(const byte num);
    void handleBool(const bool status = true, int errorSize = 0, int errorCode = 0);
    void handleProgress(const quint64 progress);
    void handleMaxProgress(const quint64 progress);
    void handleRawBlock(const QByteArray &ba, quint32 blkNum);
    inline void handleCommand(const QByteArray &ba);
    void handleTechBlock(const QByteArray &ba, quint32 blkNum);
signals:
    void writeDataAttempt(const QByteArray);
    void errorOccurred(Error::Msg);
    /// Like a QIODevice::readyRead()
    void readyRead();
};
