#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/protocom_types.h>

namespace Interface
{

class ProtocomRequestParser : public BaseRequestParser
{
    Q_OBJECT
private:
    static const std::map<Interface::Commands, Proto::Commands> s_protoCmdMap;

    quint16 blockByReg(const quint32 regAddr);
    void appendInt16(QByteArray &ba, quint16 data);
    QByteArray prepareBlock(const Proto::Commands cmd, const QByteArray &data, //
        Proto::Starters startByte = Proto::Starters::Request);
    bool isSupportedCommand(Commands command) noexcept;

public:
    explicit ProtocomRequestParser(QObject *parent = nullptr);

    QByteArray parse(const CommandStruct &cmd) override;
};

} // namespace Interface
