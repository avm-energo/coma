#pragma once

#include <interfaces/types/common_types.h>

namespace Interface
{

class BaseResponseParser : public QObject
{
    Q_OBJECT
protected:
    CommandStruct m_request;

    void processProgressCount(const quint64 count);
    void processProgressRange(const quint64 count);
    void processOk();
    void processError(int errorCode = 0);

public:
    explicit BaseResponseParser(QObject *parent = nullptr);

    void setRequest(const CommandStruct &command) noexcept;

    virtual bool isValid(const QByteArray &response) = 0;
    virtual void parse(QByteArray &response) = 0;

public slots:
    void totalBytes(const int total);
    void progressBytes(const int progress);

signals:
    void responseParsed(const Interface::DeviceResponse &response);
};

} // namespace Interface
