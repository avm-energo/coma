#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "baseport.h"
#include "settingstypes.h"

#include <QPointer>
#include <QSerialPort>
#include <QTimer>

class SerialPort final : public BasePort
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    bool init(SerialPortSettings settings);

private:
    QPointer<QSerialPort> port;
    // QTimer *m_connectionTimer;

    bool connect() override;
    void disconnect() override;
    QByteArray read(bool *status = nullptr) override;
    virtual bool write(const QByteArray &ba) override;

public slots:
    bool clear();

private slots:
    void errorOccurred(QSerialPort::SerialPortError err);

signals:
    void clearQueries();
};

#endif // SERIALPORT_H
