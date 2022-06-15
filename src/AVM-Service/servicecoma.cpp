#include "servicecoma.h"

#include "../comaversion/comaversion.h"
#include "../interfaces/iec104.h"
#include "../interfaces/modbus.h"
#include "../interfaces/protocom.h"
#include "../module/board.h"
#include "../s2/datamanager.h"
#include "../widgets/aboutwidget.h"
#include "config.h"
#include "svcmodule.h"
ServiceComa::ServiceComa(QWidget *parent) : Coma(parent)
{
}

ServiceComa::~ServiceComa()
{
}

void ServiceComa::PrepareDialogs()
{
    m_Module = ModulePointer(new SvcModule);
    Coma::PrepareDialogs();
}

void ServiceComa::getAbout()
{
    AboutWidget *w = new AboutWidget;
    GitVersion version;
    w->appendLine("Config version: " + version.getConfigVersion());
    w->prependLine(QString(PROGNAME) + " version " + QString(COMAVERSION) + "-" + version.getGitHash());
    w->setupUI();
    w->show();
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
