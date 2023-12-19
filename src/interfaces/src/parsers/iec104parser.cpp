#include "interfaces/parsers/iec104parser.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <gen/pch.h>
#include <interfaces/types/iec104_types.h>
#include <s2/s2util.h>

#define I104_START 0x68

#define I104_I 0x00
#define I104_S 0x01
#define I104_U 0x03
#define I104_WRONG 0xFF

#define I104_W 8

#define I104_STARTDT_ACT 0x07
#define I104_STARTDT_CON 0x0B
#define I104_STOPDT_ACT 0x13
#define I104_STOPDT_CON 0x23
#define I104_TESTFR_ACT 0x43
#define I104_TESTFR_CON 0x83u

// определения возвращаемого значения функции isIncomeDataValid
#define I104_RCVNORM 0x00
#define I104_RCVSMSIZE 0x01
#define I104_RCVWRONG 0x02

#define SECTIONSIZE 2048
#define SEGMENTSIZE 230
#define OFFSET 13
#define FHSIZE 16
#define SYSTEM_JOUR_SIZE 65568

namespace old
{

// Old
enum Commands
{
    CM104_REQGROUP,
    CM104_COM51,
    CM104_COM45,
    CM104_COM50,
    CM104_REQFILE,
    CM104_REQCONFIGFILE,
    CM104_WRITEFILE
};

struct CommandStruct
{
    Commands cmd;
    quint32 address;
    float flarg;
    bool blarg;
    QByteArray ba;
};

struct QualifierVariableStructute
{
    quint8 Number; // number of Informational Objects
    quint8 SQ;     // Single <0> or Series <1> of Objects
};

struct COT
{
    quint8 cause;     // <0..63> cause number
    quint8 confirm;   // <0> - positive , <1> - negative
    quint8 test;      // <0> - not a test, <1> - test
    quint8 initiator; // number of initiating address
};

struct DataUnitIdentifier
{
    Iec104::MessageDataType typeIdent;
    QualifierVariableStructute qualifier;
    COT cause;
    quint8 commonAdrASDU;
};

} // namespace old
Q_DECLARE_METATYPE(old::CommandStruct)

namespace Interface
{

QMutex IEC104Parser::s_ParseReadMutex;
QMutex IEC104Parser::s_ParseWriteMutex;

using namespace old;
using namespace Iec104;

IEC104Parser::IEC104Parser(RequestQueue &queue, QObject *parent) : QObject(parent), m_queue(queue)
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
    m_log.init("ethernetThread.log");
}

void IEC104Parser::setBaseAdr(quint16 adr)
{
    m_baseAdr = adr;
    m_baseAdrHigh = adr >> 8;
    m_baseAdrLow = adr;
}

void IEC104Parser::run()
{
    m_timer104 = new QTimer(this);
    m_timer104->setInterval(15000);
    m_sendTestTimer = new QTimer(this);
    m_sendTestTimer->setInterval(5000);

#ifndef DEBUG
    connect(m_timer104, &QTimer::timeout, this, &IEC104Parser::stop);
    connect(m_sendTestTimer, &QTimer::timeout, this, &IEC104Parser::SendTestAct);
    m_sendTestTimer->start();
#endif

    startDT();
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
                // если поймали правильное начало данных, то переходим к их обработке
                if (tmpi == Error::Msg::NoError)
                {
                    if (m_APDUFormat == I104_I)
                    {
                        Q_ASSERT(tmpba.size() > 6);
                        tmpba = tmpba.mid(6);
                        parseIFormat(tmpba); // без APCI
                    }
                }
            }
        }
        if (!m_isFileSending && !m_writingToPortBlocked)
        {
            auto opt = m_queue.get().getFromQueue();
            if (!opt.has_value())
            {
                QCoreApplication::processEvents();
                continue;
            }

            const auto command(opt.value());
            switch (command.command)
            {
            case Commands::C_ReqFloats:
            {
                auto group = command.arg1.value<quint32>();
                reqGroup(group);
                break;
            }
            default:
                qCritical() << "Undefined command: " << command.command;
                break;
            }

            //            Iec104::CommandStruct inp;
            //            switch (inp.cmd)
            //            {
            //            case Iec104::CM104_COM45:
            //                com45(inp.address, inp.blarg);
            //                break;
            //            case Iec104::CM104_COM50:
            //                com50(inp.address, inp.flarg);
            //                break;
            //            case Iec104::CM104_WRITEFILE:
            //            {
            //                m_fileIsConfigFile = DataTypes::FileFormat::Binary;
            //                m_file = inp.ba;
            //                fileReady(inp.address);
            //                break;
            //            }
            //            case Iec104::CM104_REQCONFIGFILE:
            //                m_fileIsConfigFile = DataTypes::FileFormat::DefaultS2;
            //                selectFile(inp.address);
            //                break;
            //            case Iec104::CM104_REQFILE:
            //                m_fileIsConfigFile = DataTypes::FileFormat::Binary;
            //                selectFile(inp.address);
            //                break;
            //            case Iec104::CM104_COM51:
            //                com51WriteTime(inp.address);
            //                break;
            //            case Iec104::CM104_REQGROUP:
            //                reqGroup(inp.address);
            //                break;
            //            default:
            //                break;
            //            }
            if (m_V_R > (m_ackVR + I104_W))
                sendS();
        }
        QCoreApplication::processEvents();
    }
    m_log.info("Finish thread");
    emit finished();
}

void IEC104Parser::processReadBytes(QByteArray ba)
{
    if (m_isFirstParse)
        m_log.info("<-- " + ba.toHex());
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
        Q_ASSERT(basize >= missing_num);
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
    processReadBytes(ba);
    m_isFirstParse = true;
}

Error::Msg IEC104Parser::isIncomeDataValid(QByteArray &ba)
{
    if (ba.size() < 3)
        return Error::SizeError;
    try
    {
        if (ba.at(0) != 0x68)
            return Error::Msg::GeneralError;
        m_APDULength = static_cast<quint8>(ba.at(1)); // в 1-м байте лежит длина
        if ((m_APDULength < 4) || (m_APDULength > 253))
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
                m_log.error("V_Rrcv != V_R");
                return Error::Msg::GeneralError;
            }
            m_V_R++;
            if (ba.size() < 6)
                return Error::SizeError;
            quint16 V_Srcv = static_cast<quint8>(ba.at(5)) * 256 + static_cast<quint8>(ba.at(4) & 0xFE);
            V_Srcv >>= 1;
            if (V_Srcv != m_V_S)
                m_V_S = V_Srcv; // временно, нужно исправить проблему несовпадения s посылок
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
                sendGI();
                m_writingToPortBlocked = false;
            }
            if ((baat2 == I104_STOPDT_CON)
                && (m_command == I104_STOPDT_ACT)) // если пришло подтверждение стопа и перед этим мы стоп запрашивали
                m_command = I104_STOPDT_CON;
            if ((baat2 == I104_TESTFR_CON)
                && (m_command == I104_TESTFR_ACT)) // если пришло подтверждение теста и перед этим мы тест запрашивали
                m_command = I104_TESTFR_CON;
            if (baat2 == I104_TESTFR_ACT)
                sendTestCon();

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

bool IEC104Parser::handleFile(QByteArray &ba, S2::FilesEnum addr, DataTypes::FileFormat format)
{
    using DataTypes::FileFormat;
    switch (format)
    {
    case FileFormat::DefaultS2:
    {
        emit responseSend(ba);
        break;
    }
    default:
    {
        DataTypes::S2FilePack outlist;
        if (!S2Util::RestoreData(ba, outlist))
            return false;
        Q_ASSERT(outlist.size() == 1 && "Only one file supported");
        S2::FileStruct df { addr, outlist.first().data };
        emit responseSend(df);
        break;
    }
    }
    DataTypes::GeneralResponseStruct resp { DataTypes::GeneralResponseTypes::Ok, static_cast<quint64>(ba.size()) };
    emit responseSend(resp);
    return true;
}

void IEC104Parser::parseIFormat(QByteArray &ba) // основной разборщик
{
    quint32 basize = ba.size();
    DataUnitIdentifier DUI;
    try
    {
        if (basize < 6)
        {
            qDebug() << Error::SizeError;
            return;
        }
        DUI.typeIdent = static_cast<MessageDataType>(ba.at(0));
        DUI.qualifier.Number = ba.at(1) & 0x7f;
        DUI.qualifier.SQ = ba.at(1) >> 7;
        DUI.cause.cause = ba.at(2) & 0x3F;
        DUI.cause.confirm = (ba.at(2) >> 6) & 0x01;
        DUI.cause.test = ba.at(2) >> 7;
        DUI.cause.initiator = ba.at(3);
        DUI.commonAdrASDU = ba.at(4) + ba.at(5) * 256;
        qDebug() << "Address ASDU: " << DUI.commonAdrASDU;
        if (DUI.commonAdrASDU != m_baseAdr) // not our station
            return;
        quint32 objectAdr = 0;
        quint32 index = 6;
        int fileSize;
        int i; //, cntfl = 0, cntflTimestamp = 0, cntspon = 0, cntbs = 0;

        for (i = 0; i < DUI.qualifier.Number; i++)
        {
            if ((i == 0) || (DUI.qualifier.SQ == 0))
            {
                Q_ASSERT(basize >= (index + 3));
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

            case MessageDataType::M_EI_NA_1:
            {
                break;
            }
            case MessageDataType::M_ME_TF_1:
            {
                Q_ASSERT(basize >= (index + 12));
                DataTypes::FloatWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                float value;
                memcpy(&value, &(ba.data()[index]), 4);
                index += 4;
                signal.sigVal = value;
                signal.sigQuality = ba.at(index++);
                quint64 time;
                memcpy(&time, &(ba.data()[index]), 7);
                index += 7;
                signal.CP56Time = time;
                emit responseSend(signal);
                break;
            }

            case MessageDataType::C_IC_NA_1:
                break;

            case MessageDataType::M_ME_NC_1:
            {
                Q_ASSERT(basize >= (index + 5));
                DataTypes::FloatWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                memcpy(&signal.sigVal, &(ba.data()[index]), 4);
                index += 4;
                signal.sigQuality = ba.at(index++);
                emit responseSend(signal);
                break;
            }

            case MessageDataType::M_SP_NA_1:
            {
                Q_ASSERT(basize >= index);
                DataTypes::SinglePointWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                signal.sigVal = ba.at(index++);
                emit responseSend(signal);
                break;
            }

            case MessageDataType::M_SP_TB_1:
            {
                Q_ASSERT(basize >= (index + 8));
                DataTypes::SinglePointWithTimeStruct signal;
                signal.sigAdr = objectAdr;
                signal.sigVal = ba.at(index++);
                memcpy(&signal.CP56Time, &(ba.data()[index]), 7);
                index += 7;
                emit responseSend(signal);
                break;
            }

            case MessageDataType::M_BO_NA_1:
            {
                Q_ASSERT(basize >= (index + 5));
                DataTypes::BitStringStruct signal;
                signal.sigAdr = objectAdr;
                memcpy(&signal.sigVal, &(ba.data()[index]), 4);
                index += 4;
                memcpy(&signal.sigQuality, &(ba.data()[index++]), 1);
                emit responseSend(signal);
                break;
            }

            case MessageDataType::F_SR_NA_1:
            {
                Q_ASSERT(basize >= 11);
                m_log.info("Section ready");
                unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                getSection(filenum);
                break;
            }

            case MessageDataType::F_FR_NA_1:
            {
                Q_ASSERT(basize >= 14);
                m_log.info("File ready");
                unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                m_readData.clear();
                m_readSize = 0;
                m_readPos = 0;
                fileSize = (static_cast<quint8>(ba.at(13)) << 16) | (static_cast<quint8>(ba.at(12)) << 8)
                    | (static_cast<quint8>(ba.at(11)));
                setGeneralResponse(DataTypes::GeneralResponseTypes::DataSize, fileSize);
                callFile(filenum);
                break;
            }

            case MessageDataType::F_SG_NA_1:
            {
                Q_ASSERT(basize >= (m_readSize + 14));
                m_log.info(
                    "Segment ready: RDSize=" + QString::number(ba.at(12), 16) + ", num=" + QString::number(ba.at(13)));
                m_readSize = static_cast<quint8>(ba.at(12));
                m_readSize &= 0xFF;
                if (m_readSize) //>= RDLength)
                {
                    m_readData.append(&(ba.data()[13]), m_readSize);
                    m_readPos += m_readSize;
                    setGeneralResponse(DataTypes::GeneralResponseTypes::DataCount, m_readPos);
                }
                break;
            }

            case MessageDataType::F_LS_NA_1:
            {
                Q_ASSERT(basize >= 13);
                m_log.info("Last section, ba[12] = " + QString::number(ba.at(12)));
                switch (ba.at(12))
                {
                case 1:
                {
                    unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                    confirmFile(filenum);
                    m_sendTestTimer->start();
                    m_isFileSending = false;
                    m_log.info("FileSending clear");
                    auto filetype = S2::FilesEnum(ba.at(9));
                    if (!handleFile(m_readData, filetype, m_fileIsConfigFile))
                        m_log.error("Error while income file S2 parsing");
                    m_readPos = 0;
                    m_readSize = 0;
                    break;
                }

                case 3:
                {
                    unsigned char filenum = ba.at(9) | (ba.at(10) << 8);
                    confirmSection(filenum);
                    break;
                }
                }
                break;
            }

            case MessageDataType::F_SC_NA_1:
            {
                Q_ASSERT(basize >= 13);
                if (ba.at(12) == 0x02) //запрос файла
                {
                    m_log.info("File query");
                    sectionReady();
                }
                if (ba.at(12) == 0x06)
                {
                    m_log.info("Segment query");
                    sendSegments();
                }
                break;
            }

            case MessageDataType::F_AF_NA_1:
            {
                Q_ASSERT(basize >= 13);
                m_log.info("Last section of file " + QString::number(ba[12]) + " confirm");
                if (ba.at(12) == 0x03) // подтверждение секции
                    lastSection();
                if (ba.at(12) == 0x01) // подтверждение файла
                {
                    m_log.info("FileSending clear");
                    m_isFileSending = false;
                    emit sendMessagefromParse();
                }
                break;
            }

            case MessageDataType::C_SC_NA_1:
            {
                if (DUI.cause.cause == 10)
                {
                    DataTypes::GeneralResponseStruct grs;
                    grs.type = DataTypes::GeneralResponseTypes::Ok;
                    emit responseSend(grs);
                }
                break;
            }

            case MessageDataType::C_SE_NC_1:
            {
                Q_ASSERT(basize >= 14);
                if (DUI.cause.cause == 10)
                    m_signalCounter++;

                setGeneralResponse(DataTypes::GeneralResponseTypes::DataCount, m_signalCounter);
                quint32 adr = ba.at(6) + (ba.at(7) + 1) * 256; //+ (ba[8]<<16);

                if ((adr == 920) && (DUI.cause.cause == 10)) // если адрес последнего параметра коррекции
                {
                    if (ba.at(13) == 0)
                    {
                        setGeneralResponse(DataTypes::GeneralResponseTypes::Ok);
                        setGeneralResponse(
                            DataTypes::GeneralResponseTypes::DataCount, 11); // send to progressbar max count of bytes
                    }
                    m_signalCounter = 0;
                }
                break;
            }

            default:
                qDebug() << DUI.typeIdent;
            }
        }
    } catch (...)
    {
        m_log.error("Catch exception");
    }
}

void IEC104Parser::startDT()
{
    // qDebug() << QDateTime::currentMSecsSinceEpoch() << __PRETTY_FUNCTION__;
    m_log.info("Start()");
    QByteArray StartDT;
    StartDT.append(I104_START);
    StartDT.append(0x04);
    StartDT.append(I104_STARTDT_ACT);
    StartDT.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_STARTDT_ACT;
    send(0, StartDT);
    m_timer104->start();
}

void IEC104Parser::stopDT()
{
    m_log.info("Stop()");
    QByteArray StopDT;
    StopDT.append(I104_START);
    StopDT.append(0x04);
    StopDT.append(I104_STOPDT_ACT);
    StopDT.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_STOPDT_ACT;
    send(0, StopDT); // ASDU = QByteArray()
    m_sendTestTimer->stop();
}

void IEC104Parser::send(int inc, QByteArray apci, QByteArray asdu)
{
    QByteArray ba = apci;
    ba.append(asdu);
    m_log.info("--> " + ba.toHex());
    emit writeData(ba);

    if (inc)
        m_V_S++;
}

void IEC104Parser::stop()
{
    m_writingToPortBlocked = true;
    stopDT();
    m_threadMustBeFinished = true;
    m_timer104->stop();
}

QByteArray IEC104Parser::createGI(unsigned char apdulength)
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

QByteArray IEC104Parser::ASDUFilePrefix(MessageDataType cmd, unsigned char filenum, unsigned char secnum)
{
    QByteArray ba;
    ba.append(std_ext::to_underlying(cmd));
    ba.append(QByteArrayLiteral("\x01\x0d\x00"));
    ba.append(m_baseAdrLow);
    ba.append(m_baseAdrHigh);
    ba.append(QByteArrayLiteral("\x00\x00\x00"));
    ba.append(filenum);
    ba.append('\x00');
    ba.append(secnum);
    return ba;
}

QByteArray IEC104Parser::ASDU6Prefix(MessageDataType cmd, quint32 adr)
{
    QByteArray ba;
    ba.append(std_ext::to_underlying(cmd));
    ba.append(QByteArrayLiteral("\x01\x06\x00"));
    ba.append(m_baseAdrLow);
    ba.append(m_baseAdrHigh);
    ba.append(adr);
    ba.append(adr >> 8);
    ba.append(adr >> 16);
    return ba;
}

void IEC104Parser::sendGI()
{
    QByteArray GInter = ASDU6Prefix(MessageDataType::C_IC_NA_1, 0x00);
    GInter.append(0x14);
    QByteArray GI = createGI(0x0e);
    send(1, GI, GInter); // ASDU = QByteArray()
    m_ackVR = m_V_R;
}

template <typename T> QByteArray IEC104Parser::ToByteArray(T var)
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

void IEC104Parser::sendS()
{
    QByteArray Confirm;
    Confirm.append(I104_START);
    Confirm.append(0x04);
    Confirm.append(I104_S);
    Confirm.append('\x0');
    Confirm.append((m_V_R & 0x007F) << 1);
    Confirm.append((m_V_R & 0x7F80) >> 7);
    m_command = I104_S;
    send(0, Confirm); // ASDU = QByteArray()
    m_ackVR = m_V_R;
}

void IEC104Parser::sendTestCon()
{
    QByteArray GI;
    GI.append(I104_START);
    GI.append(0x04);
    GI.append(I104_TESTFR_CON);
    GI.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_TESTFR_CON;
    send(0, GI); // ASDU = QByteArray()
}

void IEC104Parser::SendTestAct()
{
    if (m_noAnswer)
    {
        emit reconnectSignal();
        qCritical("No answer");
        return;
    }
    else
        m_noAnswer = 1;
    m_sendTestTimer->stop();
    QByteArray GI;
    GI.append(I104_START);
    GI.append(0x04);
    GI.append(I104_TESTFR_ACT);
    GI.append(QByteArrayLiteral("\x00\x00\x00"));
    m_command = I104_TESTFR_ACT;
    send(0, GI); // ASDU = QByteArray()
}

void IEC104Parser::selectFile(char numfile)
{
    m_sectionNum = 1;
    m_isFileSending = true;
    m_log.info("FileSending set");
    m_sendTestTimer->stop();
    m_log.info("SelectFile(" + QString::number(numfile) + ")");
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_SC_NA_1, numfile, 0x00);
    cmd.append('\x01');
    QByteArray GI = createGI(0x11);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::callFile(unsigned char numFile)
{
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_SC_NA_1, numFile, 0x00);
    cmd.append('\x02');
    QByteArray GI = createGI(0x11);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::getSection(unsigned char numFile)
{
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_SC_NA_1, numFile, m_sectionNum);
    cmd.append('\x06');
    QByteArray GI = createGI(0x11);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::confirmSection(unsigned char numFile)
{
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_AF_NA_1, numFile, m_sectionNum);
    cmd.append('\x03');
    QByteArray GI = createGI(0x11);
    send(1, GI, cmd); // ASDU = QByteArray()
    m_sectionNum++;
}

void IEC104Parser::confirmFile(unsigned char numFile)
{
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_AF_NA_1, numFile, m_sectionNum);
    cmd.append('\x01');
    QByteArray GI = createGI(0x11);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::fileReady(quint16 numfile)
{
    m_isFileSending = true;
    m_log.info("FileSending set");
    m_sectionNum = 1;
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_FR_NA_1, numfile, 0x00);
    cmd.chop(1);
    m_fileLen = m_file.size();
    cmd.append(m_fileLen & 0xFF);
    cmd.append((m_fileLen & 0xFF00) >> 8);
    cmd.append((m_fileLen & 0xFF0000) >> 16);
    QByteArray GI = createGI(0x12);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::sectionReady()
{
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_SR_NA_1, 0x01, m_sectionNum);
    cmd.append(m_fileLen & 0xFF);
    cmd.append((m_fileLen & 0xFF00) >> 8);
    cmd.append((m_fileLen & 0xFF0000) >> 16);
    QByteArray GI = createGI(0x13);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::sendSegments()
{
    QByteArray GI;
    m_KSS = 0;
    m_KSF = 0;

    unsigned int pos = 0;
    setGeneralResponse(DataTypes::GeneralResponseTypes::DataSize, m_fileLen);
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_SG_NA_1, 0x01, m_sectionNum);
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
        GI = createGI(diff + 17);
        send(1, GI, cmd); // ASDU = QByteArray()
        cmd = cmd.left(13);
        QThread::msleep(100);
    } while (!m_file.isEmpty());

    cmd = ASDUFilePrefix(MessageDataType::F_LS_NA_1, 1, m_sectionNum);
    cmd.append('\x03');
    cmd.append(m_KSS);
    m_KSS = 0;
    GI = createGI(0x12);
    send(1, GI, cmd); // ASDU = QByteArray()
    emit responseSend(resp);
}

void IEC104Parser::lastSection()
{
    QByteArray cmd = ASDUFilePrefix(MessageDataType::F_LS_NA_1, 1, m_sectionNum);
    cmd.append('\x01');
    cmd.append(m_KSF);
    m_KSF = 0;
    QByteArray GI = createGI(0x12);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::com45(quint32 com, bool value)
{
    quint8 data = value ? '\x01' : '\x00';
    QByteArray cmd = ASDU6Prefix(MessageDataType::C_SC_NA_1, com);
    cmd.append(data);
    QByteArray GI = createGI(0x0e);
    send(1, GI, cmd);
}

void IEC104Parser::com50(quint32 adr, float param)
{
    if (adr == 910)
        setGeneralResponse(DataTypes::GeneralResponseTypes::DataSize, 11);

    QByteArray cmd = ASDU6Prefix(MessageDataType::C_SE_NC_1, adr);
    cmd.append(ToByteArray(param));
    cmd.append('\x00');
    QByteArray GI = createGI(0x12);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::reqGroup(int groupNum)
{
    QByteArray req = ASDU6Prefix(MessageDataType::C_IC_NA_1, 0);
    req.append(groupNum + 20); // group 0 (GI) -> 20, group 1 -> 21 etc
    QByteArray GI = createGI(0x0e);
    send(1, GI, req); // ASDU = QByteArray()
    m_ackVR = m_V_R;
}

void IEC104Parser::com51WriteTime(quint32 time)
{
    QByteArray cmd = ASDU6Prefix(MessageDataType::C_BO_NA_1, 4601);
    cmd.append(ToByteArray(time));
    QByteArray GI = createGI(0x11);
    send(1, GI, cmd); // ASDU = QByteArray()
}

void IEC104Parser::setGeneralResponse(DataTypes::GeneralResponseTypes type, quint64 data)
{
    DataTypes::GeneralResponseStruct resp { type, data };
    emit responseSend(resp);
}

} // namespace Interface
