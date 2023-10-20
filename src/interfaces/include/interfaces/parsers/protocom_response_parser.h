#pragma once

#include <interfaces/parsers/base_response_parser.h>
#include <interfaces/types/protocom_types.h>

namespace Interface
{

class ProtocomResponseParser final : public BaseResponseParser
{
    Q_OBJECT
private:
    Proto::Commands m_receivedCommand;
    bool m_isFirstSection;

    /// \brief Anonymous structure for representing type of the connected board.
    struct
    {
        quint8 mTypeB = 0;
        quint8 mTypeM = 0;

        bool isEmpty()
        {
            return ((mTypeB == 0) || (mTypeM == 0));
        }
    } boardType;

#ifdef Q_OS_LINUX
    void processUnixTime(const QByteArray &ba);
#endif
    void processU32(const QByteArray &data, quint16 startAddr);
    void processFloat(const QByteArray &data, quint32 startAddr);
    void processInt(const byte num);
    void processSinglePoint(const QByteArray &data, const quint16 startAddr);
    void processBlock(const QByteArray &data, quint32 blockNum);
    void processTechBlock(const QByteArray &data, quint32 blockNum);

    void progressFile(const QByteArray &filePart);

public:
    explicit ProtocomResponseParser(QObject *parent = nullptr);

    bool isValid(const QByteArray &response) override;
    virtual void parse(const QByteArray &response) override;
};

} // namespace Interface
