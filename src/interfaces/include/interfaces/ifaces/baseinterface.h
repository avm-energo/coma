#pragma once

#include <QMutex>
#include <QObject>
#include <QTimer>
#include <atomic>
#include <gen/error.h>
#include <gen/logclass.h>
#include <gen/stdfunc.h>
#include <interfaces/types/base_settings.h>
#include <interfaces/types/common_types.h>

enum class InterfaceError : quint16
{
    OpenError,
    ReadError,
    WriteError
};
Q_DECLARE_METATYPE(InterfaceError)

class BaseInterface : public QObject
{
    Q_OBJECT
public:
    explicit BaseInterface(const QString &logFilename, const BaseSettings &settings, QObject *parent = nullptr);

signals:
    void dataReceived(const QByteArray &ba);
    void started();
    void reconnected();
    void finished();
    void error(const InterfaceError error);
    void stateChanged(Interface::State);
    void clearQueries();

protected:
    std::atomic<Interface::State> m_state;
    quint16 m_reconnectInterval;
    bool m_isLoggingEnabled;
    LogClass m_log;
    QMutex m_dataGuard;

    void setState(const Interface::State state) noexcept;
    Interface::State getState() const noexcept;

    void writeLog(const QByteArray &ba, Interface::Direction dir = Interface::NoDirection);
    void writeLog(const Error::Msg msg, Interface::Direction dir = Interface::NoDirection);

    virtual QByteArray read(bool &status) = 0;
    virtual bool write(const QByteArray &ba) = 0;

public slots:
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    void poll();
    void writeData(const QByteArray &ba);
    void close();
    void reconnect();
};
