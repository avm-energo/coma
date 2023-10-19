#pragma once

#include <interfaces/parsers/base_request_parser.h>
#include <interfaces/types/protocom_types.h>

namespace Interface
{

class ProtocomRequestParser final : public BaseRequestParser
{
    Q_OBJECT
private:
    static const std::map<Interface::Commands, Proto::Commands> s_protoCmdMap;

    quint16 getBlockByReg(const quint32 regAddr);
    QByteArray prepareBlock(const Proto::Commands cmd, const QByteArray &data, //
        Proto::Starters startByte = Proto::Starters::Request);
    bool isSupportedCommand(const Commands command) const noexcept;
    bool isOneSegment(const quint64 length) const noexcept;
    void prepareLongData(const Proto::Commands cmd, const QByteArray &data);
    QByteArray writeLongData(const Proto::Commands cmd, const QByteArray &data);

public:
    explicit ProtocomRequestParser(QObject *parent = nullptr);

    QByteArray parse(const CommandStruct &cmd) override;
};

} // namespace Interface
