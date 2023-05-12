#include "s2parser.h"

#include <QDateTime>
#include <QDebug>
#include <gen/error.h>

namespace S2Dev
{

S2Parser::S2Parser(QObject *parent) : QObject { parent }
{
}

QByteArray S2Parser::storeRecord(const S2DataTypes::DataRec &record)
{
    if (record.header.id == S2DataTypes::dummyElement || record.thedata == nullptr)
    {
        status = false;
        qCritical() << "Invalid record data" << Error::NullDataError;
        return QByteArray();
    }
    else
    {
        QByteArray binaryRecord;
        auto tmpSize = sizeof(record.header);
        quint32 size = tmpSize;
        auto ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&record.header), tmpSize);
        binaryRecord.append(ba);
        tmpSize = record.header.numByte;
        size += tmpSize;
        ba = QByteArray::fromRawData(reinterpret_cast<const char *>(record.thedata), tmpSize);
        binaryRecord.append(ba);
        assert(size == binaryRecord.size());
        return binaryRecord;
    }
}

void S2Parser::storeFile(const S2RecordArray &records)
{
    binaryData.clear();
    status = true;
    for (auto &record : records)
    {
        auto rawRecord = std::visit(overloaded //
            {
                [this](const S2DataTypes::DataRec &record) { return storeRecord(record); },             //
                [this](const DataTypes::DataRecV &record) { return storeRecord(record.serialize()); },  //
                [this](const DataTypes::FileStruct &record) { return storeRecord(record.serialize()); } //
            },
            record);
        if (!status)
            break;
        binaryData.append(rawRecord);
    }
}

quint32 S2Parser::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}

QByteArray S2Parser::createHeader(const DataTypes::FilesEnum fname)
{
    S2DataTypes::FileHeader header;
    header.size = binaryData.size();
    header.crc32 = crc;
    header.thetime = getTime32();
    header.service = 0xFFFF;
    header.fname = static_cast<quint16>(fname);
    return QByteArray::fromRawData(reinterpret_cast<char *>(&header), sizeof(header));
}

}
