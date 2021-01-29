#include "s2.h"

#include "error.h"

#include <QDateTime>
#include <QDebug>

S2DataTypes::S2ConfigType S2::config;
S2::S2()
{
}

void S2::StoreDataMem(void *mem, QVector<S2DataTypes::DataRec> *dr, int fname)
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
    return;
}

void S2::StoreDataMem(QByteArray &mem, QVector<S2DataTypes::DataRec> *dr, int fname)
{
    quint32 crc = 0xFFFFFFFF;
    S2DataTypes::FileHeader header;
    QByteArray ba;
    quint32 i;
    header.size = 0;
    for (S2DataTypes::DataRec &record : *dr)
    {
        char *Rptr = reinterpret_cast<char *>(&record);
        quint32 tmpi = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        ba = QByteArray::fromRawData(reinterpret_cast<char *>(&record), tmpi);
        mem.append(ba);
        header.size += tmpi;
        for (i = 0; i < tmpi; i++)
            updCRC32((Rptr)[i], &crc);
        if (record.id == 0xFFFFFFFF)
            break;
        if (record.thedata)
        {
            tmpi = record.num_byte;
            header.size += tmpi;
            char *data = static_cast<char *>(record.thedata);
            for (i = 0; i < tmpi; i++)
                updCRC32(data[i], &crc);
            ba = QByteArray::fromRawData(data, tmpi);
            mem.append(ba);
        }
    }
    header.crc32 = crc;
    header.thetime = getTime32();
    header.service = 0xFFFF;
    header.fname = static_cast<quint16>(fname);
    ba = QByteArray::fromRawData(reinterpret_cast<char *>(&header), sizeof(header));
    mem.prepend(ba);
    return;
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
        return Error::Msg::SizeError;
    }
    memcpy(&header, m, fhsize);
    m += fhsize;

    // проверка контрольной суммы
    if (!CheckCRC32(m, memsize - fhsize, header.crc32))
    {
        ERMSG("S2: CRC error"); // выход за границу принятых байт
        return Error::Msg::CrcError;
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
            return Error::Msg::SizeError;
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
                    return Error::Msg::SizeError;
                }
                m += tmpi;
                continue;
            }
            noIDs = false;
            if (r->num_byte != R.num_byte) //несовпадения описания прочитанного элемента с ожидаемым
            {
                ERMSG("S2: block description mismatch"); // несовпадение описаний одного
                                                         // и того же блока
                return Error::Msg::DescError;
            }
            tmpi = r->num_byte;
            pos += tmpi;
            if (pos > memsize)
            {
                ERMSG("S2: out of memory"); // выход за границу принятых байт
                return Error::Msg::SizeError;
            }
            memcpy(r->thedata, m, tmpi);
            m += tmpi;
        }
    }
    if (header.size != (pos - fhsize))
    {
        ERMSG("S2: length error"); // ошибка длины
        return Error::Msg::HeaderSizeError;
    }
    if (noIDs)
    {
        ERMSG("S2: there's no such ID"); // не найдено ни одного ИД
        return Error::Msg::NoIdError;
    }
    return Error::Msg::NoError;
}

Error::Msg S2::RestoreData(QByteArray &bain, QList<DataTypes::ConfParameterStruct> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2DataTypes::FileHeader));
    qInfo() << "S2 File size:" << bain.size();
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
        return Error::Msg::CrcError;
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
            return Error::Msg::SizeError;
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
                return Error::Msg::SizeError;
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

void S2::findElemAndWriteIt(QVector<S2DataTypes::DataRec> *s2config, const DataTypes::ConfParameterStruct &cfp)
{
    std::for_each(s2config->begin(), s2config->end(), [&](S2DataTypes::DataRec &record) {
        findElemAndWriteIt(&record, cfp); //
    });
}

Error::Msg S2::findElemAndWriteIt(S2DataTypes::DataRec *record, const DataTypes::ConfParameterStruct &cfp)
{
    if (record->id != cfp.ID)
        return Error::DescError;

    if (record->num_byte != static_cast<quint32>(cfp.data.size()))
    {
        qCritical("S2: Wrong element size in ConfParameter");
        qDebug() << "Wait for element" << record->id    //
                 << "with size:" << record->num_byte    //
                 << "but get size:" << cfp.data.size(); //
        return Error::Msg::HeaderSizeError;
    }
    memcpy(record->thedata, cfp.data, cfp.data.size());
    return Error::Msg::NoError;
}

S2DataTypes::S2ConfigType S2::ParseHexToS2(QByteArray &ba)
{
    using namespace S2DataTypes;
    File_struct *PV_file = new File_struct;

    // quint32 crc=0xFFFFFFFF;
    // quint32 tmpi = 0;
    // void *Rptr = static_cast<void *>(&PV_file.Type);
    // tmpi = sizeof(PV_file.Type)+sizeof(PV_file.File.FileDatHeader);
    S2ConfigType S2DR;
    uint usize;
    int i, j = 0, k, h, p, size, copysize, iVal;
    QString str, st;
    QStringList sl;
    bool ok = false;
    // QByteArray* ForProcess = new QByteArray;

    QString binnumber, process;
    char byte = '\0';
    // QVector<QByteArray> * BaForSend = new QVector<QByteArray>;
    QByteArray *BaForSend = new QByteArray;

    str = ba;

    sl.append(str.split("\r\n:"));

    // c = QString::number(sl.at(0), 2);
    // st.append(QString("%1").arg(c, 4, 2, QChar('0')));
    // BaForSend.data()[0] = st.at(0).toLatin1();

    // st.append(QString("%1").arg(c, 4, 2, QChar('0')));

    BaForSend->resize(MAXSIZE);
    BaForSend->clear();
    int MainSize = 0;
    for (i = 0; i < sl.size(); i++)
    {
        str = sl.at(i);

        if (i == 1)
        {
            st = QString("%1%2").arg(str.at(0)).arg(str.at(1));
            // st =  QString("%1").arg(st.toULongLong(&ok, 16), 0, 16, QChar('0'));
            copysize = size = st.toInt(&ok, 16);

            if (size)
            {
                // if(size == 10)
                // size=16;

                st = QString("%1%2").arg(str.at(6)).arg(str.at(7));
                if (st == "00")
                {
                    MainSize += size;
                    for (j = 0; j < size; j = j + 2)
                    { // while(size)

                        binnumber.clear();
                        process.clear();
                        iVal = QString("%1%2").arg(str.at(8 + j)).arg(str.at(9 + j)).toInt(&ok, 16); // size
                        binnumber = st.setNum(iVal, 2); //.arg(iVal, 8, QChar('0'));  //st.setNum(iVal, 2).

                        byte = '\0';

                        if ((binnumber.size() - 1) < 7)
                        {
                            // memcpy(&process.data()[7-binnumber.size()-1], &binnumber,
                            // binnumber.size());
                            for (k = 0; k < (8 - binnumber.size()); k++)
                                process.append("0");

                            process.append(binnumber);
                            binnumber = process;
                        }

                        for (h = 7, p = 0; h >= 0; h--, p++)
                        {

                            if (binnumber.at(h).toLatin1() & 1)
                                byte |= 0b00000001 << p;
                            // else
                            // byte &= 0b11111110<<p;
                        }

                        BaForSend->append(byte);
                        // size = size-2;
                    }

                    st = QString("%1%2").arg(str.at(14)).arg(str.at(15));

                    for (j = 0; j < copysize; j = j + 2)
                    {
                        binnumber.clear();
                        process.clear();
                        iVal = QString("%1%2").arg(str.at(16 + j)).arg(str.at(17 + j)).toInt(&ok, 16);
                        binnumber = st.setNum(iVal, 2); //.arg(iVal, 8, QChar('0'));  //st.setNum(iVal, 2).

                        byte = '\0';

                        if ((binnumber.size() - 1) < 7)
                        {
                            // memcpy(&process.data()[7-binnumber.size()-1], &binnumber,
                            // binnumber.size());
                            for (k = 0; k < (8 - binnumber.size()); k++)
                                process.append("0");

                            process.append(binnumber);
                            binnumber = process;
                        }

                        for (h = 7, p = 0; h >= 0; h--, p++)
                        {

                            if (binnumber.at(h).toLatin1() & 1)
                                byte |= 0b00000001 << p;
                            // else
                            // byte &= 0b11111110<<p;
                        }

                        BaForSend->append(byte);
                        // copysize = copysize-2;
                    }
                }
            }

            // ba.data()[0] = QString("%1").arg(str.toULongLong(&ok, 16), 16, 2,
            // QChar('0'));
            // i++;
            // u64 = str.toULongLong(&ok, 16);
            // memcpy(&ba.data()[0], &str, 16);
            // QString("%1").arg(c, 8, 2, QChar('0')
            // str = str.arg(str.toULongLong(&ok, 2), 16, 2, QChar('0'));
        }
        else
        {
            // st = str.at(1).toUpper() + str.at(2).toLower();
            st = QString("%1%2").arg(str.at(0)).arg(str.at(1));
            size = st.toInt(&ok, 16);

            if (size)
            {
                // if(size == 10)
                // size=16;

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
                            // memcpy(&process.data()[7-binnumber.size()-1], &binnumber,
                            // binnumber.size());
                            for (k = 0; k < (8 - binnumber.size()); k++)
                                process.append("0");

                            process.append(binnumber);
                            binnumber = process;
                        }

                        for (h = 7, p = 0; h >= 0; h--, p++)
                        {

                            if (binnumber.at(h).toLatin1() & 1)
                                byte |= 0b00000001 << p;
                            // else
                            // byte &= 0b11111110<<p;
                        }

                        BaForSend->append(byte);
                    }
                }
            }
        }
    }

    BaForSend->resize(MainSize);

    PV_file->File.Data.clear();
    PV_file->File.Data.resize(MainSize - 8);

    PV_file->Type.TypeHeader.id = 8000;
    PV_file->Type.TypeHeader.NumByte = 8;
    memcpy(&PV_file->Type.TypeTheData, &BaForSend->data()[0], 4);
    memcpy(&PV_file->Type.VerPO, &BaForSend->data()[4], 4);

    st.clear();
    st.append("Ver");
    for (i = 3; i >= 0; i--)
        st.append("." + QString::number(PV_file->Type.VerPO[i]));

    PV_file->File.FileDatHeader.id = 8001;
    usize = (MainSize - 8);
    PV_file->File.FileDatHeader.NumByte = usize;
    // PV_file->File.Data = &BaForSend->data()[8];
    memcpy(&PV_file->File.Data.data()[0], &BaForSend->data()[8], usize);

    PV_file->void_recHeader.id = 0xFFFFFFFF;
    PV_file->void_recHeader.NumByte = 0;

    S2DR.append({ PV_file->Type.TypeHeader.id, PV_file->Type.TypeHeader.NumByte, &PV_file->Type.TypeTheData });
    S2DR.append({ PV_file->File.FileDatHeader.id, PV_file->File.FileDatHeader.NumByte,
        &PV_file->File.Data.data()[0] }); // BaForSend->data_ptr()
    S2DR.append({ PV_file->void_recHeader.id, PV_file->void_recHeader.NumByte, nullptr });

    /*ForProcess->clear();
    ForProcess->resize(MAXSIZE);

    memcpy(&ForProcess->data()[0], &PV_file.Type.TypeHeader.id, 24);
    memcpy(&ForProcess->data()[24], &BaForSend->data()[8], (BaForSend->size()-8));
    ForProcess->resize((BaForSend->size()+16));
    //BaForSend->resize(ForProcess->size());
    memcpy(&BaForSend->data()[0], &ForProcess->data()[0],
        (BaForSend->size()+16));*/
    if (!ok)
        return S2ConfigType();
    return S2DR;
}

void inline S2::updCRC32(const char byte, quint32 *dwCRC32)
{
    *dwCRC32 = ((*dwCRC32) >> 8) ^ _crc32_t[static_cast<const quint8>(byte) ^ ((*dwCRC32) & 0x000000FF)];
}

bool S2::CheckCRC32(void *m, const quint32 length, const quint32 crctocheck)
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
