#include "emulator.h"

#include "../gen/datamanager/datamanager.h"
#include "protocomprivate.h"
Emulator::Emulator(QObject *parent) : BaseInterface(parent)
{
}

bool Emulator::start(const ConnectStruct &st)
{
    Q_ASSERT(std::holds_alternative<EmulatorSettings>(st.settings));

    if (!std::holds_alternative<EmulatorSettings>(st.settings))
        return false;
    m_settings = std::get<EmulatorSettings>(st.settings);
    emit stateChanged(BaseInterface::State::Run);
    return true;
}

void Emulator::reqStartup(quint32, quint32)
{
}

void Emulator::reqBSI()
{
    constexpr auto bsiCounter = sizeof(Modules::StartupInfoBlock) / sizeof(quint32);
    Modules::StartupInfoBlock bsiStruct;
    bsiStruct.MTypeB = m_settings.typeB;
    bsiStruct.MTypeM = m_settings.typeM;
    std::array<quint32, bsiCounter> &bsiArray = *reinterpret_cast<std::array<quint32, bsiCounter> *>(&bsiStruct);
    int counter = 1;
    for (const auto i : bsiArray)
    {
        DataTypes::BitStringStruct bitString;
        bitString.sigAdr = counter++;
        bitString.sigVal = i;
        DataManager::GetInstance().addSignalToOutList(bitString);
    }
}

void Emulator::reqBSIExt()
{
}

void Emulator::reqFile(quint32, FileFormat)
{
}

void Emulator::writeFile(quint32, const QByteArray &)
{
}

void Emulator::reqTime()
{
}

void Emulator::writeTime(quint32)
{
}

void Emulator::writeCommand(Queries::Commands, QVariant)
{
}

void Emulator::reqFloats(quint32, quint32)
{
}

void Emulator::reqBitStrings(quint32, quint32)
{
}

// InterfaceSettings Emulator::parseSettings(QDomElement domElement) const
//{
//    return BaseInterface::parseSettings<Proto::ProtocomGroup>(domElement);
//}
