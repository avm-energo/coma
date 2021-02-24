#include "servicecoma.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../interfaces/iec104.h"
#include "../interfaces/modbus.h"
#include "../interfaces/protocom.h"
#include "servicemodule.h"
ServiceComa::ServiceComa(QWidget *parent) : Coma(parent)
{
}

ServiceComa::~ServiceComa()
{
}

void ServiceComa::PrepareDialogs()
{
    m_Module = ModulePointer(new ServiceModule);
    if (!m_Module->loadSettings())
    {
        qCritical() << "No conf .xml file for this module";
        return;
    }
    m_Module->createAlarm(AlarmW);
    m_Module->create(BdaTimer);
}

void ServiceComa::setupConnection()
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
    case Board::InterfaceType::Ethernet:
    {
        device = BaseInterface::InterfacePointer(new IEC104());
        break;
    }
    case Board::InterfaceType::RS485:
    {
        device = BaseInterface::InterfacePointer(new ModBus());
        break;
    }
    default:
        qFatal("Connection type error");
    }
    BaseInterface::setIface(std::move(device));
    Coma::setupConnection();
}
