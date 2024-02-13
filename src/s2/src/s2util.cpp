#include "s2/s2util.h"

#include <QDateTime>
#include <QDebug>
#include <gen/stdfunc.h>
#include <gen/utils/crc32.h>
#include <s2/s2configuration.h>

namespace S2
{

QByteArray Util::convert(const quint32 id, const DataItem &item) const
{
    const auto bytes = item.toByteArray();
    S2::DataRecHeader header { id, static_cast<quint32>(bytes.size()) };
    QByteArray retValue = StdFunc::toByteArray(header);
    if (!bytes.isEmpty())
        retValue.append(bytes);
    return retValue;
}

QByteArray Util::convert(const Configuration &config, quint32 fileType) const
{
    utils::CRC32 crc;
    S2::S2FileHeader header;
    QByteArray retValue, temp;
    header.size = 0;
    // Append data from configuration
    for (const auto &[id, value] : config)
    {
        temp = convert(id, value);
        header.size += temp.size();
        crc.update(temp);
        retValue.append(temp);
    }
    // Append file tail
    S2::DataRecHeader tail { S2::dummyElement, 0 };
    temp = StdFunc::toByteArray(tail);
    header.size += temp.size();
    crc.update(temp);
    retValue.append(temp);
    // Prepend file header
    header.crc32 = crc;
    header.thetime = getTime32();
    header.service = 0xFFFF;
    header.fname = static_cast<quint16>(fileType);
    temp = StdFunc::toByteArray(header);
    retValue.prepend(temp);
    return retValue;
}

Error::Msg Util::convert(const QByteArray &rawFile, const DataFactory &factory, //
    std::map<quint32, DataItem> &result) const
{
    using namespace S2;
    if (rawFile.size() < sizeof(S2FileHeader))
        return Error::Msg::HeaderSizeError;
    auto headerBytes = rawFile.left(sizeof(S2FileHeader));
    auto header = *reinterpret_cast<const S2FileHeader *>(headerBytes.constData());
    auto workLoad = rawFile.mid(sizeof(S2FileHeader));
    if (workLoad.size() != header.size)
        return Error::Msg::SizeError;
    if (header.fname != std_ext::to_underlying(FilesEnum::Config))
        return Error::Msg::WrongFileError;
    utils::CRC32 crc(workLoad);
    if (crc != header.crc32)
        return Error::Msg::CrcError;
    DataRecHeader recordHeader { 0, 0 };
    while (recordHeader.id != S2::dummyElement && (!workLoad.isEmpty()))
    {
        auto size = sizeof(recordHeader);
        if (size > workLoad.size())
            return Error::Msg::SizeError;
        auto recordHeaderBytes = workLoad.left(size);
        recordHeader = *reinterpret_cast<const DataRecHeader *>(recordHeaderBytes.constData());
        workLoad.remove(0, size);
        if (recordHeader.id != S2::dummyElement && recordHeader.numByte > 0)
        {
            size = recordHeader.numByte;
            auto dataItem = factory.create(recordHeader.id, workLoad.left(size));
            result.insert({ recordHeader.id, dataItem });
            workLoad.remove(0, size);
        }
    }
    return Error::Msg::NoError;
}

void Util::StoreDataMem(QByteArray &mem, const std::vector<DataRec> &dr, int fname)
{
    utils::CRC32 crc;
    S2::S2FileHeader header;
    QByteArray ba;
    header.size = 0;
    for (const S2::DataRec &record : dr)
    {
        auto tmpi = sizeof(record.header);
        header.size += tmpi;
        ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&record.header), tmpi);
        crc.update(ba);
        mem.append(ba);
        if (record.header.id == S2::dummyElement)
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

void Util::StoreDataMem(QByteArray &mem, const std::vector<FileStruct> &dr, int fname)
{
    std::vector<S2::DataRec> recVec;
    std::transform(dr.cbegin(), dr.cend(), std::back_inserter(recVec), //
        [](const S2::FileStruct &record) { return record.serialize(); });
    StoreDataMem(mem, recVec, fname);
}

bool Util::RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2::S2FileHeader));
    qInfo() << "S2 File size:" << bain.size();
    S2::DataRecHeader recordHeader;

    // копируем FileHeader
    S2::S2FileHeader fileHeader;
    memcpy(&fileHeader, &bain.data()[0], sizeof(S2::S2FileHeader));
    bain.remove(0, sizeof(S2::S2FileHeader));

    // проверка контрольной суммы
    Q_ASSERT(bain.size() == fileHeader.size);
    utils::CRC32 crc32(bain);
    if (crc32 != fileHeader.crc32)
    {
        qCritical() << "S2" << Error::Msg::CrcError; // выход за границу принятых байт
        return false;
    }
    recordHeader.id = 0;
    while ((recordHeader.id != S2::dummyElement) && (!bain.isEmpty()))
    {
        auto size = sizeof(S2::DataRecHeader);
        if (size > bain.size())
        {
            qCritical() << "S2" << Error::Msg::SizeError; // выход за границу принятых байт
            return false;
        }
        memcpy(&recordHeader, &bain.data()[0], size);
        bain.remove(0, size);

        if (recordHeader.id != S2::dummyElement)
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

// quint16 Util::GetIdByName(const QString &name)
//{
//    static const auto &nameMap = S2::ConfigStorage::GetInstance().getIdByNameMap();
//    auto search = nameMap.find(name);
//    if (search == nameMap.cend())
//        return 0;
//    else
//        return search->second;
//}

std::size_t Util::getFileSize(const QByteArray &s2file) const
{
    constexpr auto s2fileHeaderSize = sizeof(S2::S2FileHeader);
    if (s2file.size() >= s2fileHeaderSize)
    {
        const auto headerBytes = s2file.left(s2fileHeaderSize);
        auto fileHeader = *reinterpret_cast<const S2::S2FileHeader *>(headerBytes.data());
        return fileHeader.size + s2fileHeaderSize;
    }
    return 0;
}

S2BFile Util::emulateS2B(const QByteArray &data, quint16 fname, quint16 typeB, quint16 typeM) const
{
    using namespace S2;
    constexpr quint32 tailEnd = 0xEEEE1111;
    utils::CRC32 crc;
    S2BFileHeader header;
    crc.update(data);
    header.fname = fname;
    header.types2b = 0;
    header.typeB = typeB;
    header.typeM = typeM;
    header.thetime = getTime32();
    S2BFileTail tail;
    tail.crc32 = crc;
    tail.end = tailEnd;
    header.size = data.size() + sizeof(tail);
    return S2BFile { header, data, tail };
}

Error::Msg Util::parseS2B(const QByteArray &file, S2BFile &result) const
{
    using namespace S2;
    constexpr auto minSize = sizeof(S2BFileHeader) + sizeof(S2BFileTail);
    if (file.size() < minSize)
        return Error::Msg::SizeError;

    auto headerBytes = file.left(sizeof(S2BFileHeader));
    const auto header = *reinterpret_cast<const S2BFileHeader *>(headerBytes.constData());
    if (header.types2b != 0)
        return Error::Msg::WrongFormatError;
    if (header.size < sizeof(DataRecHeader))
        return Error::Msg::SizeError;

    const auto calcSize = sizeof(S2BFileHeader) + header.size;
    if (file.size() != calcSize)
        return Error::Msg::SizeError;

    auto fileBytes = file.mid(sizeof(S2BFileHeader), header.size - sizeof(S2BFileTail));
    if (fileBytes.size() != (header.size - sizeof(S2BFileTail)))
        return Error::Msg::SizeError;

    constexpr quint32 tailEnd = 0xEEEE1111;
    auto tailBytes = file.right(sizeof(S2BFileTail));
    const auto tail = *reinterpret_cast<const S2BFileTail *>(tailBytes.constData());
    utils::CRC32 crc(fileBytes);
    if (crc != tail.crc32)
        return Error::Msg::CrcError;
    if (tail.end != tailEnd)
        return Error::Msg::WrongFormatError;

    result.header = header;
    result.data = fileBytes;
    result.tail = tail;
    return Error::Msg::NoError;
}

quint32 Util::getTime32() noexcept
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}

} // namespace S2
