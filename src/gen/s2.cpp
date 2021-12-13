#include "s2.h"

#include "error.h"

#include <QDateTime>
#include <QDebug>

S2::S2()
{
}

void S2::StoreDataMem(QByteArray &mem, const QVector<S2DataTypes::DataRec> &dr, int fname)
{
    quint32 crc = 0xFFFFFFFF;
    S2DataTypes::FileHeader header;
    QByteArray ba;
    quint32 i = 0;
    header.size = 0;
    for (const S2DataTypes::DataRec &record : dr)
    {
        const char *Rptr = reinterpret_cast<const char *>(&record);
        quint32 tmpi = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        ba = QByteArray::fromRawData(reinterpret_cast<const char *>(&record), tmpi);
        mem.append(ba);
        header.size += tmpi;
        for (i = 0; i < tmpi; i++)
            updCRC32((Rptr)[i], &crc);
        if (record.header.id == S2DataTypes::dummyElement)
            break;
        if (record.thedata)
        {
            tmpi = record.header.numByte;
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

void S2::StoreDataMem(QByteArray &mem, const QList<DataTypes::DataRecV> &dr, int fname)
{
    QVector<S2DataTypes::DataRec> recVec;
    std::transform(
        dr.cbegin(), dr.cend(), std::back_inserter(recVec), [](const auto &record) { return record.serialize(); });
    StoreDataMem(mem, recVec, fname);
}

void S2::StoreDataMem(QByteArray &mem, std::vector<DataTypes::FileStruct> &dr, int fname)
{
    QVector<S2DataTypes::DataRec> recVec;
    for (auto &record : dr)
    {
        recVec.push_back(record.serialize());
    }
    StoreDataMem(mem, recVec, fname);
}

bool S2::RestoreDataMem(void *mem, quint32 memsize, const QVector<S2DataTypes::DataRec> &dr)
{
    unsigned char *m = static_cast<unsigned char *>(mem);
    S2DataTypes::DataRec R;

    S2DataTypes::FileHeader header;
    quint32 tmpi = 0, pos = 0;
    bool noIDs = true; // признак того, что не встретился ни один из ID в dr

    // копируем FileHeader
    quint32 fhsize = sizeof(S2DataTypes::FileHeader);
    if (fhsize > memsize)
    {
        // выход за границу принятых байт
        qCritical() << "S2" << Error::Msg::SizeError;
        return false;
    }
    memcpy(&header, m, fhsize);
    m += fhsize;

    // проверка контрольной суммы
    if (!CheckCRC32(m, memsize - fhsize, header.crc32))
    {
        qCritical() << "S2: CRC error" << Error::CrcError;
        return false;
    }
    pos = fhsize;
    R.header.id = 0;
    while ((R.header.id != S2DataTypes::dummyElement) && (pos < memsize))
    {
        tmpi = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        pos += tmpi;
        if (pos > memsize)
        {
            // выход за границу принятых байт
            qCritical() << "S2" << Error::Msg::SizeError;
            return false;
        }
        memcpy(&R, m, tmpi);
        m += tmpi;
        if (R.header.id != S2DataTypes::dummyElement)
        {
            const S2DataTypes::DataRec *r = FindElem(&dr, R.header.id);
            if (r == nullptr) // элемент не найден в описании, пропускаем
            {
                tmpi = R.header.numByte;
                pos += tmpi;
                if (pos > memsize)
                {
                    // выход за границу принятых байт
                    qCritical() << "S2" << Error::Msg::SizeError;
                    return false;
                }
                m += tmpi;
                continue;
            }
            noIDs = false;
            if (r->header.numByte != R.header.numByte) //несовпадения описания прочитанного элемента с ожидаемым
            {
                qCritical() << "S2: block description mismatch" << Error::Msg::DescError; // несовпадение описаний
                                                                                          // одного и того же блока
                return false;
            }
            tmpi = r->header.numByte;
            pos += tmpi;
            if (pos > memsize)
            {
                // выход за границу принятых байт
                qCritical() << "S2" << Error::Msg::SizeError;
                return false;
            }
            memcpy(r->thedata, m, tmpi);
            m += tmpi;
        }
    }
    if (header.size != (pos - fhsize))
    {
        qCritical() << "S2: length error" << Error::Msg::HeaderSizeError; // ошибка длины
        return false;
    }
    if (noIDs)
    {
        qCritical() << "S2: there's no such ID" << Error::Msg::NoIdError; // не найдено ни одного ИД
        return false;
    }
    return true;
}

bool S2::RestoreData(QByteArray bain, QList<DataTypes::S2Record> &outlist)
{
    Q_ASSERT(bain.size() >= sizeof(S2DataTypes::FileHeader));
    qInfo() << "S2 File size:" << bain.size();
    S2DataTypes::DataRec DR;

    // копируем FileHeader
    S2DataTypes::FileHeader fh;
    memcpy(&fh, &bain.data()[0], sizeof(S2DataTypes::FileHeader));
    bain.remove(0, sizeof(S2DataTypes::FileHeader));

    // проверка контрольной суммы
    if (!CheckCRC32(&bain.data()[0], bain.size(), fh.crc32))
    {
        qCritical() << "S2" << Error::Msg::CrcError; // выход за границу принятых байт
        return false;
    }
    DR.header.id = 0;
    while ((DR.header.id != S2DataTypes::dummyElement) && (!bain.isEmpty()))
    {
        auto size = sizeof(S2DataTypes::DataRec) - sizeof(void *);
        if (size > bain.size())
        {
            qCritical() << "S2" << Error::Msg::SizeError; // выход за границу принятых байт
            return false;
        }
        memcpy(&DR, &bain.data()[0], size);
        bain.remove(0, size);

        if (DR.header.id != S2DataTypes::dummyElement)
        {
            DataTypes::S2Record cfp;
            cfp.ID = DR.header.id;
            size = DR.header.numByte;
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
    if (!CheckCRC32(&bain.data()[0], bain.size(), fh.crc32))
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

            auto search = DataTypes::DataRecV::map.map().find(DR.header.id);
            Q_ASSERT(search != DataTypes::DataRecV::map.map().end());
            if (search != DataTypes::DataRecV::map.map().end())
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

const S2DataTypes::DataRec *S2::FindElem(const QVector<S2DataTypes::DataRec> *dr, quint32 id)
{
    for (auto it = dr->cbegin(); it != dr->cend(); ++it)
    {
        if (it->header.id == id)
            return it;
        if (it->header.id == S2DataTypes::dummyElement)
            return nullptr;
    }
    return nullptr;
}

void S2::findElemAndWriteIt(QVector<S2DataTypes::DataRec> *s2config, const DataTypes::S2Record &cfp)
{
    std::for_each(s2config->begin(), s2config->end(), [&](S2DataTypes::DataRec &record) {
        findElemAndWriteIt(&record, cfp); //
    });
}

bool S2::findElemAndWriteIt(S2DataTypes::DataRec *record, const DataTypes::S2Record &cfp)
{
    if (record->header.id != cfp.ID)
    {
        return false;
    }

    if (record->header.numByte != static_cast<quint32>(cfp.data.size()))
    {
        qCritical() << "S2: Wrong element size in ConfParameter" << Error::Msg::HeaderSizeError;
        qDebug() << "Wait for element" << record->header.id //
                 << "with size:" << record->header.numByte  //
                 << "but get size:" << cfp.data.size();     //
        return false;
    }
    memcpy(record->thedata, cfp.data, cfp.data.size());
    return true;
}

S2DataTypes::S2ConfigType S2::ParseHexToS2(QByteArray &ba)
{
    using namespace S2DataTypes;
    FileStruct *PV_file = new FileStruct;

    // quint32 crc=0xFFFFFFFF;
    // quint32 tmpi = 0;
    // void *Rptr = static_cast<void *>(&PV_file.Type);
    // tmpi = sizeof(PV_file.Type)+sizeof(PV_file.File.FileDatHeader);
    S2ConfigType S2DR;
    uint usize = 0;
    int i = 0, j = 0, k = 0, h = 0, p = 0, size = 0, copysize = 0, iVal = 0;
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
    // PV_file->File.Data = &BaForSend->data()[8];
    memcpy(&PV_file->file.data.data()[0], &BaForSend->data()[8], usize);

    PV_file->void_recHeader.id = 0xFFFFFFFF;
    PV_file->void_recHeader.numByte = 0;

    S2DR.append({ PV_file->type.typeHeader.id, PV_file->type.typeHeader.numByte, &PV_file->type.typeTheData });
    S2DR.append({ PV_file->file.fileDataHeader.id, PV_file->file.fileDataHeader.numByte,
        &PV_file->file.data.data()[0] }); // BaForSend->data_ptr()
    S2DR.append({ PV_file->void_recHeader.id, PV_file->void_recHeader.numByte, nullptr });

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
    quint32 crc = 0xFFFFFFFF;
    auto *mem = static_cast<char *>(m);

    for (quint32 i = 0; i < length; ++i)
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

quint32 S2::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toSecsSinceEpoch();
}
