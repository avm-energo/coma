#include "newmodule.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "s2xmlparser.h"

#include <QDir>
#include <QFile>

constexpr auto debug = true;

NewModule::NewModule(const Modules::StartupInfoBlock &startupInfoBlock, QObject *parent)
    : QObject(parent), mStorage(&ConfigStorage::GetInstance())
{
    mStorage->initSettings(startupInfoBlock);
}

bool NewModule::isFileExist(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto file = new QFile(dir.filePath(filename), this);
    auto retVal = file->exists();
    file->deleteLater();
    return retVal;
}

QDomDocument NewModule::getFileContent(const QString &filename)
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

bool NewModule::loadSettings()
{
    if (loadS2Settings())
    {
        const auto &startupInfoBlock = mStorage->getStartupInfoBlock();
        auto moduleName = QString::number(startupInfoBlock.type(), 16) + ".xml";
        if (isFileExist(moduleName))
        {
            return loadModuleSettings(moduleName);
        }
        else
        {
            auto baseFile = QString::number(startupInfoBlock.MTypeB, 16) + "00.xml";
            auto measFile = "00" + QString::number(startupInfoBlock.MTypeM, 16) + ".xml";
            // TODO: парсить данные в разные структуры
            auto isBaseSuccess = loadModuleSettings(baseFile);
            auto isMeasSuccess = loadModuleSettings(measFile);
            if (debug)
            {
                qCritical() << isBaseSuccess << " " << isMeasSuccess;
            }
            return (isBaseSuccess && isMeasSuccess);
        }
    }
    return false;
}

bool NewModule::loadS2Settings()
{
    // Если ещё не парсили s2files.xml
    if (!mStorage->getS2Status())
    {
        constexpr auto filename = "s2files.xml";
        auto content = getFileContent(filename);
        if (!content.isNull())
        {
            auto mS2Parser = new S2XmlParser(this);
            auto conn1 = QObject::connect(mS2Parser, &S2XmlParser::typeDataSending,   //
                mStorage, &ConfigStorage::typeDataReceive);                           //
            auto conn2 = QObject::connect(mS2Parser, &S2XmlParser::widgetDataSending, //
                mStorage, &ConfigStorage::widgetDataReceive);                         //
            mS2Parser->parse(content);
            // Успешно распарсили s2files.xml
            mStorage->setS2Status(true);
            mS2Parser->deleteLater();
            QObject::disconnect(conn1);
            QObject::disconnect(conn2);
            return true;
        }
        else
            return false;
    }
    else
        return true;
}

bool NewModule::loadModuleSettings(const QString &filename)
{
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        // TODO: Потом закомментировать при внедрении ModuleSettings
        // XmlParser::traverseNode(content, m_settings.get(), m_gsettings.config);
        return true;
    }
    else
        return false;
}
