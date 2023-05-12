#pragma once

#include "crc32.h"
#include "datarecv.h"
#include "s2datatypes.h"

#include <QByteArray>
#include <gen/datatypes.h>
#include <vector>

namespace S2Dev
{

using S2Record = std::variant<S2DataTypes::DataRec, DataTypes::DataRecV, DataTypes::FileStruct>;
using S2RecordArray = std::vector<S2Record>;

class S2Parser : public QObject
{
    Q_OBJECT
private:
    QByteArray binaryData;
    CRC32 crc;
    bool status;

    QByteArray storeRecord(const S2DataTypes::DataRec &record);
    void storeFile(const S2RecordArray &records);
    quint32 getTime32();
    QByteArray createHeader(const DataTypes::FilesEnum fname);

    void parseHeader()
    {
        ;
    }

public:
    explicit S2Parser(QObject *parent = nullptr);

    void store(const S2RecordArray &records, const DataTypes::FilesEnum fname)
    {
        storeFile(records);
        if (status)
        {
            crc.reset();
            crc.update(binaryData);
            binaryData.prepend(createHeader(fname));
            emit binaryFileReady(binaryData);
        }
    }

    void parse(const QByteArray &file)
    {
        binaryData = file;
    }

signals:
    void binaryFileReady(const QByteArray &file);
    void s2record1(const DataTypes::S2Record &record);
    void s2record2(const DataTypes::DataRecV &record);
};

}
