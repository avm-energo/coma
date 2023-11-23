#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

class Iec104ResponseParser final : public BaseResponseParser
{
    Q_OBJECT
private:
public:
    explicit Iec104ResponseParser(QObject *parent = nullptr);

    bool isCompleteResponse() override;
    Error::Msg validate() override;
    void parse() override;
};

} // namespace Interface
