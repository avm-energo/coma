#include "s2requestservice.h"

#include "board.h"

S2RequestService::S2RequestService(QObject *parent) : QObject(parent), m_conn(nullptr), m_lastRequestedFile(0xffff)
{
}

void S2RequestService::updateConnection(Interface::AsyncConnection *connection)
{
    m_conn = connection;
    if (m_conn != nullptr)
        m_conn->connection(this, &S2RequestService::responseReceived);
}

void S2RequestService::responseReceived(const QByteArray &file)
{
    auto filenum = *reinterpret_cast<quint16 *>(file.left(sizeof(quint16)).data());
    if (filenum == m_lastRequestedFile)
        emit response(file);
}

void S2RequestService::request(const S2::FilesEnum filenum, bool withCheck)
{
    if (Board::GetInstance().noConfig() && withCheck)
    {
        emit noConfigurationError();
        return;
    }

    m_lastRequestedFile = std_ext::to_underlying(filenum);
    if (m_conn != nullptr)
        m_conn->reqFile(m_lastRequestedFile, DataTypes::FileFormat::DefaultS2);
}
