#include "iec104thread.h"

#include "../gen/datamanager.h"
#include "../gen/pch.h"
#include "../gen/s2.h"
#include "iec104private.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QTimer>
QMutex IEC104Thread::s_ParseReadMutex;
QMutex IEC104Thread::s_ParseWriteMutex;

using namespace Commands104;

IEC104Thread::IEC104Thread(LogClass *log, QObject *parent) : QObject(parent), m_log(log)
{
    m_writingToPortBlocked = true;
    m_isFirstParse = true;
    m_threadMustBeFinished = false;
    m_signalCounter = 0;
    m_V_S = m_V_R = 0;
    m_ackVR = I104_W;
    m_APDUFormat = I104_WRONG;
    m_isFileSending = false;
    m_noAnswer = 0;
}

IEC104Thread::~IEC104Thread()
{
}

void IEC104Thread::SetBaseAdr(quint16 adr)
{
    m_baseAdrHigh = adr >> 8;
    m_baseAdrLow = adr;
}

void IEC104Thread::Run()
{
    m_timer104 = new QTimer(this);
    m_timer104->setInterval(15000);
    m_sendTestTimer = new QTimer(this);
    m_sendTestTimer->setInterval(5000);
#ifndef DEBUG
    connect(m_timer104, &QTimer::timeout, this, &IEC104Thread::Stop);
    connect(m_sendTestTimer, &QTimer::timeout, this, &IEC104Thread::SendTestAct);
    m_sendTestTimer->start();
#endif
    while (!m_threadMustBeFinished)
    {
        if (!m_parseData.isEmpty())
        {
            s_ParseReadMutex.lock();
            QByteArray tmpba = m_parseData.takeFirst();
            s_ParseReadMutex.unlock();
            if (!tmpba.isEmpty())
            {
                Error::Msg tmpi = isIncomeDataValid(tmpba);
                if (tmpi == Error::Msg::NoError) // если поймали правильное начало данных,
                                                 // переходим к их обработке
                {
                    if (m_APDUFormat == I104_I)
                    {
                        Q_ASSERT(tmpba.size() > 6);
                        tmpba = tmpba.mid(6);
                        ParseIFormat(tmpba); // без APCI
                    }
                }
            }
        }
        if (!m_isFileSending && !m_writingToPortBlocked)
        {
            Commands104::CommandStruct inp;
            if (DataManager::deQueue(inp) == Error::Msg::NoError)
            {
                switch (inp.cmd)
                {
                case Commands104::CM104_COM45:
                    Com45(inp.uintarg);
                    break;
                case Commands104::CM104_COM50:
                    Com50(inp.uintarg, inp.flarg);
                    break;
                case Commands104::CM104_WRITEFILE:
                {
                    //                    S2ConfigType *ptr = reinterpret_cast<S2ConfigType
                    //                    *>(inp.args.ptrarg); FileReady(ptr);
                    m_fileIsConfigFile = Queries::FileFormat::Binary;
                    m_file = inp.ba;
                    FileReady(inp.uintarg);
                    break;
                }
                case Commands104::CM104_REQCONFIGFILE:
                    m_fileIsConfigFile = Queries::FileFormat::DefaultS2;
                    SelectFile(inp.uintarg);
                    break;
                case Commands104::CM104_REQFILE:
                    m_fileIsConfigFile = Queries::FileFormat::Binary;
                    SelectFile(inp.uintarg);
                    break;
                case Commands104::CM104_COM51:
                    Com51WriteTime(inp.uintarg);
                    break;
                    //                case Commands104::CM104_CORREADREQUEST:
                    //                    CorReadRequest();
                    //                    break;
                case Commands104::CM104_REQGROUP:
                    reqGroup(inp.uintarg);
                    break;
                default:
                    break;
                }
            }
            if (m_V_R > (m_ackVR + I104_W))
                SendS();
        }
        QCoreApplication::processEvents();
    }
    emit finished();
}

void IEC104Thread::GetSomeData(QByteArray ba)
{

    if (m_isFirstParse)
        m_log->info("<-- " + ba.toHex());
    m_isFirstParse = false;
    quint32 basize = static_cast<quint32>(ba.size());
    if (m_cutPckt.size() > 1)
    {
        quint32 cutpcktlen = static_cast<quint8>(m_cutPckt.at(1));
        cutpcktlen += 2;
        quint32 cutpcktsize = static_cast<quint32>(m_cutPckt.size());
        quint32 missing_num = cutpcktlen - cutpcktsize; // взяли длину остатка от предыдущего пакета
        if (missing_num > basize)
        {
            m_cutPckt.append(ba);
            m_isFirstParse = true;
            return; // если так и не достигли конца пакета, надо брать следующий пакет
                    // в cutpckt
        }
        Q_ASSERT(ba.size() >= missing_num);
        m_cutPckt.append(ba.left(missing_num)); // взяли из текущего пакета сами байты
        ba.remove(0, missing_num);
        s_ParseReadMutex.lock();
        m_parseData.append(m_cutPckt);
        s_ParseReadMutex.unlock();
        m_cutPckt.clear();
        basize = static_cast<quint32>(ba.size());
    }
    if (basize < 2) // ba is empty or there's not enough symbols to parse in it
    {
        m_isFirstParse = true;
        return;
    }

    m_cutPckt = ba.left(2);
    ba = ba.mid(2);
    GetSomeData(ba);
    m_isFirstParse = true;
}

Error::Msg IEC104Thread::isIncomeDataValid(QByteArray ba)
{
    if (ba.size() < 3)
        return Error::SizeError;
    try
    {
        if (ba.at(0) != 0x68)
            // return I104_RCVWRONG;
            return Error::Msg::GeneralError;
        m_APDULength = static_cast<quint8>(ba.at(1)); // в 1-м байте лежит длина
        if ((m_APDULength < 4) || (m_APDULength > 253))
            // return I104_RCVWRONG;
            return Error::Msg::GeneralError;
        if (ba.size() < 3)
            return Error::SizeError;
        if (!(ba.at(2) & 0x01)) // I
            m_APDUFormat = I104_I;
        else
        {
            if (!(ba.at(2) & 0x02)) // S
                m_APDUFormat = I104_S;
            else
                m_APDUFormat = I104_U;
        }
        switch (m_APDUFormat)
        {
        case I104_I:
        {
            quint16 V_Rrcv = static_cast<quint8>(ba.at(3)) * 256 + static_cast<quint8>(ba.at(2) & 0xFE);
            V_Rrcv >>= 1;
            if (V_Rrcv != m_V_R)
            {
                m_log->error("V_Rrcv != V_R");
                // return I104_RCVWRONG;
                return Error::Msg::GeneralError;
            }
            m_V_R++;
            if (ba.size() < 6)
                return Error::SizeError;
            quint16 V_Srcv = static_cast<quint8>(ba.at(5)) * 256 + static_cast<quint8>(ba.at(4) & 0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != m_V_S)
                m_V_S = V_Srcv; // временно, нужно исправить проблему несовпадения s посылок
            // return I104_RCVNORM;
            return Error::Msg::NoError;
        }
        case I104_S:
        {
            if (ba.size() < 6)
                return Error::SizeError;
            quint16 V_Srcv = static_cast<quint8>(ba.at(5)) * 256 + static_cast<quint8>(ba.at(4) & 0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != m_V_S)
                m_V_S = V_Srcv;
            // return I104_RCVNORM;
            return Error::Msg::NoError;
        }
        case I104_U:
        {
            unsigned char baat2 = ba.at(2);
            if ((baat2 == I104_STARTDT_CON)
                && (m_command
                    == I104_STARTDT_ACT)) // если пришло подтверждение старта и перед этим мы старт запрашивали
            {
                m_timer104->stop();
                m_command = I104_STARTDT_CON;
                SendGI();
                m_writingToPortBlocked = false;
            }
            if ((baat2 == I104_STOPDT_CON)
                && (m_command == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
                m_command = I104_STOPDT_CON;
            if ((baat2 == I104_TESTFR_CON)
                && (m_command == I104_TESTFR_ACT)) // если пришло подтверждение теста и перед этим мы тест запрашивали
                m_command = I104_TESTFR_CON;
            if (baat2 == I104_TESTFR_ACT)
                SendTestCon();

            m_noAnswer = 0;

            // return I104_RCVNORM;
            return Error::Msg::NoError;
        }
        default:
            break;
        }
        // return I104_RCVWRONG;
        return Error::Msg::GeneralError;
    } catch (...)
    {
        ERMSG("Fatal exception");
        return Error::Msg::GeneralError;
    }
}

bool IEC104Thread::handleFile(QByteArray &ba, DataTypes::FilesEnum addr, Queries::FileFormat format)
{
    using Queries::FileFormat;
    switch (format)
    {
    case FileFormat::DefaultS2:
    {
        QList<DataTypes::DataRecV> outlistV;
        if (!S2::RestoreData(ba, outlistV))
            return false;
        DataManager::addSignalToOutList(DataTypes::DataRecVList, outlistV);
        break;
    }
    default:
    {
        DataTypes::S2FilePack outlist;
        if (!S2::RestoreData(ba, outlist))
            return false;
        Q_ASSERT(outlist.size() == 1 && "Only one file supported");
        DataTypes::FileStruct df { addr, outlist.first().data };
        DataManager::addSignalToOutList(DataTypes::SignalTypes::File, df);
        break;
    }
    }
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, static_cast<quint64>(ba.size()) };
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
    return true;
}

void IEC104Thread::ParseIFormat(QByteArray &ba) // основной разборщик
{
    using namespace Commands104;
    DataUnitIdentifier DUI;
    try
    {
        if (ba.size() < 6)
        {
            qDebug() << Error::SizeError;
            return;
        }
        DUI.typeIdent = static_cast<TypeId>(ba.at(0));
        DUI.qualifier.Number = ba.at(1) & 0x7f;
        DUI.qualifier.SQ = ba.at(1) >> 7;
        DUI.cause.cause = ba.at(2) & 0x3F;
        DUI.cause.confirm = (ba.at(2) >> 6) & 0x01;
        DUI.cause.test = ba.at(2) >> 7;
        DUI.cause.initiator = ba.at(3);
        DUI.commonAdrASDU = ba.at(4) + ba.at(5) * 256;
        quint32 objectAdr = 0;
        quint32 index = 6;
        int fileSize;
        int i; //, cntfl = 0, cntflTimestamp = 0, cntspon = 0, cntbs = 0;
        //        IEC104Thread::FlSignals104 *flSignals = new IEC104Thread::FlSignals104[DUI.qualifier.Number];
        //        IEC104Thread::SponSignals *sponsignals = new IEC104Thread::SponSignals[DUI.qualifier.Number];
        //        IEC104Thread::BS104Signals *BS104Signals = new IEC104Thread::BS104Signals[DUI.qualifier.Number];

        for (i = 0; i < DUI.qualifier.Number; i++)
        {
            if ((i == 0) || (DUI.qualifier.SQ == 0))
            {
                objectAdr = ba.at(index++);
                objectAdr &= 0x00FF;
                objectAdr |= ba.at(index++) << 8;
                objectAdr |= ba.at(index++) << 16;
            }
            else
                objectAdr++;
            // qDebug() << DUI.typeIdent;
            switch (DUI.typeIdent)
            {

            case TypeId::M_EI_NA_1:
            {
                break;
            }
            case TypeId::M_ME_TF_1:
            {
                //                if (cntflTimestamp >= DUI.qualifier.Number)
                //                {
                //                    ERMSG("out of array flSignals");
                //                    return;
                //                }
                DataTypes::FloatWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                float value;
                memcpy(&value, &(ba.data()[index]), 4);
                index += 4;
                signal.sigVal = value;
                // quint8 quality;
                // memcpy(&quality, &(ba.data()[index]), 1);
                // index++;
                signal.sigQuality = ba.at(index++);
                // signal.sigQuality = quality;
                quint64 time;
                memcpy(&time, &(ba.data()[index]), 8);
                index += 8;
                signal.CP56Time = time;
                DataManager::addSignalToOutList(DataTypes::SignalTypes::FloatWithTime, signal);
                //                cntflTimestamp++;
                break;
            }

            case TypeId::C_IC_NA_1:
                break;

            case TypeId::M_ME_NC_1:
            {
                DataTypes::FloatWithTimeStruct signal;
                //                if (cntfl >= DUI.qualifier.Number)
                //                {
                //                    ERMSG("out of array flSignals");
                //                    return;
                //                }
                signal.sigAdr = objectAdr;
                //                float value;
                //                memcpy(&value, &(ba.data()[index]), 4);
                memcpy(&signal.sigVal, &(ba.data()[index]), 4);
                index += 4;
                //                signal.SigVal = value;
                //                quint8 quality;
                //                memcpy(&quality, &(ba.data()[index]), 1);
                signal.sigQuality = ba.at(index++);
                // memcpy(&signal.sigQuality, &(ba.data()[index]), 1);
                //                index++;
                //                signal.SigQuality = quality;
                DataManager::addSignalToOutList(DataTypes::SignalTypes::FloatWithTime, signal);
                //                cntfl++;
                break;
            }

            case TypeId::M_SP_NA_1:
            {
                //                if (cntspon > 255)
                //                {
                //                    ERMSG("out of array sponsignals");
                //                    return;
                //                }
                DataTypes::SinglePointWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                //                quint8 value;
                //                memcpy(&value, &(ba.data()[index]), 1);
                signal.sigVal = ba.at(index++);
                // memcpy(&signal.sigVal, &(ba.data()[index++]), 1);
                //                index += 1;
                //                sponsignals->Spon[cntspon].SigVal = value;
                //                cntspon++;
                DataManager::addSignalToOutList(DataTypes::SignalTypes::SinglePointWithTime, signal);
                break;
            }

            case TypeId::M_SP_TB_1:
            {
                // qDebug() << "TypeId::M_SP_TB_1";
                //                if (cntspon > 255)
                //                {
                //                    ERMSG("out of array sponsignals");
                //                    return;
                //                }
                DataTypes::SinglePointWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                //                quint8 value;
                signal.sigVal = ba.at(index++);
                // memcpy(&signal.sigVal, &(ba.data()[index++]), 1);
                //                index += 1;
                //                sponsignals->Spon[cntspon].SigVal = value;
                //                quint64 time;
                memcpy(&signal.CP56Time, &(ba.data()[index]), 7);
                index += 7;
                //                sponsignals->Spon[cntspon].CP56Time = time;
                //                cntspon++;
                DataManager::addSignalToOutList(DataTypes::SignalTypes::SinglePointWithTime, signal);
                break;
            }

            case TypeId::M_BO_NA_1:
            {
                //                if (cntbs >= DUI.qualifier.Number)
                //                {
                //                    ERMSG("out of array BS104Signals");
                //                    return;
                //                }
                DataTypes::BitStringStruct signal;
                //                int j;
                //                for (j = 0; j < 3; j++)
                //                    signal.sigAdr[j] = (objectAdr >> 8 * j);
                signal.sigAdr = objectAdr;

                //                quint32 value;
                memcpy(&signal.sigVal, &(ba.data()[index]), 4);
                index += 4;
                //                (BS104Signals + cntbs)->BS.SigVal = value;
                //                quint8 quality;
                memcpy(&signal.sigQuality, &(ba.data()[index++]), 1);
                //                index++;
                //                (BS104Signals + cntbs)->BS.SigQuality = quality;
                //                cntbs++;
                DataManager::addSignalToOutList(DataTypes::SignalTypes::BitString, signal);
                break;
            }

            case TypeId::F_SR_NA_1:
            {
                m_log->info("Section ready");
                unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                GetSection(filenum);
                break;
            }

            case TypeId::F_FR_NA_1:
            {
                m_log->info("File ready");
                unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                m_readData.clear();
                m_readSize = 0;
                m_readPos = 0;
                fileSize = (static_cast<quint8>(ba.at(13)) << 16) | (static_cast<quint8>(ba.at(12)) << 8)
                    | (static_cast<quint8>(ba.at(11)));
                setGeneralResponse(DataTypes::GeneralResponseTypes::DataSize, fileSize);
                //                emit SetDataSize(fileSize);
                CallFile(filenum);
                break;
            }

            case TypeId::F_SG_NA_1:
            {
                m_log->info(
                    "Segment ready: RDSize=" + QString::number(ba.at(12), 16) + ", num=" + QString::number(ba.at(13)));
                m_readSize = static_cast<quint8>(ba.at(12));
                m_readSize &= 0xFF;
                if (m_readSize) //>= RDLength)
                {
                    m_readData.append(&(ba.data()[13]), m_readSize);
                    m_readPos += m_readSize;
                    setGeneralResponse(DataTypes::GeneralResponseTypes::DataCount, m_readPos);
                    //                    emit SetDataCount(RDLength);
                }
                break;
            }

            case TypeId::F_LS_NA_1:
            {
                m_log->info("Last section, ba[12] = " + QString::number(ba.at(12)));
                switch (ba.at(12))
                {
                case 1:
                {
                    unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                    ConfirmFile(filenum);
                    m_sendTestTimer->start();
                    m_isFileSending = false;
                    m_log->info("FileSending clear");

                    auto filetype = static_cast<DataTypes::FilesEnum>(ba.at(9));
                    if (!handleFile(m_readData, filetype, m_fileIsConfigFile))
                        m_log->error("Error while income file S2 parsing");

                    //                    if (filetype == 0x01) // если файл конфигурации
                    //                    {
                    //                        Error::Msg res = S2::RestoreDataMem(ReadData.data(), RDLength, DR);
                    //                        if (res == Error::Msg::NoError)
                    //                            emit SendS2fromParse(DR);
                    //                    }
                    //                    else if (filetype == 0x04) // если файл системного журнала
                    //                        emit SendJourSysfromParse(ReadData);
                    //                    else if (filetype == 0x05) // если файл рабочего журнала
                    //                        emit SendJourWorkfromParse(ReadData);
                    //                    else if (filetype == 6) // если файл журнала измерений
                    //                        emit SendJourMeasfromParse(ReadData);
                    m_readPos = 0;
                    m_readSize = 0;
                    break;
                }

                case 3:
                {
                    unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                    ConfirmSection(filenum);
                    break;
                }
                }
                break;
            }

            case TypeId::F_SC_NA_1:
            {
                if (ba.at(12) == 0x02) //запрос файла
                {
                    m_log->info("File query");
                    SectionReady();
                }
                if (ba.at(12) == 0x06)
                {
                    m_log->info("Segment query");
                    SendSegments();
                }
                break;
            }

            case TypeId::F_AF_NA_1:
            {
                m_log->info("Last section of file " + QString::number(ba[12]) + " confirm");
                if (ba.at(12) == 0x03) // подтверждение секции
                    LastSection();
                if (ba.at(12) == 0x01) // подтверждение файла
                {
                    m_log->info("FileSending clear");
                    m_isFileSending = false;
                    emit SendMessagefromParse();
                }
                break;
            }

            case TypeId::C_SC_NA_1:
            {
                if (DUI.cause.cause == 10)
                {
                    //                    emit SendMessageOk();
                    DataTypes::GeneralResponseStruct grs;
                    grs.type = DataTypes::GeneralResponseTypes::Ok;
                    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, grs);
                }
                break;
            }

            case TypeId::C_SE_NC_1:
            {
                if (DUI.cause.cause == 10)
                    m_signalCounter++;

                setGeneralResponse(DataTypes::GeneralResponseTypes::DataCount, m_signalCounter);
                //                emit SetDataCount(count);
                quint32 adr = ba.at(6) + (ba.at(7) + 1) * 256; //+ (ba[8]<<16);

                if ((adr == 920) && (DUI.cause.cause == 10)) // если адрес последнего параметра коррекции
                {
                    if (ba.at(13) == 0)
                    {
                        setGeneralResponse(DataTypes::GeneralResponseTypes::Ok);
                        //                        emit SendMessageOk();
                        setGeneralResponse(
                            DataTypes::GeneralResponseTypes::DataCount, 11); // send to progressbar max count of bytes
                        //                        emit SetDataCount(11);
                    }
                    m_signalCounter = 0;
                }
                break;
            }

            default:
                qDebug() << DUI.typeIdent;
            }
        }

        //        if (cntflTimestamp != 0)
        //        {
        //            flSignals->SigNumber = cntflTimestamp;
        //            emit Floatsignalsreceived(flSignals);
        //        }

        //        if (cntfl != 0)
        //        {
        //            flSignals->SigNumber = cntfl;
        //            emit Floatsignalsreceived(flSignals);
        //        }
        //        else
        //            delete[] flSignals;

        //        if (cntspon != 0)
        //        {
        //            sponsignals->SigNumber = cntspon;
        //            convert(sponsignals);
        //        }
        //        else
        //            delete[] sponsignals;

        //        if (cntbs != 0)
        //        {
        //            BS104Signals->SigNumber = cntbs;
        //            emit Bs104signalsreceived(BS104Signals);
        //        }
        //        else
        //            delete[] BS104Signals;
    } catch (...)
    {
        m_log->error("Catch exception");
    }
}

void IEC104Thread::StartDT()
{
    // qDebug() << QDateTime::currentMSecsSinceEpoch() << __PRETTY_FUNCTION__;
    m_log->info("Start()");
    APCI StartDT;
    StartDT.append(I104_START);
    StartDT.append(0x04);
    StartDT.append(I104_STARTDT_ACT);
    StartDT.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_STARTDT_ACT;
    Send(0, StartDT);
    m_timer104->start();
}

void IEC104Thread::StopDT()
{
    m_log->info("Stop()");
    APCI StopDT;
    StopDT.append(I104_START);
    StopDT.append(0x04);
    StopDT.append(I104_STOPDT_ACT);
    StopDT.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_STOPDT_ACT;
    Send(0, StopDT); // ASDU = QByteArray()
    m_sendTestTimer->stop();
}

void IEC104Thread::Send(int inc, APCI apci, ASDU asdu)
{
    QByteArray ba = apci;
    ba.append(asdu);
    m_log->info("--> " + ba.toHex());
    emit WriteData(ba);

    if (inc)
        m_V_S++;
}

void IEC104Thread::Stop()
{
    m_writingToPortBlocked = true;
    StopDT();
    m_threadMustBeFinished = true;
    m_timer104->stop();
}

QByteArray IEC104Thread::CreateGI(unsigned char apdulength)
{
    QByteArray GI;
    GI.append(I104_START);
    GI.append(apdulength);
    GI.append((m_V_S & 0x007F) << 1);
    GI.append((m_V_S & 0x7F80) >> 7);
    GI.append((m_V_R & 0x007F) << 1);
    GI.append((m_V_R & 0x7F80) >> 7);
    m_command = I104_S;
    return GI;
}

QByteArray IEC104Thread::ASDUFilePrefix(unsigned char cmd, unsigned char filenum, unsigned char secnum)
{
    QByteArray ba;
    ba.append(cmd);
    ba.append(QByteArrayLiteral("\x01\x0d\x00"));
    ba.append(m_baseAdrLow);
    ba.append(m_baseAdrHigh);
    ba.append(QByteArrayLiteral("\x00\x00\x00"));
    ba.append(filenum);
    ba.append('\x00');
    ba.append(secnum);
    return ba;
}

QByteArray IEC104Thread::ASDU6Prefix(unsigned char cmd, quint32 adr)
{
    ASDU ba;
    ba.append(cmd);
    ba.append(QByteArrayLiteral("\x01\x06\x00"));
    ba.append(m_baseAdrLow);
    ba.append(m_baseAdrHigh);
    ba.append(adr);
    ba.append(adr >> 8);
    ba.append(adr >> 16);
    return ba;
}

void IEC104Thread::SendGI()
{
    ASDU GInter = ASDU6Prefix(C_IC_NA_1, 0x00);
    GInter.append(0x14);
    APCI GI = CreateGI(0x0e);
    Send(1, GI, GInter); // ASDU = QByteArray()
    m_ackVR = m_V_R;
}

template <typename T> QByteArray IEC104Thread::ToByteArray(T var)
{
    QByteArray ba;
    int sizeofvar = sizeof(var);
    for (int i = 0; i < sizeofvar; ++i)
    {
        unsigned char tmpc;
        memcpy(&tmpc, reinterpret_cast<quint8 *>(&var) + i, sizeof(quint8));
        ba.append(tmpc);
    }
    return ba;
}

void IEC104Thread::SendS()
{
    APCI Confirm;

    Confirm.append(I104_START);
    Confirm.append(0x04);
    Confirm.append(I104_S);
    Confirm.append('\x0');
    Confirm.append((m_V_R & 0x007F) << 1);
    Confirm.append((m_V_R & 0x7F80) >> 7);
    m_command = I104_S;
    Send(0, Confirm); // ASDU = QByteArray()
    m_ackVR = m_V_R;
}

void IEC104Thread::SendTestCon()
{
    APCI GI;
    GI.append(I104_START);
    GI.append(0x04);
    GI.append(I104_TESTFR_CON);
    GI.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_TESTFR_CON;
    Send(0, GI); // ASDU = QByteArray()
}

void IEC104Thread::SendTestAct()
{
    if (m_noAnswer)
    {
        emit ReconnectSignal();
        qCritical("No answer");
        return;
    }
    else
        m_noAnswer = 1;
    m_sendTestTimer->stop();
    APCI GI;
    GI.append(I104_START);
    GI.append(0x04);
    GI.append(I104_TESTFR_ACT);
    GI.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_TESTFR_ACT;
    Send(0, GI); // ASDU = QByteArray()
}

void IEC104Thread::SelectFile(char numfile)
{
    m_sectionNum = 1;
    m_isFileSending = true;
    m_log->info("FileSending set");
    m_sendTestTimer->stop();
    m_log->info("SelectFile(" + QString::number(numfile) + ")");
    ASDU cmd = ASDUFilePrefix(F_SC_NA_1, numfile, 0x00);
    cmd.append('\x01');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::CallFile(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_SC_NA_1, numFile, 0x00);
    cmd.append('\x02');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::GetSection(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_SC_NA_1, numFile, m_sectionNum);
    cmd.append('\x06');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::ConfirmSection(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_AF_NA_1, numFile, m_sectionNum);
    cmd.append('\x03');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
    m_sectionNum++;
}

void IEC104Thread::ConfirmFile(unsigned char numFile)
{
    ASDU cmd = ASDUFilePrefix(F_AF_NA_1, numFile, m_sectionNum);
    cmd.append('\x01');
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

// void IEC104Thread::FileReady(S2ConfigType *file)
void IEC104Thread::FileReady(quint16 numfile)
{
    m_isFileSending = true;
    m_log->info("FileSending set");
    //    DR = file;
    m_sectionNum = 1;
    ASDU cmd = ASDUFilePrefix(F_FR_NA_1, numfile, 0x00);
    cmd.chop(1);
    //    m_file.resize(65535);
    //    S2::StoreDataMem(&(m_file.data()[0]), DR,
    //        0x0001); // 0x0001 - номер файла конфигурации
    //    m_fileLen = static_cast<quint8>(m_file.data()[4]);
    //    m_fileLen += static_cast<quint8>(m_file.data()[5]) * 256;
    //    m_fileLen += static_cast<quint8>(m_file.data()[6]) * 65536;
    //    m_fileLen += static_cast<quint8>(m_file.data()[7]) * 16777216;
    //    m_fileLen += sizeof(S2::FileHeader); // FileHeader
    //    m_file.resize(m_fileLen);
    m_fileLen = m_file.size();
    cmd.append(m_fileLen & 0xFF);
    cmd.append((m_fileLen & 0xFF00) >> 8);
    cmd.append((m_fileLen & 0xFF0000) >> 16);
    APCI GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::SectionReady()
{
    ASDU cmd = ASDUFilePrefix(F_SR_NA_1, 0x01, m_sectionNum);
    cmd.append(m_fileLen & 0xFF);
    cmd.append((m_fileLen & 0xFF00) >> 8);
    cmd.append((m_fileLen & 0xFF0000) >> 16);
    APCI GI = CreateGI(0x13);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::SendSegments()
{
    APCI GI;
    m_KSS = 0;
    m_KSF = 0;

    unsigned int pos = 0;
    /*    if (FileLen > static_cast<quint32>(File.size()))
        {
            ERMSG("FileLen is bigger than file");
            return;
        } */
    setGeneralResponse(DataTypes::GeneralResponseTypes::DataSize, m_fileLen);
    //    emit SetDataSize(FileLen);
    ASDU cmd = ASDUFilePrefix(F_SG_NA_1, 0x01, m_sectionNum);
    cmd.append('\x0');
    unsigned char diff;

    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, static_cast<quint64>(m_file.size()) };

    do
    {
        quint32 filesize = m_file.size();
        if (filesize > SEGMENTSIZE)
            diff = SEGMENTSIZE;
        else
            diff = filesize;
        cmd.append(m_file.left(diff));
        for (int i = 0; i < diff; ++i)
        {
            m_KSS += static_cast<quint8>(m_file.data()[i]);
            m_KSF += static_cast<quint8>(m_file.data()[i]);
        }
        m_file = m_file.mid(diff);
        pos += diff;
        setGeneralResponse(DataTypes::GeneralResponseTypes::DataCount, pos);
        //        emit SetDataCount(pos);
        cmd[12] = static_cast<unsigned char>(diff);
        GI = CreateGI(diff + 17);
        Send(1, GI, cmd); // ASDU = QByteArray()
        cmd = cmd.left(13);
        QThread::msleep(100);
    } while (!m_file.isEmpty());

    cmd = ASDUFilePrefix(F_LS_NA_1, 1, m_sectionNum);
    cmd.append('\x03');
    cmd.append(m_KSS);
    m_KSS = 0;
    GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()

    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, resp);
}

void IEC104Thread::LastSection()
{
    ASDU cmd = ASDUFilePrefix(F_LS_NA_1, 1, m_sectionNum);
    cmd.append('\x01');
    cmd.append(m_KSF);
    m_KSF = 0;
    APCI GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::Com45(quint32 com)
{
    ASDU cmd = ASDU6Prefix(C_SC_NA_1, com);
    cmd.append('\x01');
    APCI GI = CreateGI(0x0e);
    Send(1, GI, cmd);
}

void IEC104Thread::Com50(quint32 adr, float param)
{
    if (adr == 910)
        setGeneralResponse(DataTypes::GeneralResponseTypes::DataSize, 11);

    //        emit SetDataSize(11);

    ASDU cmd = ASDU6Prefix(C_SE_NC_1, adr);
    cmd.append(ToByteArray(param));
    cmd.append('\x00');
    APCI GI = CreateGI(0x12);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::reqGroup(int groupNum)
{
    // qDebug(__PRETTY_FUNCTION__);
    ASDU req = ASDU6Prefix(C_IC_NA_1, 0);
    //    GTime.append(0x23);
    req.append(groupNum + 20); // group 0 (GI) -> 20, group 1 -> 21 etc
    APCI GI = CreateGI(0x0e);
    Send(1, GI, req); // ASDU = QByteArray()
    m_ackVR = m_V_R;
}

// void IEC104Thread::CorReadRequest()
//{
//    Log->info("CorReadRequest()");
//    ASDU GCor = ASDU6Prefix(C_IC_NA_1, 0);
//    GCor.append('\x16');
//    APCI GI = CreateGI(0x0e);
//    Send(1, GI, GCor); // ASDU = QByteArray()
//    AckVR = V_R;
//}

void IEC104Thread::Com51WriteTime(quint32 time)
{
    ASDU cmd = ASDU6Prefix(C_BO_NA_1, 4601);
    cmd.append(ToByteArray(time));
    APCI GI = CreateGI(0x11);
    Send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Thread::setGeneralResponse(DataTypes::GeneralResponseTypes type, quint64 data)
{
    DataTypes::GeneralResponseStruct grs;
    grs.type = type;
    grs.data = data;
    DataManager::addSignalToOutList(DataTypes::SignalTypes::GeneralResponse, grs);
}

// void IEC104Thread::convert(IEC104Thread::SponSignals *signal) { emit Sponsignalsreceived(signal); }
