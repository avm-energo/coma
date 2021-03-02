#include "module.h"

#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../gen/board.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../widgets/udialog.h"
//#define XML_DEBUG

#include <QDir>
#include <QtXml>

Module::Module(QObject *parent) : QObject(parent)
{
}
void Module::create(AlarmWidget *aw)
{
    using namespace Modules;

    BaseInterface::iface()->setSettings(settings()->ifaceSettings);

    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    Q_ASSERT(aw->count() == 0);
    aw->addAlarm(alarmStateAll);

    auto *warnAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());

    aw->addAlarm(warnAlarm, tr("Предупредительная сигнализация"));
    auto *critAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical), settings()->alarmCount());
    aw->addAlarm(critAlarm, tr("Аварийная сигнализация"));
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

    S2::config.clear();
}

ModuleSettings *Module::settings() const
{
    return m_settings.get();
}

quint32 Module::parseInt32(QDomElement domElement) const
{
#ifdef XML_DEBUG
    qDebug() << domElement.attribute("name", "") << domElement.text();
#endif
    if (domElement.text().isEmpty())
        return 0;
    bool ok;
    const quint32 number = domElement.text().toUInt(&ok);
    Q_ASSERT(ok);
    return number;
}

quint32 Module::parseHexInt32(QDomElement domElement) const
{
    auto str = domElement.text();
#ifdef XML_DEBUG
    qDebug() << domElement.attribute("name", "") << domElement.text();
#endif
    if (domElement.text().isEmpty())
        return 0;
    Q_ASSERT(str.startsWith("0x"));
    str.remove(0, 2);
    bool ok;
    const quint32 number = domElement.text().toUInt(&ok, 16);
    Q_ASSERT(ok);
    return number;
}

QStringList Module::parseStringList(QDomElement domElement) const
{
    const auto &nodes = domElement.childNodes();
    QStringList description;
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    while (i != nodes.count())
    {
        description.push_back(nodes.item(i++).toElement().text());
    }
    return description;
}

DataTypes::Alarm Module::parseAlarm(QDomElement domElement)
{
    DataTypes::Alarm alarm;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    alarm.name = domElement.attribute("name", "");
    auto element = domElement.firstChildElement("string-array");
    alarm.desc = parseStringList(element);
    element = domElement.firstChildElement("color");
    alarm.color = element.isNull() ? "" : element.text();
    element = domElement.firstChildElement("quint32");
    while (!element.isNull())
    {
        const auto name = element.attribute("name", "");
        if (name.contains("flags", Qt::CaseInsensitive))
            alarm.flags = parseHexInt32(element);
        if (name.contains("addr", Qt::CaseInsensitive))
            alarm.startAddr = parseInt32(element);
        element = element.nextSiblingElement("quint32");
    }

    return alarm;
}

DataTypes::Journal Module::parseJournal(QDomElement domElement)
{
    DataTypes::Journal journal;
#ifdef XML_DEBUG
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
#endif
    journal.name = domElement.attribute("name", "");
    journal.id = parseInt32(domElement.firstChildElement("quint32"));
    domElement = domElement.firstChildElement("string-array");
    while (!domElement.isNull())
    {
        const auto name = domElement.attribute("name", "");
        if (name.contains("description"), Qt::CaseInsensitive)
            journal.desc = parseStringList(domElement);
        if (name.contains("header"), Qt::CaseInsensitive)
            journal.header = parseStringList(domElement);
        domElement = domElement.nextSiblingElement("string-array");
    }
    return journal;
}

bool isCorrectModule(const QString &typem, const QString &typeb)
{
    const auto &board = Board::GetInstance();
    quint16 mtypem = typem.toUInt(nullptr, 16);
    quint16 mtypeb = typeb.toUInt(nullptr, 16);
#ifdef XML_DEBUG
    qDebug() << typem << mtypem;
    qDebug() << typem << mtypeb;
#endif
    if (board.typeB() != mtypeb)
        return false;
    if (board.typeM() != mtypem)
        return false;
    return true;
}

void Module::traverseNode(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull())
    {
        if (domNode.isElement())
        {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull())
            {
                if (domElement.tagName() == "quint32")
                {
                    //      qDebug() << "Attr: " << domElement.attribute("name", "")
                    //               << "\tValue: " << /*qPrintable*/ (domElement.text());
                    parseInt32(domElement);
                    // domNode = domNode.nextSibling();
                    // break;
                }
                if (domElement.tagName() == "string-array")
                {
#ifdef XML_DEBUG
                    qDebug() << "Attr: " << domElement.attribute("name", "");
#endif
                    parseStringList(domElement);
                    domNode = domNode.nextSibling();
                    continue;
                    // domNode = domNode.nextSibling();
                }
                if (domElement.tagName() == "alarm")
                {

                    qDebug() << "Attr: " << domElement.attribute("name", "");
                    const auto alarm = parseAlarm(domElement);
                    if (alarm.name.contains("critical", Qt::CaseInsensitive))
                        m_settings->alarms.insert(AlarmType::Critical, alarm);
                    else if (alarm.name.contains("warning", Qt::CaseInsensitive))
                        m_settings->alarms.insert(AlarmType::Warning, alarm);
                    else
                        m_settings->alarms.insert(AlarmType::All, alarm);
                    //  m_settings->alarms.push_back(parseAlarm(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                    // domNode = domNode.nextSibling();
                }
                if (domElement.tagName() == "journal")
                {
#ifdef XML_DEBUG
                    qDebug() << "Attr: " << domElement.attribute("name", "");
#endif
                    const auto journal = parseJournal(domElement);
                    if (journal.name.contains("work", Qt::CaseInsensitive))
                        m_settings->journals.insert(JournalType::Work, journal);
                    else if (journal.name.contains("meas", Qt::CaseInsensitive))
                        m_settings->journals.insert(JournalType::Meas, journal);
                    else if (journal.name.contains("sys", Qt::CaseInsensitive))
                        m_settings->journals.insert(JournalType::System, journal);
                    //    m_settings->journals.push_back(parseJournal(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                    // domNode = domNode.nextSibling();
                }
                if (domElement.tagName() == "module")
                {
                    if (!isCorrectModule(domElement.attribute("mtypem", ""), domElement.attribute("mtypeb", "")))
                    {
                        domNode = domNode.nextSibling();
                        continue;
                    }

                    // qDebug() << "Attr: " << domElement.attribute("mtypea", "") << domElement.attribute("mtypeb", "");
                }

                if (domElement.tagName() == "modbus")
                {
                    if (Board::GetInstance().interfaceType() == Board::RS485)
                        m_settings->ifaceSettings /*.settings*/
                            = /*QVariant::fromValue*/ (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "protocom")
                {
                    if (Board::GetInstance().interfaceType() == Board::USB)
                        m_settings->ifaceSettings /*.settings*/
                            = /*QVariant::fromValue*/ (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "iec60870")
                {
                    if (Board::GetInstance().interfaceType() == Board::Ethernet)
                        m_settings->ifaceSettings /*.settings*/
                            = /*QVariant::fromValue*/ (BaseInterface::iface()->parseSettings(domElement));

                    domNode = domNode.nextSibling();
                    continue;
                }
                //                else
                //                {
                //                    qDebug() << "TagName: " << domElement.tagName() << "\tText: " << /*qPrintable*/
                //                    (domElement.text());
                //                }
            }
        }
        traverseNode(domNode);
        domNode = domNode.nextSibling();
    }
}

bool Module::loadSettings()
{
    const auto moduleName = Board::GetInstance().moduleName();
    if (moduleName.isEmpty())
        return false;
    QDir directory(StdFunc::GetSystemHomeDir());
    qDebug() << directory;
    auto allFiles = directory.entryList(QDir::Files);
    auto xmlFiles = allFiles.filter(".xml");
    qDebug() << xmlFiles;
    QDomDocument domDoc;
    QFile file;
    for (const auto &xmlFile : xmlFiles)
    {
        if (xmlFile.contains(moduleName, Qt::CaseInsensitive))
            file.setFileName(directory.filePath(xmlFile));
    }
    if (file.fileName().isEmpty())
    {
        directory = QDir(":/module");
        allFiles = directory.entryList(QDir::Files);
        xmlFiles = allFiles.filter(".xml");
        qDebug() << xmlFiles;
        for (const auto &xmlFile : qAsConst(xmlFiles))
        {
            if (!xmlFile.contains(moduleName, Qt::CaseInsensitive))
                continue;
            if (!QFile::copy(directory.filePath(xmlFile), StdFunc::GetSystemHomeDir() + xmlFile))
            {
                qCritical() << Error::DescError;
                qInfo() << directory.filePath(xmlFile) << StdFunc::GetSystemHomeDir() + xmlFile;
            }

            return loadSettings();
        }
    }
    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            m_settings = std::unique_ptr<ModuleSettings>(new ModuleSettings);
            QDomElement domElement = domDoc.documentElement();
            traverseNode(domElement);
            file.close();
            return true;
        }
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
    else
    {
        return false;
        qCritical() << Error::FileOpenError << file.fileName();
    }
}

void Module::create(UniquePointer<Journals> jour)
{
    const auto &board = Board::GetInstance();
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        Q_ASSERT(jour != nullptr);
        addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }
    TimeDialog *tdlg = new TimeDialog;
    addDialogToList(tdlg, "Время", "time");

    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");
}
