#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/protocom_types.h>

namespace Interface
{

class ProtocomResponseParser final : public BaseResponseParser
{
    Q_OBJECT
public:
    explicit ProtocomResponseParser(QObject *parent = nullptr);

    bool isValid(const QByteArray &response) override;
    virtual void parse(const QByteArray &response) override;
};

} // namespace Interface
