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
    Iec104::ASDU m_currentASDU;

    void apciErrorHandle(const Iec104::ApciError err) const noexcept;

public:
    explicit Iec104ResponseParser(QObject *parent = nullptr);

    void updateControlBlock(const SharedControlBlock &newControlBlock) noexcept;

    bool isCompleteResponse() override;
    Error::Msg validate() override;
    void parse() override;
};

} // namespace Interface
