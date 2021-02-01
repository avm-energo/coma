#include "module.h"

#include "../check/checkkdvdialog.h"
#include "../check/checkkdvharmonicdialog.h"
#include "../check/checkkdvvibrdialog.h"
#include "../check/checkkivdialog.h"
#include "../check/checkktfdialog.h"
#include "../check/checkktfharmonicdialog.h"
#include "../config/confkdvdialog.h"
#include "../config/confkivdialog.h"
#include "../config/confktfdialog.h"
#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../tune/kiv/tunekivdialog.h"
#include "../widgets/udialog.h"
#include "journkdv.h"
#include "journkiv.h"
#include "journktf.h"
//#define XML_DEBUG

#include <QDir>
#include <QtXml>

Module::Module(QObject *parent) : QObject(parent)
{
}

Module::Module(QTimer *updateTimer, AlarmWidget *aw, QObject *parent) : QObject(parent)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    UniquePointer<Journals> jour;
    // Journals *JOUR = nullptr;

    // S2::config = UniquePointer<S2DataTypes::S2ConfigType>(new S2DataTypes::S2ConfigType);

    quint16 typeb = board.typeB();
    if (!loadSettings())
    {
        qCritical() << "No conf .xml file for this module";
        return;
    }

    BaseInterface::iface()->setSettings(settings()->ifaceSettings);

    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    Q_ASSERT(aw->count() == 0);
    aw->addAlarm(alarmStateAll);
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        quint16 typem = board.typeM();
        Q_UNUSED(typem)
        switch (typeb)
        {
        case BaseBoards::MTB_00:
            /*
                str = (checkMDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nБазовая";
                if (checkBDialog != nullptr)
                {
                    checkBDialog->setMinimumHeight(500);
                    MainTW->addTab(checkBDialog, str);
                    CheckIndex = MainTW->indexOf(checkBDialog);
                }
                str = (checkBDialog == nullptr) ? "Текущие параметры" : "Текущие параметры\nМезонин";
                if (checkMDialog != nullptr)
                    MainTW->addTab(checkMDialog, str);
            */
        default:
            break;
        }
    }
    else
    {
        quint16 mtype = board.type();
        create(Modules::Model(mtype));
    }
    auto *warnAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());
    aw->addAlarm(warnAlarm, tr("Предупредительная сигнализация"));
    auto *critAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical), settings()->alarmCount());
    aw->addAlarm(critAlarm, tr("Аварийная сигнализация"));
    TimeDialog *tdlg = new TimeDialog;
    addDialogToList(tdlg, "Время", "time");

    if (board.interfaceType() != Board::InterfaceType::RS485)
    {
        m->addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }

    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");

    QList<UDialog *> dlgs = dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        d->uponInterfaceSetting();
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

void Module::create(Modules::Model model)
{
    using namespace Modules;
    Journals *JOUR = nullptr;
    const auto &board = Board::GetInstance();
    switch (model)
    {
    case Model::KIV:
    {
        JOUR = new JournKIV(this);
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKIV *CKIV = new ConfigKIV;
            addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
            //            if (board.interfaceType() == Board::InterfaceType::USB)
            //            {
            //                if (appType == Debug)
            //                    addDialogToList(new TuneKIVDialog(CKIV), "Регулировка");
            //            }
        }
        CheckKIVDialog *cdkiv = new CheckKIVDialog;
        addDialogToList(cdkiv, "Проверка");

        addDialogToList(new StartupKIVDialog, "Начальные\nзначения");

        break;
    }
    case Model::KTF:
    {
        JOUR = new JournKTF(this);
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKTF *CKTF = new ConfigKTF;
            addDialogToList(new ConfKTFDialog(CKTF), "Конфигурирование", "conf1");
        }
        CheckKTFDialog *cdktf = new CheckKTFDialog;
        addDialogToList(cdktf);
        // if (appType == Debug)
        // TuneKTF *TKTF = new TuneKTF(0, s2Config);
        //                        m->addDialogToList(new TuneKTFDialog(CKTF, TKTF));

        addDialogToList(new StartupKTFDialog, "Старение\nизоляции");
        addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
        break;
    }
    case Model::KDV:
    {
        JOUR = new JournKDV(this);
        if (board.interfaceType() != Board::InterfaceType::RS485)
        {
            ConfigKDV *CKDV = new ConfigKDV;
            addDialogToList(new ConfKDVDialog(CKDV), "Конфигурирование", "conf1");
        }
        CheckKDVDialog *cdkdv = new CheckKDVDialog;
        addDialogToList(cdkdv);
        // if (appType == Debug)
        //            TuneKDV *TKDV = new TuneKDV;
        //            m->addDialogToList(new TuneKDVDialog(CKDV, TKDV));

        addDialogToList(new StartupKDVDialog, "Старение\nизоляции");
        addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
        addDialogToList(new CheckKDVVibrDialog, "Вибрации");
        //            VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE);
        //            connect(BdaTimer, &QTimer::timeout, VibrDialog, &AbstractCheckDialog::USBUpdate);
        break;
    }
    default:
        assert(false);
    }
}