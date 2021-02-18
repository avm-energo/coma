#include "debugcoma.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../interfaces/protocom.h"
#include "tunemodule.h"

DebugComa::DebugComa(QWidget *parent) : Coma(parent)
{
}

DebugComa::~DebugComa()
{
}

void DebugComa::PrepareDialogs()
{
    m_Module = ModulePointer(new TuneModule);
    if (!m_Module->loadSettings())
    {
        qCritical() << "No conf .xml file for this module";
        return;
    }
    m_Module->create(AlarmW);
    m_Module->create(BdaTimer);
}

void DebugComa::setupConnection()
{
    auto const &board = Board::GetInstance();
    connect(&DataManager::GetInstance(), &DataManager::bitStringReceived, &Board::GetInstance(), &Board::update);
    BaseInterface::InterfacePointer device;
    switch (board.interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        device = BaseInterface::InterfacePointer(new Protocom());
        break;
    }
    default:
        qFatal("Connection type error");
    }
    BaseInterface::setIface(std::move(device));
    Coma::setupConnection();
}
