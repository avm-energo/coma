#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

class Iec104RequestParser final : public BaseRequestParser
{
    Q_OBJECT
private:
public:
    explicit Iec104RequestParser(QObject *parent = nullptr);

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;
};

} // namespace Interface
