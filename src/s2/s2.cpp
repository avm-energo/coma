#include "s2.h"

#include "../module/configstorage.h"
#include "crc32.h"

#include <QDateTime>
#include <QDebug>
#include <gen/error.h>

QMap<QString, quint16> S2::NameIdMap;

S2::S2()
{
}

void S2::StoreDataMem(QByteArray &mem, const QVector<S2DataTypes::DataRec> &dr, int fname)
{
    S2Dev::CRC32 crc;
    S2DataTypes::FileHeader header;
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

void S2::StoreDataMem(QByteArray &mem, const QList<DataTypes::DataRecV> &dr, int fname)
{
    QVector<S2DataTypes::DataRec> recVec;
    std::transform(dr.cbegin(), dr.cend(), std::back_inserter(recVec), //
        [](const DataTypes::DataRecV &record) { return record.serialize(); });
    StoreDataMem(mem, recVec, fname);
}

void S2::StoreDataMem(QByteArray &mem, const std::vector<DataTypes::FileStruct> &dr, int fname)
{
    QVector<S2DataTypes::DataRec> recVec;
    std::transform(dr.cbegin(), dr.cend(), std::back_inserter(recVec), //
        [](const DataTypes::FileStruct &record) { return record.serialize(); });
    StoreDataMem(mem, recVec, fname);
}

bool S2::RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2DataTypes::FileHeader));
    qInfo() << "S2 File size:" << bain.size();
    S2DataTypes::DataRecHeader recordHeader;

    // копируем FileHeader
    S2DataTypes::FileHeader fileHeader;
    memcpy(&fileHeader, &bain.data()[0], sizeof(S2DataTypes::FileHeader));
    bain.remove(0, sizeof(S2DataTypes::FileHeader));

    // проверка контрольной суммы
    Q_ASSERT(bain.size() == fileHeader.size);
    S2Dev::CRC32 crc32(bain);
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

bool S2::RestoreData(QByteArray bain, QList<DataTypes::DataRecV> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2DataTypes::FileHeader));
    qInfo() << "S2 File size:" << bain.size();
    S2DataTypes::DataRec DR;

    // копируем FileHeader
    S2DataTypes::FileHeader fh;
    memcpy(&fh, &bain.data()[0], sizeof(S2DataTypes::FileHeader));
    bain.remove(0, sizeof(S2DataTypes::FileHeader));

    // проверка контрольной суммы
    Q_ASSERT(bain.size() == fh.size);
    S2Dev::CRC32 crc32(bain);
    if (crc32 != fh.crc32)
    {
        qCritical() << Error::Msg::CrcError << "S2"; // выход за границу принятых байт
        return false;
    }
    DR.header.id = 0;
    while ((DR.header.id != S2DataTypes::dummyElement) && (!bain.isEmpty()))
    {
        auto size = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        if (size > bain.size())
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
                DataTypes::DataRecV DRV(DR, bain.left(size));
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

S2DataTypes::S2ConfigType S2::ParseHexToS2(QByteArray &ba)
{
    using namespace S2DataTypes;
    FileStruct *PV_file = new FileStruct;
    S2ConfigType S2DR;
    uint usize = 0;
    int i = 0, j = 0, k = 0, h = 0, p = 0, size = 0, copysize = 0, iVal = 0;
    QString str, st;
    QStringList sl;
    bool ok = false;
    QString binnumber, process;
    char byte = '\0';
    QByteArray *BaForSend = new QByteArray;
    str = ba;
    sl.append(str.split("\r\n:"));
    BaForSend->resize(MAXSIZE);
    BaForSend->clear();
    int MainSize = 0;
    for (i = 0; i < sl.size(); i++)
    {
        str = sl.at(i);

        if (i == 1)
        {
            st = QString("%1%2").arg(str.at(0)).arg(str.at(1));
            copysize = size = st.toInt(&ok, 16);

            if (size)
            {
                st = QString("%1%2").arg(str.at(6)).arg(str.at(7));
                if (st == "00")
                {
                    MainSize += size;
                    for (j = 0; j < size; j = j + 2)
                    {
                        binnumber.clear();
                        process.clear();
                        iVal = QString("%1%2").arg(str.at(8 + j)).arg(str.at(9 + j)).toInt(&ok, 16); // size
                        binnumber = st.setNum(iVal, 2);
                        byte = '\0';
                        if ((binnumber.size() - 1) < 7)
                        {
                            for (k = 0; k < (8 - binnumber.size()); k++)
                                process.append("0");
                            process.append(binnumber);
                            binnumber = process;
                        }

                        for (h = 7, p = 0; h >= 0; h--, p++)
                        {
                            if (binnumber.at(h).toLatin1() & 1)
                                byte |= 0b00000001 << p;
                        }
                        BaForSend->append(byte);
                    }
                    st = QString("%1%2").arg(str.at(14)).arg(str.at(15));
                    for (j = 0; j < copysize; j = j + 2)
                    {
                        binnumber.clear();
                        process.clear();
                        iVal = QString("%1%2").arg(str.at(16 + j)).arg(str.at(17 + j)).toInt(&ok, 16);
                        binnumber = st.setNum(iVal, 2);
                        byte = '\0';
                        if ((binnumber.size() - 1) < 7)
                        {
                            for (k = 0; k < (8 - binnumber.size()); k++)
                                process.append("0");
                            process.append(binnumber);
                            binnumber = process;
                        }

                        for (h = 7, p = 0; h >= 0; h--, p++)
                        {
                            if (binnumber.at(h).toLatin1() & 1)
                                byte |= 0b00000001 << p;
                        }
                        BaForSend->append(byte);
                    }
                }
            }
        }
        else
        {
            st = QString("%1%2").arg(str.at(0)).arg(str.at(1));
            size = st.toInt(&ok, 16);

            if (size)
            {
                st = QString("%1%2").arg(str.at(6)).arg(str.at(7));
                if (st == "00")
                {
                    MainSize += size;
                    for (j = 0; j < 2 * size; j = j + 2)
                    {
                        binnumber.clear();
                        process.clear();
                        st = QString("%1%2").arg(str.at(8 + j)).arg(str.at(9 + j)); //, 4, QChar('0')
                        iVal = QString("%1%2").arg(str.at(8 + j)).arg(str.at(9 + j)).toInt(&ok, 16);
                        binnumber = st.setNum(iVal, 2); //.arg(iVal, 8, QChar('0'));  //st.setNum(iVal, 2).
                        byte = '\0';
                        if ((binnumber.size() - 1) < 7)
                        {
                            for (k = 0; k < (8 - binnumber.size()); k++)
                                process.append("0");
                            process.append(binnumber);
                            binnumber = process;
                        }

                        for (h = 7, p = 0; h >= 0; h--, p++)
                        {
                            if (binnumber.at(h).toLatin1() & 1)
                                byte |= 0b00000001 << p;
                        }
                        BaForSend->append(byte);
                    }
                }
            }
        }
    }

    BaForSend->resize(MainSize);

    PV_file->file.data.clear();
    PV_file->file.data.resize(MainSize - 8);

    PV_file->type.typeHeader.id = 8000;
    PV_file->type.typeHeader.numByte = 8;
    memcpy(&PV_file->type.typeTheData, &BaForSend->data()[0], 4);
    memcpy(&PV_file->type.versionSoftware, &BaForSend->data()[4], 4);

    st.clear();
    st.append("Ver");
    for (i = 3; i >= 0; i--)
        st.append("." + QString::number(PV_file->type.versionSoftware[i]));

    PV_file->file.fileDataHeader.id = 8001;
    usize = (MainSize - 8);
    PV_file->file.fileDataHeader.numByte = usize;
    memcpy(&PV_file->file.data.data()[0], &BaForSend->data()[8], usize);

    PV_file->void_recHeader.id = 0xFFFFFFFF;
    PV_file->void_recHeader.numByte = 0;

    S2DR.append({ PV_file->type.typeHeader.id, PV_file->type.typeHeader.numByte, &PV_file->type.typeTheData });
    S2DR.append(
        { PV_file->file.fileDataHeader.id, PV_file->file.fileDataHeader.numByte, &PV_file->file.data.data()[0] });
    S2DR.append({ PV_file->void_recHeader.id, PV_file->void_recHeader.numByte, nullptr });
    if (!ok)
        return S2ConfigType();
    return S2DR;
}

quint16 S2::GetIdByName(QString name)
{
    return NameIdMap.value(name, 0);
}

void S2::tester(S2DataTypes::S2ConfigType &buffer)
{
    // here is test functions
    QList<DataTypes::DataRecV> bufferV;
    std::transform(buffer.begin(), buffer.end(), std::back_inserter(bufferV),
        [](const auto &oldRec) -> DataTypes::DataRecV { return DataTypes::DataRecV(oldRec); });
    Q_ASSERT(std::equal(buffer.cbegin(), buffer.cend(), bufferV.cbegin(), bufferV.cend(),
                 [](const S2DataTypes::DataRec &oldRec, const DataTypes::DataRecV &newRec) {
                     return S2DataTypes::is_same(oldRec, newRec.serialize());
                 })
        && "Broken DataRecV S2 conf");
    for (auto i = 0; i != bufferV.size() && i != buffer.size(); ++i)
    {
        const auto oldRec = buffer.at(i);
        const auto newRec = bufferV.at(i).serialize();
        if (!S2DataTypes::is_same(oldRec, newRec))
            qDebug() << oldRec.header.id << oldRec.header.numByte;
    }
    // test funcs end
}

quint64 S2::GetFileSize(const QByteArray &bain)
{
    // копируем FileHeader
    S2DataTypes::FileHeader fh;
    memcpy(&fh, &bain.data()[0], sizeof(S2DataTypes::FileHeader));
    return fh.size + sizeof(S2DataTypes::FileHeader);
}

quint32 S2::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}
