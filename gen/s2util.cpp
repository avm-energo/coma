#include "s2util.h"

#include "crc32.h"
#include <gen/stdfunc.h>
// #include <s2/s2configuration.h>

#include <QDateTime>
#include <QDebug>

namespace S2
{
void Util::StoreDataMem(QByteArray &mem, const QList<DataRec> &dr, int fname)
{
    utils::CRC32 crc;
    S2FileHeader header;
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
            ba = QByteArray::fromRawData(static_cast<char *>(record.thedata), tmpi);
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

bool Util::RestoreData(QByteArray bain, QList<S2::DataRec> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2::S2FileHeader));
    qDebug() << "S2 File size:" << bain.size();
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
        qDebug() << "S2 Size error"; // выход за границу принятых байт
        return false;
    }
    recordHeader.id = 0;
    while ((recordHeader.id != S2::dummyElement) && (!bain.isEmpty()))
    {
        auto size = sizeof(S2::DataRecHeader);
        if (size > bain.size())
        {
            qDebug() << "S2 Size error"; // выход за границу принятых байт
            return false;
        }
        memcpy(&recordHeader, &bain.data()[0], size);
        bain.remove(0, size);

        if (recordHeader.id != S2::dummyElement)
        {
            S2::DataRec cfp;
            cfp.header.id = recordHeader.id;
            size = cfp.header.numByte = recordHeader.numByte;
            if (size > bain.size())
            {
                qDebug() << "S2 Size error"; // выход за границу принятых байт
                return false;
            }
            cfp.thedata = new char(size);
            size_t tmpt = static_cast<size_t>(size);
            memcpy(cfp.thedata, &bain.data()[0], tmpt);
            bain.remove(0, size);
            outlist.append(cfp);
        }
    }
    return true;
}

quint32 Util::getTime32() noexcept
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}

} // namespace S2
