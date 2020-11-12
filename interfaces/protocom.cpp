#include "protocom.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/modulebsi.h"
#include "protocomthread.h"
#include "settingstypes.h"
#include "usbhidport.h"

#include <QThread>

const QMap<Queries::Commands, CN::Commands> Protocom::m_dict { { Queries::Commands::QC_StartFirmwareUpgrade,
                                                                   CN::Commands::WriteUpgrade },
    { Queries::QC_SetStartupValues, CN::Commands::WriteStartupValues },
    { Queries::QC_ClearStartupValues, CN::Commands::EraseStartupValues },
    { Queries::QC_SetNewConfiguration, CN::Commands::WriteInitValues },
    { Queries::QC_EraseJournals, CN::Commands::EraseStartupValues },
    { Queries::QC_EraseTechBlock, CN::Commands::EraseStartupValues }, { Queries::QC_Test, CN::Commands::Test } };

Protocom::Protocom(QObject *parent) : BaseInterface(parent)
{
}

bool Protocom::start(const ConnectStruct &st)
{
    Q_ASSERT(Board::GetInstance().interfaceType() == Board::InterfaceType::USB);
    Q_ASSERT(std::holds_alternative<UsbHidSettings>(st.settings));
    INFOMSG("Modbus: connect");
    if (!std::holds_alternative<UsbHidSettings>(st.settings))
        return false;

    UsbHidPort *port = new UsbHidPort(std::get<UsbHidSettings>(st.settings), Log, this);
    ProtocomThread *parser = new ProtocomThread(this);

    QThread *portThread = new QThread;
    QThread *parseThread = new QThread;

    port->moveToThread(portThread);
    parser->moveToThread(parseThread);

    // FIXME Разобраться с удалением и закрытием потоков
    connect(portThread, &QThread::started, port, &UsbHidPort::poll);
    connect(parseThread, &QThread::started, parser, &ProtocomThread::parse);

    connect(port, &UsbHidPort::finished, portThread, &QThread::quit);
    connect(port, &UsbHidPort::finished, portThread, &QThread::deleteLater);
    connect(port, &UsbHidPort::finished, port, &UsbHidPort::deleteLater);

    connect(port, &UsbHidPort::dataReceived, [&](const QByteArray &ba) {
        parser->appendReadDataChunk(ba);
        parseThread->start();
    });

    connect(parser, &ProtocomThread::writeDataAttempt, port, &UsbHidPort::writeDataAttempt);

    if (!port->setupConnection())
        return false;

    Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    portThread->start();
    parseThread->start();
    return true;
}

void Protocom::reqTime()
{
    CommandStruct inp { CN::Commands::ReadTime, 0, 0, {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqFile(quint32 filenum, bool isConfigFile)
{
    // TODO Как использовать флаг?
    CommandStruct inp {
        CN::Commands::ReadFile, // Command
        filenum,                // File number
        QVariant(),             // Null float arg
        {}                      // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct inp {
        CN::Commands::ReadBlkData,                                 // Command
        sigAdr,                                                    // Board type
        sigCount,                                                  // Empty float arg
        QByteArray(sizeof(sigCount) * sigCount, Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void Protocom::reqBSI()
{
    CommandStruct inp {
        CN::Commands::ReadBlkStartInfo,                       // Command
        BoardTypes::BT_NONE,                                  // Board type
        QVariant(),                                           // Null float arg
        QByteArray(sizeof(ModuleBSI::Bsi), Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void Protocom::writeFile(quint32 filenum, const QByteArray &file)
{
    CommandStruct inp {
        CN::Commands::WriteFile, // Command
        filenum,                 // File number
        QVariant(),              // Null float arg
        file                     // Buffer with file
    };
    DataManager::addToInQueue(inp);
}

void Protocom::writeTime(quint32 time)
{
    CommandStruct inp { CN::Commands::WriteTime, time, QVariant(), {} };
    DataManager::addToInQueue(inp);
}

void Protocom::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct inp { CN::Commands::ReadBlkData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
}

void handleInt(CN::Commands cmd, int block)
{
    CommandStruct inp {
        cmd,          // Command
        block,        // Block for erasure
        QVariant(),   // Null arg
        QByteArray {} // Empty ba
    };
    DataManager::addToInQueue(inp);
}

void handleCommand(CN::Commands cmd)
{
    CommandStruct inp { cmd, QVariant(), QVariant(), QByteArray {} };
    DataManager::addToInQueue(inp);
}

void Protocom::writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> list)
{
    // list not supported, only first
    auto member = list.first();
    Q_ASSERT(list.size() < 2);
    // NOTE using enum after cpp20
    using namespace CN;

    auto ourCmd = translate(cmd);
    switch (ourCmd)
    {
    case Commands::EraseStartupValues:
    {
        handleInt(ourCmd, member.data.toInt());
    }
    default:
    {
        handleCommand(ourCmd);
    }
    }
}
