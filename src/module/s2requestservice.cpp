#include "s2requestservice.h"

#include "board.h"

#include <interfaces/conn/baseconnection.h>

S2RequestService::S2RequestService(QObject *parent)
    : QObject(parent), m_proxy(new DataTypesProxy(this)), m_lastRequestedFile(0xffff)
{
    m_proxy->RegisterType<QByteArray>();
    connect(m_proxy.get(), &DataTypesProxy::DataStorable, this, &S2RequestService::responseReceived);
}

void S2RequestService::responseReceived(const QVariant &var)
{
    if (var.isValid() && var.canConvert<QByteArray>())
    {
        const auto bytes = var.value<QByteArray>();
        auto filenum = *reinterpret_cast<quint16 *>(bytes.left(sizeof(quint16)).data());
        if (filenum == m_lastRequestedFile)
            emit response(bytes);
    }
}

void S2RequestService::request(const S2::FilesEnum filenum, bool withCheck)
{
    if (Board::GetInstance().noConfig() && withCheck)
    {
        emit noConfigurationError();
        return;
    }

    m_lastRequestedFile = std_ext::to_underlying(filenum);
    Interface::BaseConnection::iface()->reqFile(m_lastRequestedFile, DataTypes::FileFormat::DefaultS2);
}
