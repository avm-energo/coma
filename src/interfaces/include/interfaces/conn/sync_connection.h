#pragma once

#include <QTimer>
#include <gen/error.h>
#include <interfaces/types/common_types.h>

namespace Interface
{

class AsyncConnection;

class SyncConnection : public QObject
{
    Q_OBJECT
private:
    AsyncConnection *m_connection;
    QTimer *m_timeoutTimer;
    QByteArray m_byteArrayResult;
    bool m_busy, m_timeout, m_responseResult;

public:
    explicit SyncConnection(AsyncConnection *connection) noexcept;

    bool supportBSIExt();
    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeFileSync(S2::FilesEnum filenum, const QByteArray &ba);
    Error::Msg writeConfigurationSync(const QByteArray &ba);
    Error::Msg readS2FileSync(S2::FilesEnum filenum);
    Error::Msg readFileSync(S2::FilesEnum filenum, QByteArray &ba);
    Error::Msg reqTimeSync(void *block, quint32 blocksize);

private slots:
    void resultReady(const DataTypes::BlockStruct &result);
    void responseReceived(const DataTypes::GeneralResponseStruct &response);
    void fileReceived(const S2::FileStruct &file);
    void timeout();
};

} // namespace Interface
