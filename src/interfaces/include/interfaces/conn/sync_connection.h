#pragma once

#include <gen/error.h>
#include <interfaces/types/common_types.h>

#include <QTimer>

namespace Interface
{

class AsyncConnection;

class SyncConnection : public QObject
{
    Q_OBJECT
private:
    AsyncConnection *m_connection;
    QByteArray m_byteArrayResult;
    S2::S2BFile m_s2bFile;
    bool m_busy;
    Error::Msg m_responseResult;

    void eventLoop() noexcept;
    void reset() noexcept;

public:
    explicit SyncConnection(AsyncConnection *connection) noexcept;

    Error::Msg reqBSI();
    Error::Msg reqBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeBlockSync(quint32 blocknum, DataTypes::DataBlockTypes blocktype, void *block, quint32 blocksize);
    Error::Msg writeFileSync(S2::FilesEnum filenum, const QByteArray &ba);
    Error::Msg writeConfigurationSync(const QByteArray &ba);
    Error::Msg readS2FileSync(S2::FilesEnum filenum);
    Error::Msg readFileSync(S2::FilesEnum filenum, QByteArray &ba);
    Error::Msg readS2BFileSync(S2::FilesEnum filenum, S2::S2BFile &file);
    Error::Msg reqTimeSync(void *block, quint32 blocksize);

signals:
    void setRange(qint64);
    void setValue(qint64);

private slots:
    void resultReady(const DataTypes::BlockStruct &result);
    void responseReceived(const DataTypes::GeneralResponseStruct &response);
    void fileReceived(const S2::FileStruct &file);
    void s2bfileReceived(const S2::S2BFile &file);
    void timeout();
    void responseError(Error::Msg msg);
};

} // namespace Interface
