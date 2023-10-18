#pragma once

#include <interfaces/types/common_types.h>

namespace Interface
{

class BaseResponseParser : public QObject
{
    Q_OBJECT
public:
    explicit BaseResponseParser(QObject *parent = nullptr);

    virtual bool isValid(const QByteArray &response) = 0;
    virtual void parse(const QByteArray &response) = 0;
};

} // namespace Interface
