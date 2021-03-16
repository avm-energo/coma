#include "debugcoma.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../interfaces/protocom.h"
#include "../widgets/aboutwidget.h"
#include "config.h"
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
    Coma::PrepareDialogs();
}

void DebugComa::getAbout()
{
    AboutWidget *w = new AboutWidget;
    QFile fileHelper(":/git/version.txt");
    QString gitCommit {};
    if (fileHelper.open(QIODevice::ReadOnly))
        gitCommit = fileHelper.readAll();
    w->prependLine(QString(PROGNAME) + " version " + QString(COMAVERSION) + "-" + QString(gitCommit));
    w->setupUI();
    w->show();
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
