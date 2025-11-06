#include "xml/xmlconfigloader.h"

#include <device/current_device.h>
#include <xml/xmlparser/xmlmoduleparser.h>
#include <xml/xmlparser/xmls2parser.h>

namespace Xml
{

ConfigLoader::ConfigLoader(Device::CurrentDevice *device) noexcept
    : QObject(device)
    , m_device(device)
    , m_parseStatus(true)
{
}

bool ConfigLoader::loadSettings() noexcept
{
    m_parseStatus = true;
    loadS2Data();
    if (loadDeviceData())
        m_device->internalProtocolUpdate();
    return m_parseStatus;
}

void ConfigLoader::loadS2Data() noexcept
{
    // Если ещё не парсили s2files.xml
    auto &s2storage = m_device->getS2Datamanager()->getStorage();
    auto mXmlParser = new Xml::XmlS2Parser(this);
    QObject::connect(mXmlParser, &Xml::XmlParser::nameDataSending,      //
        &s2storage, &S2::ConfigStorage::nameDataReceive);
    QObject::connect(mXmlParser, &Xml::XmlParser::typeDataSending,      //
        &s2storage, &S2::ConfigStorage::typeDataReceive);
    QObject::connect(mXmlParser, &Xml::XmlParser::dtypeDataSending,     //
        &s2storage, &S2::ConfigStorage::dtypeDataReceive);
    QObject::connect(mXmlParser, &Xml::XmlParser::widgetDataSending,    //
        &s2storage, &S2::ConfigStorage::widgetDataReceive);
    QObject::connect(mXmlParser, &Xml::XmlParser::configTabDataSending, //
        &s2storage, &S2::ConfigStorage::configTabDataReceive);
    // Парсинг s2files.xml
    mXmlParser->parse();
    // Успешно распарсили s2files.xml
    mXmlParser->deleteLater();
}

bool ConfigLoader::loadDeviceData() noexcept
{
    auto &s2storage = m_device->getS2Datamanager()->getStorage();
    auto s2manager = m_device->getS2Datamanager();
    auto cfgStorage = m_device->getConfigStorage();
    auto moduleParser = new Xml::ModuleParser(this);
    QObject::connect(moduleParser, &Xml::ModuleParser::startNewConfig,           //
        s2manager, &S2::DataManager::startNewConfig);                            //
    QObject::connect(moduleParser, &Xml::ModuleParser::configDataSending,        //
        s2manager, &S2::DataManager::configDataReceive);                         //
    QObject::connect(moduleParser, &Xml::ModuleParser::configNameSending,        //
        s2manager, &S2::DataManager::configNameReceive);                         //
    QObject::connect(moduleParser, &Xml::ModuleParser::newFeatureParsedSignal,   //
        cfgStorage, &Device::ConfigStorage::featureReceive);                     //
    QObject::connect(moduleParser, &Xml::ModuleParser::signalDataSending,        //
        cfgStorage, &Device::ConfigStorage::signalDataReceive);                  //
    QObject::connect(moduleParser, &Xml::ModuleParser::tabDataSending,           //
        cfgStorage, &Device::ConfigStorage::tabDataReceive);                     //
    QObject::connect(moduleParser, &Xml::ModuleParser::sectionDataSending,       //
        cfgStorage, &Device::ConfigStorage::sectionDataReceive);                 //
    QObject::connect(moduleParser, &Xml::ModuleParser::alarmStateAllDataSending, //
        cfgStorage, &Device::ConfigStorage::alarmStateAllDataReceive);           //
    QObject::connect(moduleParser, &Xml::ModuleParser::alarmDataSending,         //
        cfgStorage, &Device::ConfigStorage::alarmDataReceive);                   //
    QObject::connect(moduleParser, &Xml::ModuleParser::workJourDataSending,      //
        cfgStorage, &Device::ConfigStorage::workJourDataReceive);                //
    QObject::connect(moduleParser, &Xml::ModuleParser::measJourDataSending,      //
        cfgStorage, &Device::ConfigStorage::measJourDataReceive);                //
    QObject::connect(moduleParser, &Xml::ModuleParser::protocolGroupSending,     //
        cfgStorage, &Device::ConfigStorage::protocolGroupReceive);               //
    QObject::connect(moduleParser, &Xml::ModuleParser::hiddenTabDataSending,     //
        cfgStorage, &Device::ConfigStorage::hiddenTabDataReceive);               //
    QObject::connect(moduleParser, &Xml::ModuleParser::bsiExtItemDataSending,    //
        cfgStorage, &Device::ConfigStorage::bsiExtItemDataReceive);              //
    QObject::connect(moduleParser, &Xml::ModuleParser::parseError,               //
        this, &Xml::ConfigLoader::parseErrorHandle);                             //
    QObject::connect(moduleParser, &Xml::XmlParser::nameDataSending,             //
        &s2storage, &S2::ConfigStorage::nameDataReceive);
    QObject::connect(moduleParser, &Xml::XmlParser::typeDataSending,             //
        &s2storage, &S2::ConfigStorage::typeDataReceive);
    QObject::connect(moduleParser, &Xml::XmlParser::dtypeDataSending,            //
        &s2storage, &S2::ConfigStorage::dtypeDataReceive);
    QObject::connect(moduleParser, &Xml::XmlParser::widgetDataSending,           //
        &s2storage, &S2::ConfigStorage::widgetDataReceive);
    QObject::connect(moduleParser, &Xml::XmlParser::configTabDataSending,        //
        &s2storage, &S2::ConfigStorage::configTabDataReceive);
    moduleParser->parse(m_device);
    m_device->configFileLoadFinished();
    moduleParser->deleteLater();
    return m_parseStatus;
}

void ConfigLoader::parseErrorHandle(const QString errMsg)
{
    m_parseStatus = false;
    qDebug() << errMsg;
}

} // namespace Xml
