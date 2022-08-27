#include "newmodule.h"

//#include "../comaversion/comaversion.h"
//#include "../ctti/type_id.hpp"
//#include "../dialogs/fwuploaddialog.h"
//#include "../dialogs/infodialog.h"
//#include "../dialogs/journalsdialog.h"
//#include "../dialogs/timedialog.h"
#include "../gen/stdfunc.h"
#include "../s2/s2.h"
#include "../widgets/udialog.h"
#include "../xml/xmlparser.h"
#include "board.h"

#include <QDir>
#include <QFile>
#include <QtXml>

constexpr char resDir[] = ":/module";

NewModule::NewModule(QObject *parent) : QObject(parent)
{
    m_gsettings = { { &DataTypes::DataRecV::map, &WidgetFactory::m_widgetMap, &WidgetFactory::m_categoryMap }, {} };
}

/*
void NewModule::createAlarm(AlarmWidget *aw)
{
    using namespace Modules;

    Q_ASSERT(aw->count() == 0);
    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    alarmStateAll->setupUI(AVM::HthToolTip);
    aw->addAlarm(alarmStateAll);
    if (m_settings)
    {
        if (settings()->alarms.contains(AlarmType::Warning))
        {
            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());
            aw->addAlarm(alarm, tr("Предупредительная сигнализация"));
        }
        if (settings()->alarms.contains(AlarmType::Critical))
        {
            auto *alarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical), settings()->alarmCount());
            aw->addAlarm(alarm, tr("Аварийная сигнализация"));
        }
        if (settings()->alarms.contains(AlarmType::Base))
        {
            auto *alarm = new ModuleAlarm(
                settings()->alarms.value(AlarmType::Base), settings()->alarms.value(AlarmType::Base).desc.count());
            aw->addAlarm(alarm, tr("Базовая сигнализация"));
        }
        if (settings()->alarms.contains(AlarmType::Mezz))
        {
            auto *alarm = new ModuleAlarm(
                settings()->alarms.value(AlarmType::Mezz), settings()->alarms.value(AlarmType::Mezz).desc.count());
            aw->addAlarm(alarm, tr("Мезонинная сигнализация"));
        }
    }
}

QList<UDialog *> NewModule::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

void NewModule::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void NewModule::parentTWTabChanged(int index)
{
    if (index == -1 || m_dialogs.isEmpty())
        return;

    for (auto &dialog : m_dialogs)
    {
        //        if (dialog->updatesEnabled())
        dialog->engine()->setUpdatesEnabled(false);
    }

    UDialog *udlg = m_dialogs.at(index);

    udlg->setEnabled(true);
    udlg->engine()->setUpdatesEnabled();
    udlg->reqUpdate();
}

void NewModule::closeDialogs()
{
    while (!m_dialogs.isEmpty())
        m_dialogs.takeFirst()->close();
}

void NewModule::create(UniquePointer<Journals> jour)
{
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        Q_ASSERT(jour != nullptr);
        addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }
}

void NewModule::createCommon()
{
    const auto &board = Board::GetInstance();
    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");
}

bool NewModule::loadSettings(const Modules::StartupInfoBlock &startupInfoBlock, int interfaceType)
{
    if (!loadS2Settings())
        return false;
    if (!loadCheckSettings(m_gsettings.check))
        return false;

    m_settings = std::unique_ptr<ModuleSettingsDeprecated>(new ModuleSettingsDeprecated(startupInfoBlock));
    m_settings->interfaceType = interfaceType;
    auto mtypeb = Modules::BaseBoard(startupInfoBlock.MTypeB);
    auto mtypem = Modules::MezzanineBoard(startupInfoBlock.MTypeM);
    if (Board::isUSIO(mtypeb, mtypem))
    {
        if (!loadUsioSettings(startupInfoBlock))
            return false;
    }
    auto moduleName = "avtuk-" + QString::number(startupInfoBlock.type(), 16);

    if (!obtainXmlFile(moduleName))
        return false;

    QDir dir(resourceDirectory);
    auto xmlFiles = dir.entryList(QDir::Files).filter(".xml");

    auto result = std::find_if(xmlFiles.cbegin(), xmlFiles.cend(), [&module = moduleName](const QString &str) {
        return str.contains(module, Qt::CaseInsensitive) && str.contains("avtuk", Qt::CaseInsensitive);
    });
    // load settings by entire module
    if (result != std::cend(xmlFiles))
    {
        return loadMainSettings(dir.filePath(*result));
    }
    // load settings for every board of module
    // especially for usio modules, load directly protocom settings
    else
        switch (interfaceType)
        {
        case Board::InterfaceType::USB:
        {
            {
                QString protocomBase("protocom-%0100");
                protocomBase = protocomBase.arg(startupInfoBlock.MTypeB, 0, 16);
                if (!obtainXmlFile(protocomBase))
                    return false; // no valid protocom if no valid settings for base board
                else if (!loadMainSettings(dir.filePath(protocomBase + ".xml")))
                    qWarning() << Error::OpenError << protocomBase;
            }
            {
                QString protocomMezz("protocom-00%1");
                protocomMezz = protocomMezz.arg(startupInfoBlock.MTypeM, 0, 16);
                if (!obtainXmlFile(protocomMezz))
                    qDebug() << Error::OpenError << protocomMezz;
                else if (!loadMainSettings(dir.filePath(protocomMezz + ".xml")))
                    qWarning() << Error::OpenError << protocomMezz;
            }
            return true;
        }
        case Board::InterfaceType::Emulator:
            return true;
        }
    return false;
}

bool NewModule::loadCheckSettings(CheckSettings &settings)
{
    constexpr auto name = "check.xml";
    QDir dir(resDir);
    QDomDocument domDoc;
    QFile file;

file.setFileName(dir.filePath(name));
if (!file.exists())
{
    qCritical() << Error::DescError;
    qInfo() << dir.filePath(name) << StdFunc::GetSystemHomeDir() + name;
    assert(false);
    return false;
}

if (file.open(QIODevice::ReadOnly))
{
    if (domDoc.setContent(&file))
    {
        const auto &board = Board::GetInstance();
        auto typeb = static_cast<Modules::BaseBoard>(board.typeB());
        auto typem = static_cast<Modules::MezzanineBoard>(board.typeM());
        if (!loadCheckSettings(typeb, typem, settings.items))
            return false;
        auto domElement = domDoc.documentElement();
        XmlParser::traverseNode(domElement, settings.categories);
        file.close();
        return true;
    }
    file.close();
    qInfo() << Error::WrongFileError << file.fileName();
    assert(false);
}
else
{
    qCritical() << Error::FileOpenError << file.fileName();
    assert(false);
}
return false;
}

bool NewModule::loadCheckSettings(const Modules::BaseBoard typeB, //
    const Modules::MezzanineBoard typeM,                          //
    std::vector<CheckItem> &check) const
{
    bool statusBase = true;
    {
        QString checkBase("check-%0100");
        checkBase = checkBase.arg(typeB, 0, 16);
        if (!obtainXmlFile(checkBase))
        {
            statusBase = false;
            qWarning() << Error::OpenError << checkBase;
        }
        else if (!obtainXmlCheck(checkBase, check))
        {
            statusBase = false;
            qWarning() << Error::OpenError << checkBase;
        }
    }
    bool statusMezz = true;
    {
        QString checkMezz("check-00%1");
        checkMezz = checkMezz.arg(typeM, 0, 16);
        if (!obtainXmlFile(checkMezz))
        {
            statusMezz = false;
            qWarning() << Error::OpenError << checkMezz;
        }
        else if (!obtainXmlCheck(checkMezz, check))
        {
            statusMezz = false;
            qWarning() << Error::OpenError << checkMezz;
        }
    }
    bool isCheckByBoard = statusBase || statusMezz;
    if (!isCheckByBoard)
    {
        QString checkModule("check-");
        checkModule.append(QString::number(quint8(typeB), 16));
        checkModule.append(QString::number(quint8(typeM), 16));
        if (!obtainXmlFile(checkModule))
        {
            qWarning() << Error::OpenError << checkModule;
            return false;
        }
        else if (!obtainXmlCheck(checkModule, check))
        {
            qWarning() << Error::OpenError << checkModule;
            return false;
        }
    }
    return true;
}

bool NewModule::loadUsioSettings(const Modules::StartupInfoBlock &startupInfoBlock)
{
    {
        QString configGeneral("config-general");
        if (!obtainXmlFile(configGeneral))
            return false;
        if (!obtainXmlConfig(configGeneral, m_settings->configSettings.general))
            return false;
    }
    {
        QString configBase("config-%0100");
        configBase = configBase.arg(startupInfoBlock.MTypeB, 0, 16);
        if (!obtainXmlFile(configBase))
            qDebug() << Error::OpenError << configBase;
        else if (!obtainXmlConfig(configBase, m_settings->configSettings.base))
            qWarning() << Error::OpenError << configBase;
    }
    // if avtuk-3131/3535 ignore config for mezz board
    if (startupInfoBlock.MTypeB != startupInfoBlock.MTypeM)
    {
        QString configMezz("config-00%1");
        configMezz = configMezz.arg(startupInfoBlock.MTypeM, 0, 16);
        if (!obtainXmlFile(configMezz))
            qDebug() << Error::OpenError << configMezz;
        else if (!obtainXmlConfig(configMezz, m_settings->configSettings.mezz))
            qWarning() << Error::OpenError << configMezz;
    }
    {
        QString alarmBase("alarm-%0100");
        alarmBase = alarmBase.arg(startupInfoBlock.MTypeB, 0, 16);
        if (!obtainXmlFile(alarmBase))
            qDebug() << Error::OpenError << alarmBase;
        else if (!obtainXmlAlarm(alarmBase, m_settings->alarms, Modules::AlarmType::Base))
            qWarning() << Error::OpenError << alarmBase;
    }
    {
        QString alarmMezz("alarm-00%1");
        alarmMezz = alarmMezz.arg(startupInfoBlock.MTypeM, 0, 16);
        if (!obtainXmlFile(alarmMezz))
            qDebug() << Error::OpenError << alarmMezz;
        else if (!obtainXmlAlarm(alarmMezz, m_settings->alarms, Modules::AlarmType::Mezz))
            qWarning() << Error::OpenError << alarmMezz;
    }
    return false;
}

bool NewModule::obtainXmlFile(const QString &filename)
{
    QDir dir(resDir);
    QStringList xmlFiles = dir.entryList(QDir::Files).filter(".xml");
    qDebug() << xmlFiles;
    for (const auto &xmlFile : qAsConst(xmlFiles))
    {
        if (xmlFile.contains(filename, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

bool NewModule::obtainXmlConfig(const QString &filename, QList<DataTypes::RecordPair> &config)
{
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        XmlParser::traverseNodeS2(content, config, m_gsettings.config.s2widgetMap);
        return true;
    }
    else
        return false;
}

bool NewModule::obtainXmlCheck(const QString &filename, std::vector<CheckItem> &check)
{
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        XmlParser::traverseNodeCheck(content, check);
        return true;
    }
    else
        return false;
}

bool NewModule::obtainXmlAlarm(const QString &filename, AlarmMap &alarmMap, Modules::AlarmType type)
{
    auto content = getFileContent(filename);
    if (!content.isNull())
    {
        alarmMap.insert(type, XmlParser::traverseNodeAlarm(content));
        return true;
    }
    else
        return false;
}
*/

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

ModuleSettingsDeprecated *NewModule::settings() const
{
    return m_settings.get();
}

bool NewModule::loadSettings(const Modules::StartupInfoBlock &startupInfoBlock, int interfaceType)
{
    if (loadS2Settings())
    {
        m_settings = std::unique_ptr<ModuleSettingsDeprecated>(new ModuleSettingsDeprecated(startupInfoBlock));
        m_settings->interfaceType = interfaceType;
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
            return (loadMainSettings(baseFile) && loadMainSettings(measFile));
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
        XmlParser::traverseNode(content, m_gsettings.config);
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
        try
        {
            // TODO: Потом закомментировать при внедрении ModuleSettings
            XmlParser::traverseNode(content, m_settings.get(), m_gsettings.config);
            return true;
        } catch (const std::exception &e)
        {
            qCritical() << e.what();
        }
    }
    return false;
}
