#include "newmodule.h"

#include "../gen/stdfunc.h"
#include "modulealarm.h"
//#include "../widgets/widgetfactory.h"

#include <QDir>
#include <QFile>

NewModule::NewModule(const Modules::StartupInfoBlock &startupInfoBlock, QObject *parent)
    : QObject(parent), mSettings(startupInfoBlock)
{
    // m_gsettings = { { &DataTypes::DataRecV::map, &WidgetFactory::m_widgetMap, &WidgetFactory::m_categoryMap }, {} };
}

bool NewModule::isFileExist(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    auto file = new QFile(dir.filePath(filename), this);
    auto retVal = file->exists();
    file->deleteLater();
    return retVal;
}

QDomElement NewModule::getFileContent(const QString &filename)
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
    return doc.toElement();
}

bool NewModule::loadSettings()
{
    if (loadS2Settings())
    {
        const auto &startupInfoBlock = mSettings.getStartupInfoBlock();
        auto moduleName = QString::number(startupInfoBlock.type(), 16) + ".xml";
        if (isFileExist(moduleName))
        {
            return loadMainSettings(moduleName);
        }
        else
        {
            auto baseFile = QString::number(startupInfoBlock.MTypeB, 16) + "00.xml";
            auto measFile = "00" + QString::number(startupInfoBlock.MTypeM, 16) + ".xml";
            // TODO: парсить данные в разные структуры
            auto isBaseSuccess = loadMainSettings(baseFile);
            auto isMeasSuccess = loadMainSettings(measFile);
            return (isBaseSuccess && isMeasSuccess);
        }
    }
    return false;
}

bool NewModule::loadS2Settings()
{
    constexpr auto filename = "s2files.xml";
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        // TODO: Потом закомментировать при внедрении ModuleSettings
        // XmlParser::traverseNode(content, m_gsettings.config);
        return true;
    }
    else
        return false;
}

bool NewModule::loadMainSettings(const QString &filename)
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
