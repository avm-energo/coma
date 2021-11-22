#include "module.h"

#include "../comaversion/comaversion.h"
#include "../ctti/type_id.hpp"
#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../gen/board.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/xmlparser.h"
#include "../widgets/udialog.h"
//#define XML_DEBUG

#include <QDir>
#include <QtXml>

constexpr char versionFile[] = "coma.ver";

Module::Module(QObject *parent)
    : QObject(parent)
#ifdef USE_INTERNAL_RCS
    , m_directory(resourceDirectory)
#else
    , m_directory(StdFunc::GetSystemHomeDir())
#endif
{
    m_gsettings = { { &DataTypes::DataRecV::map, &WidgetFactory::m_widgetMap, &WidgetFactory::m_categoryMap }, {} };
}
void Module::createAlarm(AlarmWidget *aw)
{
    using namespace Modules;

    if (m_settings)
        BaseInterface::iface()->setSettings(settings()->ifaceSettings);

    Q_ASSERT(aw->count() == 0);
    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    alarmStateAll->setupUI(AVM::HthToolTip);
    aw->addAlarm(alarmStateAll);
    if (m_settings)
    {
        if (settings()->alarms.contains((AlarmType::Warning)))
        {
            auto *warnAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());
            aw->addAlarm(warnAlarm, tr("Предупредительная сигнализация"));
        }
        if (settings()->alarms.contains((AlarmType::Critical)))
        {
            auto *critAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical), settings()->alarmCount());
            aw->addAlarm(critAlarm, tr("Аварийная сигнализация"));
        }
    }
}

QList<UDialog *> Module::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

QList<UDialog *> Module::confDialogs()
{
    QList<UDialog *> list;
    for (UDialog *dlg : qAsConst(m_dialogs))
    {
        if (dlg->objectName().contains("conf"))
            list.append(dlg);
    }
    return list;
}

void Module::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void Module::insertDialogToList(UDialog *dlg, int pos, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.insert(pos, dlg);
}

void Module::parentTWTabChanged(int index)
{
    if (index == -1 || m_dialogs.isEmpty())
        return;

    for (auto &dialog : m_dialogs)
    {
        if (dialog->updatesEnabled())
            dialog->setUpdatesDisabled();
    }

    UDialog *udlg = m_dialogs.at(index);

    udlg->setEnabled(true);
    udlg->setUpdatesEnabled();
    udlg->reqUpdate();
}

void Module::closeDialogs()
{
    while (!m_dialogs.isEmpty())
        m_dialogs.takeFirst()->close();
}

ModuleSettings *Module::settings() const
{
    return m_settings.get();
}

bool Module::loadSettings(const Modules::StartupInfoBlock &startupInfoBlock, int interfaceType)
{
    if (!loadS2Settings())
        return false;
    m_gsettings.check = loadCheckSettings();

    m_settings = std::unique_ptr<ModuleSettings>(new ModuleSettings(startupInfoBlock));
    m_settings->interfaceType = interfaceType;
    if (Board::isUSIO(Modules::BaseBoard(startupInfoBlock.MTypeB), Modules::MezzanineBoard(startupInfoBlock.MTypeM)))
    {
        if (!loadUsioSettings(startupInfoBlock))
            return false;
    }
    auto moduleName = QString::number(startupInfoBlock.type(), 16);

    if (!obtainXmlFile(moduleName))
        return false;

    QDir dir(m_directory);
    auto xmlFiles = dir.entryList(QDir::Files).filter(".xml");
    QDomDocument domDoc;
    QFile file;
    auto result = std::find_if(xmlFiles.cbegin(), xmlFiles.cend(),
        [&module = moduleName](const QString &str) { return str.contains(module, Qt::CaseInsensitive) && !str.contains("config", Qt::CaseInsensitive); });
    if (result != std::cend(xmlFiles))
        file.setFileName(dir.filePath(*result));
    else
        return false;

    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Error::FileOpenError << file.fileName();
        return false;
    }
    if (domDoc.setContent(&file))
    {

        QDomElement domElement = domDoc.documentElement();

        XmlParser::traverseNode(domElement, m_settings.get(), m_gsettings.config);
        file.close();
        return true;
    }
    else
    {
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
}

bool Module::obtainXmlFile(const QString &filename) const
{
    QDir dir(m_directory);
    qDebug() << dir;
    auto xmlFiles = dir.entryList(QDir::Files).filter(".xml");
    qDebug() << xmlFiles;
    QDomDocument domDoc;
    QFile file;
    auto result = std::find_if(xmlFiles.cbegin(), xmlFiles.cend(),
        [&filename](const QString &str) { return str.contains(filename, Qt::CaseInsensitive); });
    if (result != std::cend(xmlFiles))
        file.setFileName(dir.filePath(*result));

    if (file.fileName().isEmpty())
    {
        dir = QDir(resourceDirectory);
        xmlFiles = dir.entryList(QDir::Files).filter(".xml");
        qDebug() << xmlFiles;
        for (const auto &xmlFile : qAsConst(xmlFiles))
        {
            if (!xmlFile.contains(filename, Qt::CaseInsensitive))
                continue;
            if (!QFile::copy(dir.filePath(xmlFile), StdFunc::GetSystemHomeDir() + xmlFile))
            {
                qCritical() << Error::DescError;
                qInfo() << dir.filePath(xmlFile) << StdFunc::GetSystemHomeDir() + xmlFile;
                return false;
            }

            return obtainXmlFile(filename);
        }
    }
    return true;
}

quint64 Module::configVersion() const
{
    QDir directory(StdFunc::GetSystemHomeDir());
    QFile file;
    file.setFileName(directory.filePath(versionFile));
    if (!file.open(QIODevice::ReadOnly))
        return 0;
    bool status = false;
    quint64 counter = file.readAll().toULongLong(&status);
    if (!status)
        return 0;
    return counter;
}

bool Module::isConfigOutdated() const
{
    GitVersion version;
    return configVersion() < version.getGitCounter();
}

void Module::eraseSettings() const
{
    QDir directory(StdFunc::GetSystemHomeDir());
    auto files = directory.entryList(QDir::Files).filter(".xml");

    files.push_back(versionFile);
    // qDebug() << files;
    for (const auto &filename : files)
    {
        QFile file(directory.filePath(filename));
        file.setPermissions(QFile::ReadOther | QFile::WriteOther);
        if (!file.remove())
            qDebug() << file;
    }
}

void Module::putConfigVersion() const
{
    QDir directory(StdFunc::GetSystemHomeDir());
    QFile file;
    file.setFileName(directory.filePath(versionFile));
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << Error::FileWriteError << versionFile;
        return;
    }
    QTextStream out(&file);
    GitVersion version;
    out << version.getGitCounter();
    file.close();
}

const QString &Module::directory() const
{
    return m_directory;
}

void Module::setDirectory(const QString &newDirectory)
{
    m_directory = newDirectory;
}

void Module::create(UniquePointer<Journals> jour)
{
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        //     addDialogToList(new AbstractConfDialog(settings()->defaultConfig), "Конфигурирование", "conf1");
        Q_ASSERT(jour != nullptr);
        addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }
}

void Module::createCommon()
{
    const auto &board = Board::GetInstance();
    //    TimeDialog *tdlg = new TimeDialog;
    //    addDialogToList(tdlg, "Время", "time");

    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");
}

// #### C O N F I G ####

bool Module::obtainXmlConfig(const QString &filename, QList<DataTypes::RecordPair> &config) const
{
    QDir dir(m_directory);
    auto xmlFiles = dir.entryList(QDir::Files).filter(".xml");
    QDomDocument domDoc;
    QFile file;
    for (const auto &xmlFile : xmlFiles)
    {
        if (xmlFile.contains(filename, Qt::CaseInsensitive))
        {
            file.setFileName(dir.filePath(xmlFile));
            break;
        }
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Error::FileOpenError << file.fileName();
        return false;
    }
    if (domDoc.setContent(&file))
    {

        QDomElement domElement = domDoc.documentElement();
        XmlParser::traverseNodeS2(domElement, config, m_gsettings.config.s2widgetMap);
        file.close();
        return true;
    }
    else
    {
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
}

bool Module::loadUsioSettings(const Modules::StartupInfoBlock &startupInfoBlock)
{
    QString configGeneral("config-general");
    if (!obtainXmlFile(configGeneral))
        return false;
    if (!obtainXmlConfig(configGeneral, m_settings->configSettings.general))
        return false;
    QString configBase("config-%100");
    configBase = configBase.arg(startupInfoBlock.MTypeB, 0, 16);
    if (!obtainXmlFile(configBase))
        qWarning() << Error::OpenError << configBase;
    else if (!obtainXmlConfig(configBase, m_settings->configSettings.base))
        qWarning() << Error::OpenError << configBase;
    // if avtuk-3131/3535 ignore config for mezz board
    if (startupInfoBlock.MTypeB != startupInfoBlock.MTypeM)
    {
        QString configMezz("config-00%1");
        configMezz = configMezz.arg(startupInfoBlock.MTypeM, 0, 16);
        if (!obtainXmlFile(configMezz))
            qWarning() << Error::OpenError << configMezz;
        else if (!obtainXmlConfig(configMezz, m_settings->configSettings.mezz))
            qWarning() << Error::OpenError << configMezz;
    }
    return true;
}

bool Module::loadS2Settings()
{
    constexpr auto name = "s2files.xml";
    QDir dir(m_directory);
    qDebug() << dir;
    QDomDocument domDoc;
    QFile file;

    file.setFileName(dir.filePath(name));
    if (!file.exists())
    {
        dir = QDir(resourceDirectory);
        if (!QFile::copy(dir.filePath(name), StdFunc::GetSystemHomeDir() + name))
        {
            qCritical() << Error::DescError;
            qInfo() << dir.filePath(name) << StdFunc::GetSystemHomeDir() + name;
            return false;
        }
        return loadS2Settings();
    }
    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            XmlParser::traverseNode(domElement, m_gsettings.config);
            file.close();
            return true;
        }
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
    else
    {
        qCritical() << Error::FileOpenError << file.fileName();
        return false;
    }
}

// #### C H E C K ####

bool Module::obtainXmlCheck(const QString &filename, std::vector<CheckItem> &check) const
{
    QDir dir(m_directory);
    auto xmlFiles = dir.entryList(QDir::Files).filter(".xml");
    QDomDocument domDoc;
    QFile file;
    for (const auto &xmlFile : xmlFiles)
    {
        if (xmlFile.contains(filename, Qt::CaseInsensitive))
        {
            file.setFileName(dir.filePath(xmlFile));
            break;
        }
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Error::FileOpenError << file.fileName();
        return false;
    }
    if (domDoc.setContent(&file))
    {

        QDomElement domElement = domDoc.documentElement();
        XmlParser::traverseNodeCheck(domElement, check);
        file.close();
        return true;
    }
    else
    {
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
}

CheckSettings Module::loadCheckSettings()
{
    constexpr auto name = "check.xml";
    QDir dir(m_directory);
    qDebug() << dir;
    QDomDocument domDoc;
    QFile file;

    file.setFileName(dir.filePath(name));
    if (!file.exists())
    {
        dir = QDir(resourceDirectory);
        if (!QFile::copy(dir.filePath(name), StdFunc::GetSystemHomeDir() + name))
        {
            qCritical() << Error::DescError;
            qInfo() << dir.filePath(name) << StdFunc::GetSystemHomeDir() + name;
            assert(false);
        }
        return loadCheckSettings();
    }

    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            const auto &board = Board::GetInstance();
            CheckSettings checkSettings;
            checkSettings.items = loadCheckSettings(
                static_cast<Modules::BaseBoard>(board.typeB()), static_cast<Modules::MezzanineBoard>(board.typeM()));
            QDomElement domElement = domDoc.documentElement();
            XmlParser::traverseNode(domElement, checkSettings.categories);
            file.close();

            return checkSettings;
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
}

std::vector<CheckItem> Module::loadCheckSettings(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) const
{
    std::vector<CheckItem> check;
    bool statusBase = true;
    QString checkBase("check-%100");
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

    bool statusMezz = true;
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
    assert(statusBase || statusMezz);
    return check;
}
