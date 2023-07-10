#include "s2.h"

#include "../module/configstorage.h"

#include <QDateTime>
#include <QDebug>
#include <gen/error.h>
#include <gen/utils/crc32.h>

QMap<QString, quint16> S2Util::NameIdMap;

void S2Util::StoreDataMem(QByteArray &mem, const std::vector<S2DataTypes::DataRec> &dr, int fname)
{
    utils::CRC32 crc;
    S2DataTypes::S2FileHeader header;
    QByteArray ba;
    header.size = 0;
    for (const S2DataTypes::DataRec &record : dr)
    {
        auto tmpi = sizeof(record.header);
        header.size += tmpi;
        ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&record.header), tmpi);
        crc.update(ba);
        mem.append(ba);
        if (record.header.id == S2DataTypes::dummyElement)
            break;
        if (record.thedata != nullptr)
        {
            tmpi = record.header.numByte;
            header.size += tmpi;
            auto data = reinterpret_cast<const char *>(record.thedata);
            ba = QByteArray::fromRawData(data, tmpi);
            crc.update(ba);
            mem.append(ba);
        }
    }
    header.crc32 = crc;
    header.thetime = getTime32();
    header.service = 0xFFFF;
    header.fname = static_cast<quint16>(fname);
    ba = QByteArray::fromRawData(reinterpret_cast<char *>(&header), sizeof(header));
    mem.prepend(ba);
}

void S2Util::StoreDataMem(QByteArray &mem, const QList<S2DataTypes::DataRecV> &dr, int fname)
{
    std::vector<S2DataTypes::DataRec> recVec;
    std::transform(dr.cbegin(), dr.cend(), std::back_inserter(recVec), //
        [](const S2DataTypes::DataRecV &record) { return record.serialize(); });
    StoreDataMem(mem, recVec, fname);
}

void S2Util::StoreDataMem(QByteArray &mem, const std::vector<S2DataTypes::FileStruct> &dr, int fname)
{
    std::vector<S2DataTypes::DataRec> recVec;
    std::transform(dr.cbegin(), dr.cend(), std::back_inserter(recVec), //
        [](const S2DataTypes::FileStruct &record) { return record.serialize(); });
    StoreDataMem(mem, recVec, fname);
}

bool S2Util::RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2DataTypes::S2FileHeader));
    qInfo() << "S2 File size:" << bain.size();
    S2DataTypes::DataRecHeader recordHeader;

    // копируем FileHeader
    S2DataTypes::S2FileHeader fileHeader;
    memcpy(&fileHeader, &bain.data()[0], sizeof(S2DataTypes::S2FileHeader));
    bain.remove(0, sizeof(S2DataTypes::S2FileHeader));

    // проверка контрольной суммы
    Q_ASSERT(bain.size() == fileHeader.size);
    utils::CRC32 crc32(bain);
    if (crc32 != fileHeader.crc32)
    {
        qCritical() << "S2" << Error::Msg::CrcError; // выход за границу принятых байт
        return false;
    }
    recordHeader.id = 0;
    while ((recordHeader.id != S2DataTypes::dummyElement) && (!bain.isEmpty()))
    {
        auto size = sizeof(S2DataTypes::DataRecHeader);
        if (size > bain.size())
        {
            qCritical() << "S2" << Error::Msg::SizeError; // выход за границу принятых байт
            return false;
        }
        memcpy(&recordHeader, &bain.data()[0], size);
        bain.remove(0, size);

        if (recordHeader.id != S2DataTypes::dummyElement)
        {
            DataTypes::S2Record cfp;
            cfp.ID = recordHeader.id;
            size = recordHeader.numByte;
            if (size > bain.size())
            {
                qCritical() << "S2" << Error::Msg::SizeError; // выход за границу принятых байт
                return false;
            }
            cfp.data = bain.left(size);
            bain.remove(0, size);
            outlist.append(cfp);
        }
    }
    return true;
}

bool S2Util::RestoreData(QByteArray bain, QList<S2DataTypes::DataRecV> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2DataTypes::S2FileHeader));
    qInfo() << "S2 File size:" << bain.size();
    S2DataTypes::DataRec DR;

    // копируем FileHeader
    S2DataTypes::S2FileHeader fh;
    memcpy(&fh, &bain.data()[0], sizeof(S2DataTypes::S2FileHeader));
    bain.remove(0, sizeof(S2DataTypes::S2FileHeader));

    // проверка контрольной суммы
    Q_ASSERT(bain.size() == fh.size);
    utils::CRC32 crc32(bain);
    if (crc32 != fh.crc32)
    {
        qCritical() << Error::Msg::CrcError << "S2"; // выход за границу принятых байт
        return false;
    }
    DR.header.id = 0;
    while ((DR.header.id != S2DataTypes::dummyElement) && (!bain.isEmpty()))
    {
        auto size = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        if (size > static_cast<quint64>(bain.size()))
        {
            qCritical() << Error::Msg::SizeError << "S2: out of memory"; // выход за границу принятых байт
            return false;
        }
        memcpy(&DR, &bain.data()[0], size);
        bain.remove(0, size);

        if (DR.header.id != S2DataTypes::dummyElement)
        {
            size = DR.header.numByte;
            auto &s2map = ConfigStorage::GetInstance().getS2Map();
            auto search = s2map.find(DR.header.id);
            if (search != s2map.end())
            {
                S2DataTypes::DataRecV DRV(DR, bain.left(size));
                outlist.append(DRV);
            }
            else
            {
                qCritical("В модуле неизвестная конфигурация, необходимо взять конфигурацию по умолчанию");
            }
            bain.remove(0, size);
        }
    }
    return true;
}

quint16 S2Util::GetIdByName(QString name)
{
    return NameIdMap.value(name, 0);
}

void S2Util::tester(const S2DataTypes::S2ConfigType &buffer)
{
    // here is test functions
    using namespace S2DataTypes;
    std::vector<DataRecV> bufferV;
    std::transform(
        buffer.begin(), buffer.end(), std::back_inserter(bufferV), [](const auto &oldRec) { return DataRecV(oldRec); });
    Q_ASSERT(std::equal(buffer.cbegin(), buffer.cend(), bufferV.cbegin(), bufferV.cend(),
                 [](const DataRec &oldRec, const DataRecV &newRec) { return oldRec == newRec.serialize(); })
        && "Broken DataRecV S2 conf");
    for (auto i = 0; i != bufferV.size() && i != buffer.size(); ++i)
    {
        const auto oldRec = buffer.at(i);
        const auto newRec = bufferV.at(i).serialize();
        if (oldRec != newRec)
            qDebug() << oldRec.header.id << oldRec.header.numByte;
    }
    // test funcs end
}

quint64 S2Util::GetFileSize(const QByteArray &bain)
{
    // копируем FileHeader
    S2DataTypes::S2FileHeader fh;
    memcpy(&fh, &bain.data()[0], sizeof(S2DataTypes::S2FileHeader));
    return fh.size + sizeof(S2DataTypes::S2FileHeader);
}

S2DataTypes::S2BFile S2Util::emulateS2B(
    const S2DataTypes::FileStruct &journal, quint16 fname, quint16 typeB, quint16 typeM)
{
    using namespace S2DataTypes;
    constexpr quint32 tailEnd = 0xEEEE1111;
    utils::CRC32 crc;
    S2BFileHeader header;
    DataRecHeader recHeader { journal.ID, quint32(journal.data.size()) };
    crc.update(reinterpret_cast<quint8 *>(&recHeader), sizeof(recHeader));
    crc.update(journal.data);
    header.fname = fname;
    header.types2b = 0;
    header.size = journal.data.size() + sizeof(recHeader);
    header.typeB = typeB;
    header.typeM = typeM;
    header.thetime = getTime32();
    S2BFileTail tail;
    tail.crc32 = crc;
    tail.end = tailEnd;
    return { header, journal, tail };
}

S2DataTypes::S2BFile S2Util::parseS2B(const QByteArray &file)
{
    using namespace S2DataTypes;
    constexpr auto minSize = sizeof(S2BFileHeader) + sizeof(S2BFileTail) + sizeof(DataRecHeader);
    Q_ASSERT(file.size() > minSize);
    auto headerBytes = file.left(sizeof(S2BFileHeader));
    const auto header = *reinterpret_cast<const S2BFileHeader *>(headerBytes.constData());
    Q_ASSERT(header.size > sizeof(DataRecHeader));
    const auto calcSize = sizeof(S2BFileHeader) + sizeof(S2BFileTail) + header.size;
    Q_ASSERT(file.size() == calcSize);
    auto recordBytes = file.mid(sizeof(S2BFileHeader), header.size);
    Q_ASSERT(recordBytes.size() == header.size);
    auto recHeaderBytes = recordBytes.left(sizeof(DataRecHeader));
    const auto recHeader = *reinterpret_cast<const DataRecHeader *>(recHeaderBytes.constData());
    Q_ASSERT(header.size - sizeof(DataRecHeader) == recHeader.numByte);
    auto fileBytes = recordBytes.mid(sizeof(DataRecHeader));
    Q_ASSERT(recHeader.numByte == fileBytes.size());
    auto tailBytes = file.right(sizeof(S2BFileTail));
    Q_ASSERT(tailBytes.size() == sizeof(S2BFileTail));
    const auto tail = *reinterpret_cast<const S2BFileTail *>(tailBytes.constData());
    return { header, { recHeader.id, fileBytes }, tail };
}

quint32 S2Util::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}
