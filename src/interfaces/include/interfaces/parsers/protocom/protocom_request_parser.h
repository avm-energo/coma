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
    QByteArray m_continueCommand;

    bool isSupportedCommand(const Commands command) const noexcept;

    /// \brief Возврат группы протокола по запрошенному адресу.
    Protocol::ProtocomGroup getGroupByAddress(const quint32 addr) const noexcept;

    QByteArray prepareBlock(const Proto::Commands cmd, const QByteArray &data, //
        Proto::Starters startByte = Proto::Starters::Request);
    void processFileFromDisk(const S2::FilesEnum fileNum);
    void prepareLongData(const Proto::Commands cmd, const QByteArray &data);
    QByteArray writeLongData(const Proto::Commands cmd, const QByteArray &data);
    QByteArray createContinueCommand(const Proto::Commands cmd) const noexcept;

public:
    explicit ProtocomRequestParser(QObject *parent = nullptr);
    void basicProtocolSetup() noexcept override;

    QByteArray parse(const CommandStruct &cmd) override;
    QByteArray getNextContinueCommand() noexcept override;
    void exceptionalAction(const CommandStruct &cmd) noexcept override;

signals:
    void sendJournalData(const S2::FilesEnum fileNum, const QByteArray &file);
    /// \brief Сигнал для эмуляции ответов на команды, отсутствующие в Protocom.
    void emulateOkAnswer();
};

} // namespace Interface
