#include "interfaces/parsers/base_response_parser.h"

namespace Interface
{

BaseResponseParser::BaseResponseParser(QObject *parent) : QObject(parent)
{
}

void BaseResponseParser::setRequest(const CommandStruct &command) noexcept
{
    m_request = command;
}

void BaseResponseParser::totalBytes(const int total)
{
    processProgressRange(total);
}

void BaseResponseParser::progressBytes(const int progress)
{
    processProgressCount(progress);
}

void BaseResponseParser::processProgressCount(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataCount, count };
    emit responseParsed(resp);
}

void BaseResponseParser::processProgressRange(const quint64 count)
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::DataSize, count };
    emit responseParsed(resp);
}

void BaseResponseParser::processOk()
{
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, 0 };
    emit responseParsed(resp);
}

void BaseResponseParser::processError(int errorCode)
{
    qCritical() << "Device error code: " << QString::number(errorCode, 16);
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Error, static_cast<quint64>(errorCode) };
    emit responseParsed(resp);
}

} // namespace Interface
