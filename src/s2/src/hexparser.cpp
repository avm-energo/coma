#include "s2/hexparser.h"

#include <QDebug>
#include <algorithm>
#include <s2/s2datatypes.h>

namespace S2
{

HexParser::HexParser(QObject *parent) : QObject(parent)
{
}

bool HexParser::verifyChecksum(const quint8 *data, int size)
{
    auto sum = static_cast<quint8>(std::accumulate(data, data + size - 1, 0u) & 0xff);
    quint8 verify = ~sum + 1;
    auto checksum = data[size - 1];
    return (verify == checksum);
}

bool HexParser::parseASCII(const QString &strRecord, HexRecord &record)
{
    // Verifying the start code of the given HEX record.
    if (strRecord[0] != QChar(startCode))
    {
        emit error(HexParseError::InvalidHexRecord);
        return false;
    }

    // Verifying the size of the given HEX record.
    auto byteRecord = QByteArray::fromHex(strRecord.toUtf8()); // str to byte array
    auto recordSize = byteRecord.size();
    if (recordSize < minRecordSize || recordSize > maxRecordSize)
    {
        emit error(HexParseError::InvalidRecordSize);
        return false;
    }

    // Verifying the checksum of the given HEX record.
    auto dataPtr = reinterpret_cast<const quint8 *>(byteRecord.data());
    if (!verifyChecksum(dataPtr, recordSize))
    {
        emit error(HexParseError::IncorrectChecksum);
        return false;
    }

    // Store the given binary HEX record to the HEX structure.
    auto length = dataPtr[0];
    quint16 address = dataPtr[1] << 8 | dataPtr[2];
    auto type = static_cast<RecordType>(dataPtr[3]);
    auto checksum = dataPtr[recordSize - 1];
    byteRecord = byteRecord.mid(4, length);
    record = HexRecord { length, address, type, checksum, byteRecord };
    return true;
}

void HexParser::parseFile(const QByteArray &binaryFile)
{
    QString fileString = binaryFile;
    auto strRecords = fileString.split("\r\n");
    m_records.clear();
    m_records.reserve(strRecords.size());
    for (const auto &strRecord : strRecords)
    {
        if (strRecord.length() > 0)
        {
            HexRecord record;
            if (parseASCII(strRecord, record))
                m_records.push_back(record);
            else
                break;
        }
    }
}

quint16 HexParser::getIdByAddress(const QByteArray &binAddr)
{
    Q_ASSERT(binAddr.size() == 2);
    auto dataPtr = reinterpret_cast<const quint8 *>(binAddr.data());
    quint16 address = dataPtr[0] << 8 | dataPtr[1];
    // Looks like a kostyl, mah fren
    if (address == 0x0800)
        return 8000;
    else if (address >= 0x0801)
        return 8001;
    else
        return idNotFound;
}

std::vector<S2::FileStruct> HexParser::getS2Format()
{
    std::vector<S2::FileStruct> retVal;
    QByteArray data;
    quint16 prevBlockId = idNotFound, currBlockId = idNotFound;

    for (auto &record : m_records)
    {
        if (record.recordType == RecordType::LinearAddr)
        {
            currBlockId = getIdByAddress(record.data);
            if (currBlockId == idNotFound)
            {
                emit error(HexParseError::InvalidBlockAddress);
                data.clear();
                break;
            }
            else if (currBlockId == prevBlockId)
                continue;

            if (!data.isEmpty())
            {
                retVal.push_back(S2::FileStruct { prevBlockId, data });
                data.clear();
            }
            prevBlockId = currBlockId;
        }
        else if (record.recordType == RecordType::BinaryData)
        {
            data.append(record.data);
        }
        else if (record.recordType == RecordType::EndOfFile)
        {
            retVal.push_back(S2::FileStruct { prevBlockId, data });
            data.clear();
            break;
        }
        else
        {
            // TODO: Handle other record types?
        }
    }

    retVal.push_back(S2::FileStruct { S2::dummyElement, QByteArray() });
    return retVal;
}

}
