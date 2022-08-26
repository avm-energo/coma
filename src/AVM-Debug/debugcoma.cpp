#include "debugcoma.h"

#include "../comaversion/comaversion.h"
#include "../gen/datamanager/typesproxy.h"
#include "../interfaces/emulator.h"
#include "../interfaces/protocom.h"
#include "../module/board.h"
#include "../tune/tunecheckmipdialog.h"
#include "../widgets/aboutwidget.h"
#include "../xml/xmlconfigparser.h"
#include "config.h"
#include "dbgmodule.h"

DebugComa::DebugComa(QWidget *parent) : Coma(parent)
{
}

DebugComa::~DebugComa()
{
}

void DebugComa::PrepareDialogs()
{
    m_Module = ModulePointer(new DbgModule);
    Coma::PrepareDialogs();
}

void DebugComa::getAbout()
{
    auto w = new AboutWidget(this);
    GitVersion version;
    w->appendLine("Config version: " + version.getConfigVersion());
    w->prependLine(QString(PROGNAME) + " version " + QString(COMAVERSION) + "-" + version.getGitHash());
    w->setupUI();
    w->show();
}

void DebugComa::setupConnection()
{
    auto const &board = Board::GetInstance();
    connect(proxyBS.get(), &DataTypesProxy::DataStorable, &board, &Board::update);
    BaseInterface::InterfacePointer device;
    switch (board.interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        device = BaseInterface::InterfacePointer(new Protocom());
        break;
    }
#ifdef ENABLE_EMULATOR
    case Board::InterfaceType::Emulator:
    {
        device = BaseInterface::InterfacePointer(new Emulator());
        break;
    }
#endif
    default:
        qFatal("Connection type error");
    }
    BaseInterface::setIface(std::move(device));
    Coma::setupConnection();
}

void DebugComa::checkDialog()
{
    /*    ConfigV *config = new ConfigV;
        float i2nom = 5.0;
        config->setRecordValue({ XmlConfigParser::GetIdByName("I2nom"), i2nom });
        Tune82CheckMip *dlg = new Tune82CheckMip(config, 0, Modules::MezzanineBoard::MTM_82);
        dlg->show(); */
}
