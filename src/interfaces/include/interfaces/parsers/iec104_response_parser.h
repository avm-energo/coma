#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/iec104_types.h>

namespace Interface
{

class Iec104ResponseParser final : public BaseResponseParser
{
    Q_OBJECT
private:
    SharedControlBlock m_ctrlBlock;
    Iec104::APCI m_currentAPCI;
    bool m_isAPCIParseError;
    Iec104::ASDU m_currentASDU;

    void apciParseErrorHandle(const Iec104::ApciError err) noexcept;
    void parseInfoFormat() noexcept;
    void parseSupervisoryFormat() noexcept;
    void parseUnnumberedFormat() noexcept;

public:
    explicit Iec104ResponseParser(QObject *parent = nullptr);

    void updateControlBlock(const SharedControlBlock &newControlBlock) noexcept;

    bool isCompleteResponse() override;
    Error::Msg validate() override;
    void parse() override;

signals:
    void unnumberedFormatReceived(const Iec104::ControlFunc func, const Iec104::ControlArg arg);
};

} // namespace Interface
