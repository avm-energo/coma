#include "module.h"

#include "../xml/xmlparser/xmlmoduleparser.h"
#include "../xml/xmlparser/xmls2parser.h"

#include <QDir>
#include <QFile>
#include <gen/error.h>
#include <gen/stdfunc.h>

Module::Module(const bool criticalCheck, const Modules::StartupInfoBlock &startupInfoBlock, QObject *parent)
    : QObject(parent), checks(criticalCheck), sInfoBlock(startupInfoBlock), mStorage(&ConfigStorage::GetInstance())
{
}

/// \brief Функция для проверки, существует ли файл с указанным именем в локальной папке пользователя.
bool Module::isFileExist(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    return QFile::exists(dir.filePath(filename));
}

/// \brief Возвращает QDomDocument для файла с указанным именем из локальной папки пользователя.
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

/// \brief Загрузка настроек в ConfigStorage из файла/файлов настроек модуля и s2files.xml.
bool Module::loadSettings()
{
    if (loadS2Settings())
    {
        auto moduleName = QString::number(sInfoBlock.type(), 16) + ".xml";
        // Настройки находятся в одном файле
        if (isFileExist(moduleName))
            return loadModuleSettings(moduleName, sInfoBlock.MTypeB, sInfoBlock.MTypeM);
        // Настройки находятся в разных файлах
        else
        {
            auto baseFile = QString::number(sInfoBlock.MTypeB, 16) + "00.xml";
            auto mezzFile = "00" + QString::number(sInfoBlock.MTypeM, 16) + ".xml";
            auto isBaseSuccess = loadModuleSettings(baseFile, sInfoBlock.MTypeB, 0);
            auto isMezzSuccess = loadModuleSettings(mezzFile, 0, sInfoBlock.MTypeM);
            return (isBaseSuccess && isMezzSuccess);
        }
    }
    else
        return false;
}

/// \brief Загрузка настроек из файла s2files.xml с помощью S2XmlParser.
bool Module::loadS2Settings()
{
    // Если ещё не парсили s2files.xml
    if (!mStorage->getS2Status())
    {
        constexpr auto filename = "s2files.xml";
        auto content = getFileContent(filename);
        if (!content.isNull())
        {
            auto mS2Parser = new Xml::S2Parser(this);
            QObject::connect(mS2Parser, &Xml::S2Parser::typeDataSending, mStorage, &ConfigStorage::typeDataReceive);
            QObject::connect(mS2Parser, &Xml::S2Parser::widgetDataSending, mStorage, &ConfigStorage::widgetDataReceive);
            QObject::connect(mS2Parser, &Xml::S2Parser::configTabDataSending, //
                mStorage, &ConfigStorage::configTabDataReceive);
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

/// \brief Загрузка настроек из файла/файлов настроек модуля с помощью ModuleXmlParser.
bool Module::loadModuleSettings(const QString &filename, const quint16 &typeB, const quint16 &typeM)
{
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        auto moduleParser = new Xml::ModuleParser(this);
        QObject::connect(moduleParser, &Xml::ModuleParser::startNewConfig, mStorage, &ConfigStorage::startNewConfig);
        QObject::connect(moduleParser, &Xml::ModuleParser::signalDataSending, //
            mStorage, &ConfigStorage::signalDataReceive);
        QObject::connect(moduleParser, &Xml::ModuleParser::tabDataSending, mStorage, &ConfigStorage::tabDataReceive);
        QObject::connect(moduleParser, &Xml::ModuleParser::sectionDataSending, //
            mStorage, &ConfigStorage::sectionDataReceive);
        QObject::connect(moduleParser, &Xml::ModuleParser::alarmDataSending, //
            mStorage, &ConfigStorage::alarmDataReceive);
        QObject::connect(moduleParser, &Xml::ModuleParser::jourDataSending, mStorage, &ConfigStorage::jourDataReceive);
        QObject::connect(moduleParser, &Xml::ModuleParser::interfaceSettingsSending, //
            mStorage, &ConfigStorage::interfaceSettingsReceive);
        QObject::connect(moduleParser, &Xml::ModuleParser::configDataSending, //
            mStorage, &ConfigStorage::configDataReceive);
        moduleParser->parse(content, typeB, typeM, checks);
        moduleParser->deleteLater();
        return true;
    }
    else
        return false;
}
