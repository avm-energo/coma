#include "configstorage.h"

#include "../interfaces/baseinterface.h"
#include "../interfaces/interfacesettings.h"

ConfigStorage::ConfigStorage(token, QObject *parent) : QObject(parent), mSettings(new ModuleSettings)
{
}

/// \brief Constant getter for module settings.
const ModuleSettings &ConfigStorage::getModuleSettings() const
{
    return *mSettings;
}

/// \brief Cleaning module settings.
void ConfigStorage::clearModuleSettings()
{
    mSettings->clear();
}

/// \brief Slot for saving module's signal record.
void ConfigStorage::signalDataReceive(const quint32 id, const quint32 addr, //
    const quint16 count, const ModuleTypes::SignalType sigType)
{
    if (id == 0)
        qWarning() << "Invalid signal id: " << id;
    else if (addr == 0)
        qWarning() << "Invalid signal address, signal id: " << id;
    else if (count == 0)
        qWarning() << "Invalid signal count, signal id: " << id;
    else
        mSettings->appendSignal(id, { addr, count, sigType });
}

/// \brief Slot for saving check's tab record.
void ConfigStorage::tabDataReceive(const quint32 id, const QString &name)
{
    if (id == 0)
        qWarning() << "Invalid tab id: " << id;
    else if (name == "")
        qWarning() << "Empty tab name, tab id: " << id;
    else
        mSettings->appendTab(id, name);
}

/// \brief Slot for saving check's section record.
void ConfigStorage::sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead)
{
    mSettings->appendSection({ secHead, sgmap });
}

/// \brief Slot for saving module's alarm record.
void ConfigStorage::alarmDataReceive(const Modules::AlarmType aType, const quint32 addr, //
    const QString &desc, const QList<quint32> &highlights)
{
    mSettings->appendAlarm(aType, addr, desc);
    mSettings->appendHighlight(aType, addr, highlights);
}

/// \brief Slot for saving module a work journal's record.
void ConfigStorage::workJourDataReceive(const quint32 id, const QString &desc)
{
    mSettings->appendWorkJournal(id, desc);
}

/// \brief Slot for saving module a measurement journal's record.
void ConfigStorage::measJourDataReceive(const quint32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    mSettings->appendMeasJournal(index, header, type, visib);
}

/// \brief Slot for saving module's protocol groups
void ConfigStorage::protocolDescriptionReceived(const parseXChangeStruct &str)
{
    auto &sigMap = getModuleSettings().getSignals();
    if (sigMap.contains(str.sigId))
    {
        auto signal = sigMap.value(str.sigId);
        Board::InterfaceType ifaceType = str.interfaceType.value<Board::InterfaceType>();
        ProtocolDescription *descr = Interface::BaseInterface::iface()->settings();
        switch (ifaceType)
        {
        case Board::USB:
            descr->addGroup(ProtocomGroup({ signal.startAddr, signal.count, str.par2.value<quint16>() }));
            break;
        case Board::RS485:
            descr->addGroup(ModbusGroup({ signal.startAddr, signal.count, str.par2.value<quint16>() }));
            break;
        case Board::Ethernet:
            descr->addGroup(
                Iec104Group({ signal.startAddr, signal.count, str.par2.value<quint16>(), str.par3.value<quint16>() }));
            break;
        default:
            break;
        }
    }
}
