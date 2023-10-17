#pragma once

#include <QObject>
#include <QWaitCondition>
#include <gen/datatypes.h>
#include <interfaces/connection.h>
#include <s2/filestruct.h>

namespace Interface
{

enum class ParserState : std::uint32_t
{
    Starting = 0,     ///< Стартовое состояние парсера
    RequestParsing,   ///< Выполняется функция parseRequest
    CommandRequested, ///< Текущая команда отправлена интерфейсу
    ResponseParsing,  ///< Получен ответ от устройства, выполняется parseResponse
    Pending, ///< Парсер в состоянии ожидания, очередь становится неактивной
    Stopping,              ///< Парсер заканчивает своё выполнение
    Undefined = UINT32_MAX ///< ABI compatibility
};

class BaseProtocolParser : public QObject
{
    Q_OBJECT
private:
    std::atomic<ParserState> m_state;
    RequestQueue &m_queue;

    void checkQueue() noexcept;

protected:
    using FileFormat = DataTypes::FileFormat;
    static const QMap<Interface::Commands, CommandRegisters> WSCommandMap;

    quint64 m_progress = 0;
    QMutex m_mutex;
    QByteArray m_readData;
    QWaitCondition m_waiter;
    LogClass m_log;

    void setProgressCount(const quint64 count);
    void setProgressRange(const quint64 count);
    void processOk();
    void processError(int errorCode = 0);

    ParserState getState() const noexcept;
    void setState(const ParserState newState) noexcept;

public:
    CommandStruct m_currentCommand;

    explicit BaseProtocolParser(RequestQueue &queue, QObject *parent = nullptr);

    void clear();
    void wakeUp();
    void activate() noexcept;

    void filePostpone(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format);

    void finishCommand();
    virtual void parseRequest(const CommandStruct &cmdStr) = 0;

signals:
    void stateChanged(const Interface::ParserState newState);
    void finished();
    void responseSend(const Interface::DeviceResponse &resp);
    void sendDataToInterface(const QByteArray &ba);

public slots:
    void run();
    virtual void processReadBytes(QByteArray ba) = 0;
    virtual void parseResponse() = 0;
};

}
