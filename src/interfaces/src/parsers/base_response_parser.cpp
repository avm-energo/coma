#include "interfaces/parsers/base_response_parser.h"

#include <s2/s2util.h>

namespace Interface
{

BaseResponseParser::BaseResponseParser(QObject *parent)
    : QObject(parent), m_isFirstSectionReceived(true), m_isLastSectionReceived(false), m_isLastSectionSended(false)
{
}

void BaseResponseParser::setRequest(const CommandStruct &request) noexcept
{
    m_request = request;
}

bool BaseResponseParser::isLastSectionReceived() const noexcept
{
    return m_isLastSectionReceived;
}

void BaseResponseParser::accumulateToResponseBuffer(const QByteArray &responsePart) noexcept
{
    m_responseBuffer.append(responsePart);
}

void BaseResponseParser::clearResponseBuffer() noexcept
{
    m_responseBuffer.clear();
}

void BaseResponseParser::lastSectionSended() noexcept
{
    m_isLastSectionSended = true;
}

void BaseResponseParser::processProgressCount(const quint64 count) noexcept
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, count };
    emit responseParsed(resp);
}

void BaseResponseParser::processProgressRange(const quint64 count) noexcept
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, count };
    emit responseParsed(resp);
}

void BaseResponseParser::processOk() noexcept
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
    emit responseParsed(resp);
}

void BaseResponseParser::processError(int errorCode) noexcept
{
    qCritical() << "Device error code: " << QString::number(errorCode, 16);
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, static_cast<quint64>(errorCode) };
    emit responseParsed(resp);
}

void BaseResponseParser::fileReceived(const QByteArray &file, //
    const S2::FilesEnum addr, const DataTypes::FileFormat format)
{
    switch (format)
    {
    case DataTypes::FileFormat::Binary:
    {
        DataTypes::GeneralResponseStruct genResp {
            DataTypes::GeneralResponseTypes::Ok, //
            static_cast<quint64>(file.size())    //
        };
        emit responseParsed(genResp);
        switch (addr)
        {
        // По модбасу уже получили файл в формате S2B
        case S2::FilesEnum::JourSys:
        case S2::FilesEnum::JourWork:
        case S2::FilesEnum::JourMeas:
        {
            if (!file.isEmpty())
            {
                S2::S2BFile s2bFile {};
                auto errCode = m_util.parseS2B(file, s2bFile);
                if (errCode == Error::Msg::NoError)
                    emit responseParsed(s2bFile);
                else
                    qCritical() << QVariant::fromValue(errCode).toString();
            }
            break;
        }
        default:
        {
            S2::FileStruct resp { addr, file };
            emit responseParsed(resp);
            break;
        }
        }
        break;
    }
    case DataTypes::FileFormat::DefaultS2:
    {
        emit responseParsed(file);
        break;
    }
    case DataTypes::FileFormat::CustomS2:
    {
        DataTypes::S2FilePack outlist;
        if (!S2Util::RestoreData(file, outlist))
        {
            DataTypes::GeneralResponseStruct resp {
                DataTypes::GeneralResponseTypes::Error, //
                static_cast<quint64>(file.size())       //
            };
            emit responseParsed(resp);
            return;
        }
        DataTypes::GeneralResponseStruct genResp {
            DataTypes::GeneralResponseTypes::Ok, //
            static_cast<quint64>(file.size())    //
        };
        emit responseParsed(genResp);
        for (auto &&file : outlist)
        {
            S2::FileStruct resp { S2::FilesEnum(file.ID), file.data };
            emit responseParsed(resp);
        }
        break;
    }
    }
}

} // namespace Interface
