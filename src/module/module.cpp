#include "module.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../xml/modulexmlparser.h"
#include "../xml/s2xmlparser.h"

#include <QDir>
#include <QFile>

Module::Module(const Modules::StartupInfoBlock &startupInfoBlock, QObject *parent)
    : QObject(parent), sInfoBlock(startupInfoBlock), mStorage(&ConfigStorage::GetInstance())
{
}

bool Module::isFileExist(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto file = new QFile(dir.filePath(filename), this);
    auto retVal = file->exists();
    file->deleteLater();
    return retVal;
}

QDomDocument Module::getFileContent(const QString &filename)
{
    QDomDocument doc;
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto file = new QFile(dir.filePath(filename), this);
    if (file->exists())
    {
        if (file->open(QIODevice::ReadOnly))
        {
            QString errMsg = "";
            auto line = 0, column = 0;
            if (!doc.setContent(file, &errMsg, &line, &column))
            {
                qCritical() << Error::WrongFileError << file->fileName();
                qCritical() << errMsg << " Line: " << line << " Column: " << column;
            }
            file->close();
        }
        else
            qCritical() << Error::FileOpenError << file->fileName();
    }
    else
        qCritical() << Error::DescError << file->fileName();

    file->deleteLater();
    return doc;
}

bool Module::loadSettings()
{
    if (loadS2Settings())
    {
        auto moduleName = QString::number(sInfoBlock.type(), 16) + ".xml";
        if (isFileExist(moduleName))
        {
            return loadModuleSettings(moduleName, sInfoBlock.MTypeB, sInfoBlock.MTypeM);
        }
        else
        {
            auto baseFile = QString::number(sInfoBlock.MTypeB, 16) + "00.xml";
            auto mezzFile = "00" + QString::number(sInfoBlock.MTypeM, 16) + ".xml";
            // TODO: парсить данные в разные структуры
            auto isBaseSuccess = loadModuleSettings(baseFile, sInfoBlock.MTypeB, 0);
            auto isMezzSuccess = loadModuleSettings(mezzFile, 0, sInfoBlock.MTypeM);
            return (isBaseSuccess && isMezzSuccess);
        }
    }
    return false;
}

bool Module::loadS2Settings()
{
    // Если ещё не парсили s2files.xml
    if (!mStorage->getS2Status())
    {
        constexpr auto filename = "s2files.xml";
        auto content = getFileContent(filename);
        if (!content.isNull())
        {
            auto mS2Parser = new S2XmlParser(this);
            QObject::connect(mS2Parser, &S2XmlParser::typeDataSending, mStorage, &ConfigStorage::typeDataReceive);
            QObject::connect(mS2Parser, &S2XmlParser::widgetDataSending, mStorage, &ConfigStorage::widgetDataReceive);
            mS2Parser->parse(content);
            // Успешно распарсили s2files.xml
            mStorage->setS2Status(true);
            mS2Parser->deleteLater();
            return true;
        }
        else
            return false;
    }
    else
        return true;
}

bool Module::loadModuleSettings(const QString &filename, const quint16 &typeB, const quint16 &typeM)
{
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        auto moduleParser = new ModuleXmlParser(this);
        QObject::connect(moduleParser, &ModuleXmlParser::startNewConfig, mStorage, &ConfigStorage::startNewConfig);
        QObject::connect(moduleParser, &ModuleXmlParser::signalDataSending, //
            mStorage, &ConfigStorage::signalDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::tabDataSending, mStorage, &ConfigStorage::tabDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::sectionDataSending, //
            mStorage, &ConfigStorage::sectionDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::alarmDataSending, mStorage, &ConfigStorage::alarmDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::jourDataSending, mStorage, &ConfigStorage::jourDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::modbusDataSending, //
            mStorage, &ConfigStorage::modbusDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::protocomDataSending, //
            mStorage, &ConfigStorage::protocomDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::iecDataSending, mStorage, &ConfigStorage::iecDataReceive);
        QObject::connect(moduleParser, &ModuleXmlParser::configDataSending, //
            mStorage, &ConfigStorage::configDataReceive);
        moduleParser->parse(content, typeB, typeM);
        moduleParser->deleteLater();
        return true;
    }
    else
        return false;
}
