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
    std::vector<QByteArray> m_responses;
    Iec104::APCI m_currentAPCI;
    Iec104::ASDUUnpacker m_unpacker;

    /// \brief Используется для разбивания буффера на
    /// массив байт каждого ответа от устройства.
    void splitBuffer() noexcept;
    void apciParseErrorHandle(const Iec104::ApciError err) noexcept;
    void parseInfoFormat(const QByteArray &response) noexcept;
    void parseSupervisoryFormat() noexcept;
    void parseUnnumberedFormat() noexcept;

public:
    explicit Iec104ResponseParser(QObject *parent = nullptr);

    void updateControlBlock(const SharedControlBlock &newControlBlock) noexcept;

    bool isCompleteResponse() override;
    Error::Msg validate() override;
    Error::Msg validate(const QByteArray &response) noexcept;
    void parse() override;

signals:
    void unnumberedFormatReceived(const Iec104::ControlFunc func, const Iec104::ControlArg arg);
    void needToCheckControlBlock();
};

} // namespace Interface
