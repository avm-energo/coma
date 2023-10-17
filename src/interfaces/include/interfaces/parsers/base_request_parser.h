#pragma once

#include <deque>
#include <interfaces/types/common_types.h>

namespace Interface
{

class BaseRequestParser : public QObject
{
    Q_OBJECT
protected:
    QByteArray m_command;
    std::deque<QByteArray> m_longWriteData;

public:
    explicit BaseRequestParser(QObject *parent = nullptr);

    virtual QByteArray parse(const CommandStruct &command) = 0;
};

} // namespace Interface
