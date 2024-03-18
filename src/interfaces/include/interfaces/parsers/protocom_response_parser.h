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

    /// \brief Anonymous structure for representing type of the connected board.
    struct
    {
        quint8 mTypeB = 0;
        quint8 mTypeM = 0;

        bool isEmpty() noexcept
        {
            return ((mTypeB == 0) && (mTypeM == 0));
        }
    } boardType;

#ifdef Q_OS_LINUX
    void processUnixTime(const QByteArray &data);
#endif
    void processU32(const QByteArray &data, quint16 startAddr);
    void processFloat(const QByteArray &data, quint32 startAddr);
    void processInt(const byte num);
    void processSinglePoint(const QByteArray &data, const quint16 startAddr);
    void processBlock(const QByteArray &data, quint32 blockNum);
    void processTechBlock(const QByteArray &data, quint32 blockNum);

    void processDataBlock(const QByteArray &data, const quint16 addr);
    void processDataSection(const QByteArray &fileSection);

public:
    explicit ProtocomResponseParser(QObject *parent = nullptr);

    bool isCompleteResponse() override;
    Error::Msg validate() override;
    void parse() override;

public slots:
    void receiveJournalData(const S2::FilesEnum fileNum, const QByteArray &file);
};

} // namespace Interface
