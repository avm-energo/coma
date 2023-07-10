#include "module.h"

#include "../s2/s2configstorage.h"
#include "../xml/xmlparser/xmlmoduleparser.h"
#include "../xml/xmlparser/xmls2parser.h"
#include "filehelper.h"

#include <QDir>
#include <QFile>
#include <gen/error.h>
#include <gen/stdfunc.h>

Module::Module(const bool criticalCheck, const Modules::StartupInfoBlock &startupInfoBlock, QObject *parent)
    : QObject(parent)
    , checks(criticalCheck)
    , state(true)
    , sInfoBlock(startupInfoBlock)
    , mStorage(&ConfigStorage::GetInstance())
{
}

/// \brief Загрузка настроек в ConfigStorage из файла/файлов настроек модуля и s2files.xml.
bool Module::loadSettings()
{
    // Парсинг s2files.xml
    //    loadS2Settings();
    //    if (!state)
    //        return state;
    // Парсинг файла модуля
    auto moduleName = QString::number(sInfoBlock.type(), 16) + ".xml";
    // Настройки находятся в одном файле
    if (FileHelper::isFileExist(moduleName))
        loadModuleSettings(sInfoBlock.MTypeB, sInfoBlock.MTypeM);
    // Настройки находятся в разных файлах
    else
    {
        loadModuleSettings(sInfoBlock.MTypeB, 0);
        loadModuleSettings(0, sInfoBlock.MTypeM);
    }
    return state;
}

/// \brief Загрузка настроек из файла s2files.xml с помощью S2XmlParser.
bool Module::loadS2Settings(S2::ConfigStorage *s2Storage)
{
    // Если ещё не парсили s2files.xml
    if (s2Storage->getParseStatus() == S2::ParseStatus::NotYetParsed)
    {
        auto mS2Parser = new Xml::S2Parser(this);
        QObject::connect(mS2Parser, &Xml::S2Parser::typeDataSending, //
            s2Storage, &S2::ConfigStorage::typeDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::widgetDataSending, //
            s2Storage, &S2::ConfigStorage::widgetDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::configTabDataSending, //
            s2Storage, &S2::ConfigStorage::configTabDataReceive);
        QObject::connect(mS2Parser, &Xml::S2Parser::parseError, this, &Module::parseErrorHandle);
        mS2Parser->parse();
        // Успешно распарсили s2files.xml
        // mStorage->setS2Status(true);
        s2Storage->setParseStatus(S2::ParseStatus::AlreadyParsed);
        mS2Parser->deleteLater();
    }
    return state;
}

/// \brief Загрузка настроек из файла/файлов настроек модуля с помощью ModuleXmlParser.
void Module::loadModuleSettings(const quint16 &typeB, const quint16 &typeM)
{
    auto moduleParser = new Xml::ModuleParser(typeB, typeM, checks, this);
    QObject::connect(moduleParser, &Xml::ModuleParser::startNewConfig, mStorage, &ConfigStorage::startNewConfig);
    QObject::connect(moduleParser, &Xml::ModuleParser::signalDataSending, //
        mStorage, &ConfigStorage::signalDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::tabDataSending, mStorage, &ConfigStorage::tabDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::sectionDataSending, //
        mStorage, &ConfigStorage::sectionDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::alarmDataSending, //
        mStorage, &ConfigStorage::alarmDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::workJourDataSending, //
        mStorage, &ConfigStorage::workJourDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::measJourDataSending, //
        mStorage, &ConfigStorage::measJourDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::configDataSending, //
        mStorage, &ConfigStorage::configDataReceive);
    QObject::connect(moduleParser, &Xml::ModuleParser::parseError, this, &Module::parseErrorHandle);
    QObject::connect(
        moduleParser, &Xml::ModuleParser::protocolGroupSending, mStorage, &ConfigStorage::protocolDescriptionReceived);
    QObject::connect(moduleParser, &Xml::ModuleParser::parseError, this, &Module::parseErrorHandle);
    moduleParser->parse();
    moduleParser->deleteLater();
}

void Module::parseErrorHandle(const QString errMsg)
{
    state = false;
    qCritical() << errMsg;
}
