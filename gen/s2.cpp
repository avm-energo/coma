#include <QDateTime>

#include "error.h"
#include "s2.h"

S2::S2()
{
}

int S2::StoreDataMem(void *mem, QVector<DataRec> *dr, int fname) // 0 - успешно, иначе код ошибки
{
    quint32 crc = 0xFFFFFFFF;
    FileHeader header;
    DataRec R;
    quint32 i;
    char *m = static_cast<char *>(mem);
    m += sizeof(FileHeader);
    header.size = 0;
    for (QVector<DataRec>::iterator it = dr->begin(); it != dr->end(); ++it)
    {
        R = *it;
        void *Rptr = static_cast<void *>(it);
        quint32 tmpi = sizeof(DataRec) - sizeof(void *);
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
    return NOERROR;
}

int S2::RestoreDataMem(void *mem, quint32 memsize, QVector<DataRec> *dr)
{
    unsigned char *m = static_cast<unsigned char *>(mem);
    DataRec R;
    DataRec *r;
    FileHeader header;
    quint32 tmpi, pos;
    bool noIDs = true; // признак того, что не встретился ни один из ID в dr

    // копируем FileHeader
    quint32 fhsize = sizeof(FileHeader);
    if (fhsize > memsize)
    {
        ERMSG("S2: out of memory"); // выход за границу принятых байт
        return S2_SIZEERROR;
    }
    memcpy(&header, m, fhsize);
    m += fhsize;

    // проверка контрольной суммы
    if (!CheckCRC32(m, memsize - fhsize, header.crc32))
    {
        ERMSG("S2: CRC error"); // выход за границу принятых байт
        return S2_CRCERROR;
    }
    pos = fhsize;
    R.id = 0;
    while ((R.id != 0xFFFFFFFF) && (pos < memsize))
    {
        tmpi = sizeof(DataRec) - sizeof(void *);
        pos += tmpi;
        if (pos > memsize)
        {
            ERMSG("S2: out of memory"); // выход за границу принятых байт
            return S2_SIZEERROR;
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
                    return S2_SIZEERROR;
                }
                m += tmpi;
                continue;
            }
            noIDs = false;
            if (r->num_byte != R.num_byte) //несовпадения описания прочитанного элемента с ожидаемым
            {
                ERMSG("S2: block description mismatch"); // несовпадение описаний одного и того же блока
                return S2_DESCERROR;
            }
            tmpi = r->num_byte;
            pos += tmpi;
            if (pos > memsize)
            {
                ERMSG("S2: out of memory"); // выход за границу принятых байт
                return S2_SIZEERROR;
            }
            memcpy(r->thedata, m, tmpi);
            m += tmpi;
        }
    }
    if (header.size != (pos - fhsize))
    {
        ERMSG("S2: length error"); // ошибка длины
        return S2_DHSZERROR;
    }
    if (noIDs)
    {
        ERMSG("S2: there's no such ID"); // не найдено ни одного ИД
        return S2_NOIDS;
    }
    return NOERROR;
}

S2::DataRec *S2::FindElem(QVector<DataRec> *dr, quint32 id)
{
    for (QVector<DataRec>::iterator it = dr->begin(); it != dr->end(); ++it)
    {
        DataRec R = *it;
        if (R.id == id)
            return it;
        if (R.id == static_cast<quint32>(0xFFFF))
            return nullptr;
    }
    return nullptr;
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

quint32 S2::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toTime_t();
}
