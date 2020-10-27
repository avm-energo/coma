#pragma once

#include <QObject>

class UsbParser : public QObject
{
    Q_OBJECT
public:
    explicit UsbParser(QObject *parent = nullptr);

    void send(char command, char parameter, QByteArray &ba, qint64 datasize);

signals:
};
