#include "hexparser.h"

#include <algorithm>
#include <vector>

namespace S2Dev
{

HexParser::HexParser(QObject *parent) : QObject(parent)
{
}

QByteArray HexParser::stringToByteArray(const QString &string)
{
    auto bytes = QByteArray::fromHex(string.toUtf8());
    return bytes;
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
    if (strRecord[0] != startCode)
    {
        emit error(HexParseError::InvalidHexRecord);
        return false;
    }

    // Verifying the size of the given HEX record.
    auto byteRecord = stringToByteArray(strRecord);
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
    auto records = fileString.split("\r\n");
    std::vector<HexRecord> hexFile;
    hexFile.reserve(records.size());
    for (const auto &strRecord : records)
    {
        if (strRecord.length() > 0)
        {
            HexRecord record;
            if (parseASCII(strRecord, record))
                hexFile.push_back(record);
            else
                break;
        }
    }
}

}
