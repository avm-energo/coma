#include "s2.h"

#include "error.h"

#include <QDateTime>

S2::S2()
{
}

Error::Msg S2::StoreDataMem(void *mem, QVector<S2DataTypes::DataRec> *dr,
    int fname) // 0 - успешно, иначе код ошибки
{
    quint32 crc = 0xFFFFFFFF;
    S2DataTypes::FileHeader header;
    S2DataTypes::DataRec R;
    quint32 i;
    char *m = static_cast<char *>(mem);
    m += sizeof(S2DataTypes::FileHeader);
    header.size = 0;
    for (QVector<S2DataTypes::DataRec>::iterator it = dr->begin(); it != dr->end(); ++it)
    {
        R = *it;
        void *Rptr = static_cast<void *>(it);
        quint32 tmpi = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        memcpy(m, &R, tmpi);
        header.size += tmpi;
        for (i = 0; i < tmpi; i++)
            updCRC32((static_cast<char *>(Rptr))[i], &crc);
        if (R.id == 0xFFFFFFFF)
            break;
        m += tmpi;
        if (R.thedata)
        {
            tmpi = R.num_byte;
            for (i = 0; i < tmpi; i++)
                updCRC32((static_cast<char *>(R.thedata))[i], &crc);
            header.size += tmpi;
            memcpy(m, R.thedata, tmpi);
            m += tmpi;
        }
    }
    header.crc32 = crc;
    header.thetime = getTime32();
    header.service = 0xFFFF;
    header.fname = static_cast<quint16>(fname);
    memcpy(mem, &header, sizeof(header));
    return Error::Msg::NoError;
}

Error::Msg S2::RestoreDataMem(void *mem, quint32 memsize, QVector<S2DataTypes::DataRec> *dr)
{
    unsigned char *m = static_cast<unsigned char *>(mem);
    S2DataTypes::DataRec R;
    S2DataTypes::DataRec *r;
    S2DataTypes::FileHeader header;
    quint32 tmpi, pos;
    bool noIDs = true; // признак того, что не встретился ни один из ID в dr

    // копируем FileHeader
    quint32 fhsize = sizeof(S2DataTypes::FileHeader);
    if (fhsize > memsize)
    {
        ERMSG("S2: out of memory"); // выход за границу принятых байт
        return Error::Msg::S2_SIZEERROR;
    }
    memcpy(&header, m, fhsize);
    m += fhsize;

    // проверка контрольной суммы
    if (!CheckCRC32(m, memsize - fhsize, header.crc32))
    {
        ERMSG("S2: CRC error"); // выход за границу принятых байт
        return Error::Msg::S2_CRCERROR;
    }
    pos = fhsize;
    R.id = 0;
    while ((R.id != 0xFFFFFFFF) && (pos < memsize))
    {
        tmpi = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        pos += tmpi;
        if (pos > memsize)
        {
            ERMSG("S2: out of memory"); // выход за границу принятых байт
            return Error::Msg::S2_SIZEERROR;
        }
        memcpy(&R, m, tmpi);
        m += tmpi;
        if (R.id != 0xFFFFFFFF)
        {
            r = FindElem(dr, R.id);
            if (r == nullptr) // элемент не найден в описании, пропускаем
            {
                tmpi = R.num_byte;
                pos += tmpi;
                if (pos > memsize)
                {
                    ERMSG("S2: out of memory"); // выход за границу принятых байт
                    return Error::Msg::S2_SIZEERROR;
                }
                m += tmpi;
                continue;
            }
            noIDs = false;
            if (r->num_byte != R.num_byte) //несовпадения описания прочитанного элемента с ожидаемым
            {
                ERMSG("S2: block description mismatch"); // несовпадение описаний одного
                                                         // и того же блока
                return Error::Msg::S2_DESCERROR;
            }
            tmpi = r->num_byte;
            pos += tmpi;
            if (pos > memsize)
            {
                ERMSG("S2: out of memory"); // выход за границу принятых байт
                return Error::Msg::S2_SIZEERROR;
            }
            memcpy(r->thedata, m, tmpi);
            m += tmpi;
        }
    }
    if (header.size != (pos - fhsize))
    {
        ERMSG("S2: length error"); // ошибка длины
        return Error::Msg::S2_DHSZERROR;
    }
    if (noIDs)
    {
        ERMSG("S2: there's no such ID"); // не найдено ни одного ИД
        return Error::Msg::S2_NOIDS;
    }
    return Error::Msg::NoError;
}

Error::Msg S2::RestoreData(QByteArray &bain, QList<DataTypes::ConfParameterStruct> &outlist)
{
    //    unsigned char *m = static_cast<unsigned char *>(mem);
    S2DataTypes::DataRec DR;
    //    DataRec *r;
    //    FileHeader header;
    //    quint32 tmpi, pos;
    //    bool noIDs = true; // признак того, что не встретился ни один из ID в dr

    // копируем FileHeader
    S2DataTypes::FileHeader fh;
    memcpy(&fh, &bain.data()[0], sizeof(S2DataTypes::FileHeader));
    bain.remove(0, sizeof(S2DataTypes::FileHeader));
    //    quint32 fhsize = sizeof(FileHeader);
    //    if (fhsize > memsize)
    //    {
    //        ERMSG("S2: out of memory"); // выход за границу принятых байт
    //        return Error::Msg::S2_SIZEERROR;
    //    }
    //    memcpy(&header, m, fhsize);
    //    m += fhsize;

    // проверка контрольной суммы
    if (!CheckCRC32(&bain.data()[0], bain.size(), fh.crc32))
    {
        ERMSG("S2: CRC error"); // выход за границу принятых байт
        return Error::Msg::S2_CRCERROR;
    }
    //    pos = fhsize;
    DR.id = 0;
    while ((DR.id != 0xFFFFFFFF) && (!bain.isEmpty()))
    //    while ((R.id != 0xFFFFFFFF) && (pos < memsize))
    {
        int size = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        //        pos += tmpi;
        if (size > bain.size())
        {
            ERMSG("S2: out of memory"); // выход за границу принятых байт
            return Error::Msg::S2_SIZEERROR;
        }
        memcpy(&DR, &bain.data()[0], size);
        bain.remove(0, size);
        if (DR.id != 0xFFFFFFFF)
        {
            DataTypes::ConfParameterStruct cfp;
            cfp.ID = DR.id;
            size = DR.num_byte;
            if (size > bain.size())
            {
                ERMSG("S2: out of memory"); // выход за границу принятых байт
                return Error::Msg::S2_SIZEERROR;
            }
            cfp.data = bain.left(size);
            bain.remove(0, size);
            outlist.append(cfp);
            //            r = FindElem(dr, R.id);
            //            if (r == nullptr) // элемент не найден в описании, пропускаем
            //            {
            //                tmpi = R.num_byte;
            //                pos += tmpi;
            //                if (pos > memsize)
            //                {
            //                    ERMSG("S2: out of memory"); // выход за границу принятых байт
            //                    return Error::Msg::S2_SIZEERROR;
            //                }
            //                m += tmpi;
            //                continue;
            //            }
            //            noIDs = false;
            //            if (r->num_byte != R.num_byte) //несовпадения описания прочитанного элемента с ожидаемым
            //            {
            //                ERMSG("S2: block description mismatch"); // несовпадение описаний одного
            //                                                         // и того же блока
            //                return Error::Msg::S2_DESCERROR;
            //            }
            //            tmpi = r->num_byte;
            //            pos += tmpi;
            //            if (pos > memsize)
            //            {
            //                ERMSG("S2: out of memory"); // выход за границу принятых байт
            //                return Error::Msg::S2_SIZEERROR;
            //            }
            //            memcpy(r->thedata, m, tmpi);
            //            m += tmpi;
        }
    }
    //    if (header.size != (pos - fhsize))
    //    {
    //        ERMSG("S2: length error"); // ошибка длины
    //        return Error::Msg::S2_DHSZERROR;
    //    }
    //    if (noIDs)
    //    {
    //        ERMSG("S2: there's no such ID"); // не найдено ни одного ИД
    //        return Error::Msg::S2_NOIDS;
    //    }
    return Error::Msg::NoError;
}

S2DataTypes::DataRec *S2::FindElem(QVector<S2DataTypes::DataRec> *dr, quint32 id)
{
    for (QVector<S2DataTypes::DataRec>::iterator it = dr->begin(); it != dr->end(); ++it)
    {
        S2DataTypes::DataRec R = *it;
        if (R.id == id)
            return it;
        if (R.id == static_cast<quint32>(0xFFFF))
            return nullptr;
    }
    return nullptr;
}

Error::Msg S2::findElemAndWriteIt(QVector<S2DataTypes::DataRec> *s2config, DataTypes::ConfParameterStruct &cfp)
{
    for (QVector<S2DataTypes::DataRec>::iterator it = s2config->begin(); it != s2config->end(); ++it)
    {
        S2DataTypes::DataRec R = *it;
        if (R.id == cfp.ID)
        {
            if (R.num_byte == static_cast<quint32>(cfp.data.size()))
            {
                memcpy(&R.thedata, cfp.data, cfp.data.size());
                return Error::Msg::NoError;
            }
            else
            {
                ERMSG("S2: Wrong element size in ConfParameter");
                return Error::Msg::S2_DHSZERROR;
            }
        }
    }
    return Error::Msg::ResEmpty;
}

void inline S2::updCRC32(char byte, quint32 *dwCRC32)
{
    *dwCRC32 = ((*dwCRC32) >> 8) ^ _crc32_t[static_cast<quint8>(byte) ^ ((*dwCRC32) & 0x000000FF)];
}

bool S2::CheckCRC32(void *m, quint32 length, quint32 crctocheck)
{
    quint32 i;
    quint32 crc = 0xFFFFFFFF;
    unsigned char *mem = static_cast<unsigned char *>(m);

    for (i = 0; i < length; ++i)
    {
        updCRC32(*mem, &crc);
        ++mem;
    }
    return (crctocheck == crc);
}

quint32 S2::GetCRC32(char *data, quint32 len)
{
    quint32 dwCRC32 = 0xFFFFFFFF;
    for (quint32 i = 0; i < len; i++)
    {
        updCRC32(*data, &dwCRC32);
        data++;
    }
    return dwCRC32;
}

quint32 S2::updateCRC32(unsigned char ch, quint32 crc)
{
    return (_crc32_t[((crc) ^ (static_cast<quint8>(ch))) & 0xff] ^ ((crc) >> 8));
}

quint32 S2::crc32buf(const QByteArray &data)
{
    return ~std::accumulate(data.begin(), data.end(), quint32(0xFFFFFFFF),
        [](quint32 oldcrc32, char buf) { return updateCRC32(buf, oldcrc32); });
}

quint32 S2::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}
