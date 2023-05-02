#pragma once

#include "crc32.h"
#include "datarecv.h"
#include "s2datatypes.h"

#include <QByteArray>
#include <QObject>
#include <QVector>
#include <vector>

namespace S2Dev
{

using S2Record = std::variant<S2DataTypes::DataRec, DataTypes::DataRecV, DataTypes::FileStruct>;

class S2Parser : public QObject
{
    Q_OBJECT
private:
    QByteArray binaryData;
    CRC32 crc;
    bool status;

    QByteArray storeRecord(const S2DataTypes::DataRec &record)
    {
        if (record.header.id == S2DataTypes::dummyElement || record.thedata == nullptr)
        {
            status = false;
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
            crc.update(binaryRecord);
            return binaryRecord;
        }
    }

public:
    explicit S2Parser(QObject *parent = nullptr);

    void store(const QVector<S2DataTypes::DataRec> &records, int fname)
    {
        binaryData.clear();
        status = true;
        for (auto &record : records)
        {
            storeRecord(record);
        }
    }

    void store(const QVector<DataTypes::DataRecV> &objects, int fname)
    {
        binaryData.clear();
        status = true;
        for (auto &object : objects)
        {
            storeRecord(object.serialize());
        }
    }

    void store(const QVector<DataTypes::FileStruct> &objects, int fname)
    {
        binaryData.clear();
        status = true;
        for (auto &object : objects)
        {
            storeRecord(object.serialize());
        }
    }

    //    template <typename Object, typename Serial = decltype(std::declval<Object>().serialize())> //
    //    void store(const QVector<Object> &objects)
    //    {
    //        static_assert(std::is_same_v<Serial, S2DataTypes::DataRec>, "Unsupported type.");
    //        binaryData.clear();
    //        status = true;
    //        for (auto &object : objects)
    //        {
    //            auto record = object.serialize();
    //            storeRecord(record);
    //        }
    //    }
};

}
