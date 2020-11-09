#include "usbcontrol.h"

#include "../gen/datamanager.h"
#include "../gen/modulebsi.h"
#include "defines.h"

const QMap<Queries::Commands, CN::Commands> UsbControl::m_dict { { Queries::Commands::QC_StartFirmwareUpgrade,
                                                                     CN::Commands::WriteUpgrade },
    { Queries::QC_SetStartupValues, CN::Commands::WriteStartupValues },
    { Queries::QC_ClearStartupValues, CN::Commands::EraseStartupValues },
    { Queries::QC_SetNewConfiguration, CN::Commands::WriteInitValues },
    { Queries::QC_EraseJournals, CN::Commands::EraseStartupValues },
    { Queries::QC_EraseTechBlock, CN::Commands::EraseStartupValues }, { Queries::QC_Test, CN::Commands::Test } };

UsbControl::UsbControl(QObject *parent) : BaseInterface(parent)
{
}

void UsbControl::reqTime()
{
    CommandStruct inp { CN::Commands::ReadTime, 0, 0, {} };
    DataManager::addToInQueue(inp);
}

void UsbControl::reqFile(quint32 filenum, bool isConfigFile)
{
    // TODO Как использовать флаг?
    CommandStruct inp {
        CN::Commands::ReadFile, // Command
        filenum,                // File number
        0,                      // Empty float arg
        {}                      // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
}

void UsbControl::reqStartup(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct inp {
        CN::Commands::ReadBlkData,                                 // Command
        sigAdr,                                                    // Board type
        sigCount,                                                  // Empty float arg
        QByteArray(sizeof(sigCount) * sigCount, Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void UsbControl::reqBSI()
{
    CommandStruct inp {
        CN::Commands::ReadBlkStartInfo,                       // Command
        BoardTypes::BT_NONE,                                  // Board type
        0,                                                    // Empty float arg
        QByteArray(sizeof(ModuleBSI::Bsi), Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void UsbControl::writeFile(quint32 filenum, const QByteArray &file)
{
    CommandStruct inp {
        CN::Commands::WriteFile, // Command
        filenum,                 // File number
        0,                       // Empty float arg
        file                     // Buffer with file
    };
    DataManager::addToInQueue(inp);
}

void UsbControl::writeTime(quint32 time)
{
    CommandStruct inp { CN::Commands::WriteTime, time, 0, {} };
    DataManager::addToInQueue(inp);
}

void UsbControl::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    CommandStruct inp { CN::Commands::ReadBlkData, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
}

void UsbControl::reqBitStrings(quint32 sigAdr, quint32 sigCount)
{
}

void handleInt(CN::Commands cmd, int block)
{
    CommandStruct inp {
        cmd,          // Command
        block,        // Block for erasure
        0,            // Empty arg
        QByteArray {} // Empty ba
    };
    DataManager::addToInQueue(inp);
}

void handleCommand(CN::Commands cmd)
{
    CommandStruct inp { cmd, 0, 0, QByteArray {} };
    DataManager::addToInQueue(inp);
}

void UsbControl::writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> list)
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
