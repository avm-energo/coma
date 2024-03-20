#include "xmlconfigloader.h"

#include "xmlparser/xmlmoduleparser.h"
#include "xmlparser/xmls2parser.h"

#include <device/current_device.h>

namespace Xml
{

ConfigLoader::ConfigLoader(Device::CurrentDevice *device) noexcept
    : QObject(device), m_device(device), m_parseStatus(true)
{
}

bool ConfigLoader::loadSettings() noexcept
{
    m_parseStatus = true;
    if (loadS2Data())
        if (loadDeviceData())
            m_device->internalProtocolUpdate();
    return m_parseStatus;
}

bool ConfigLoader::loadS2Data() noexcept
{
    // Если ещё не парсили s2files.xml
    auto &s2storage = m_device->getS2Datamanager()->getStorage();
    if (s2storage.getParseStatus() == S2::ParseStatus::NotYetParsed)
    {
        auto mS2Parser = new Xml::S2Parser(this);
        QObject::connect(mS2Parser, &Xml::S2Parser::nameDataSending, //
            &s2storage, &S2::ConfigStorage::nameDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::typeDataSending, //
            &s2storage, &S2::ConfigStorage::typeDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::widgetDataSending, //
            &s2storage, &S2::ConfigStorage::widgetDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::configTabDataSending, //
            &s2storage, &S2::ConfigStorage::configTabDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::parseError, this, &Xml::ConfigLoader::parseErrorHandle);
        // Парсинг s2files.xml
        mS2Parser->parse();
        // Успешно распарсили s2files.xml
        s2storage.setParseStatus(S2::ParseStatus::AlreadyParsed);
        mS2Parser->deleteLater();
    }
    return m_parseStatus;
}

bool ConfigLoader::loadDeviceData() noexcept
{
    auto s2manager = m_device->getS2Datamanager();
    auto cfgStorage = m_device->getConfigStorage();
    auto moduleParser = new Xml::ModuleParser(this);
    QObject::connect(moduleParser, &Xml::ModuleParser::startNewConfig,        //
        s2manager, &S2::DataManager::startNewConfig);                         //
    QObject::connect(moduleParser, &Xml::ModuleParser::configDataSending,     //
        s2manager, &S2::DataManager::configDataReceive);                      //
    QObject::connect(moduleParser, &Xml::ModuleParser::configNameSending,     //
        s2manager, &S2::DataManager::configNameReceive);                      //
    QObject::connect(moduleParser, &Xml::ModuleParser::signalDataSending,     //
        cfgStorage, &Device::ConfigStorage::signalDataReceive);               //
    QObject::connect(moduleParser, &Xml::ModuleParser::tabDataSending,        //
        cfgStorage, &Device::ConfigStorage::tabDataReceive);                  //
    QObject::connect(moduleParser, &Xml::ModuleParser::sectionDataSending,    //
        cfgStorage, &Device::ConfigStorage::sectionDataReceive);              //
    QObject::connect(moduleParser, &Xml::ModuleParser::alarmDataSending,      //
        cfgStorage, &Device::ConfigStorage::alarmDataReceive);                //
    QObject::connect(moduleParser, &Xml::ModuleParser::workJourDataSending,   //
        cfgStorage, &Device::ConfigStorage::workJourDataReceive);             //
    QObject::connect(moduleParser, &Xml::ModuleParser::measJourDataSending,   //
        cfgStorage, &Device::ConfigStorage::measJourDataReceive);             //
    QObject::connect(moduleParser, &Xml::ModuleParser::protocolGroupSending,  //
        cfgStorage, &Device::ConfigStorage::protocolGroupReceive);            //
    QObject::connect(moduleParser, &Xml::ModuleParser::hiddenTabDataSending,  //
        cfgStorage, &Device::ConfigStorage::hiddenTabDataReceive);            //
    QObject::connect(moduleParser, &Xml::ModuleParser::bsiExtItemDataSending, //
        cfgStorage, &Device::ConfigStorage::bsiExtItemDataReceive);           //
    QObject::connect(moduleParser, &Xml::ModuleParser::parseError,            //
        this, &Xml::ConfigLoader::parseErrorHandle);                          //
    moduleParser->parse(m_device);
    moduleParser->deleteLater();
    return m_parseStatus;
}

void ConfigLoader::parseErrorHandle(const QString errMsg)
{
    m_parseStatus = false;
    qCritical() << errMsg;
}

} // namespace Xml
