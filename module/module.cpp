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
#include "../startup/startupkdvdialog.h"
#include "../startup/startupkivdialog.h"
#include "../startup/startupktfdialog.h"
#include "../widgets/udialog.h"
#include "alarmkdv.h"
#include "alarmkiv.h"
#include "alarmktf.h"
#include "journkdv.h"
#include "journkiv.h"
#include "journktf.h"
#include "modules.h"

#include <QDir>
#include <QtXml>
#ifdef AVM_DEBUG
//#include "../tune/kiv/tunekdvdialog.h"
#include "../tune/kiv/tunekivdialog.h"
//#include "../tune/kiv/tunektfdialog.h"
#endif

Module::Module(QObject *parent) : QObject(parent)
{
    // m_dialogs.clear();
    // m_oldTabIndex = m_currentTabIndex = 0;
}

// Module *Module::createModule(QTimer *updateTimer, BaseInterface *iface, AlarmWidget *aw)
Module *Module::createModule(QTimer *updateTimer, AlarmWidget *aw)
{
    using namespace Modules;
    const auto &board = Board::GetInstance();
    Journals *JOUR = nullptr;
    Module *m = new Module;
    //    m->m_iface = iface;
    S2::config = new S2DataTypes::S2ConfigType;
    // ModuleAlarm *criticalAlarm;

    // ModuleAlarm *warningAlarm;
    // m->m_alarmStateAllDialog->UpdateHealth(board.health());
    //    quint16 typeb = Board::GetInstance().typeB();
    quint16 typeb = board.typeB();
    if (!m->loadSettings())
        return m;
    BaseInterface::iface()->setSettings(m->settings()->ifaceSettings);
    //    aw->setInterface(iface);
    // aw->uponInterfaceSetting();
    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    Q_ASSERT(aw->count() == 0);
    aw->addAlarm(alarmStateAll);
    if (BaseBoards.contains(typeb)) // there must be two-part module
    {
        //        quint16 typem = Board::GetInstance().typeM();
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
        //        quint16 mtype = Board::GetInstance().type();
        quint16 mtype = board.type();
        switch (mtype)
        {
        case Model::KIV:
        {
            JOUR = new JournKIV(m);
            ConfigKIV *CKIV = new ConfigKIV;
            m->addDialogToList(new ConfKIVDialog(CKIV), "Конфигурирование", "conf1");
            CheckKIVDialog *cdkiv = new CheckKIVDialog;
            m->addDialogToList(cdkiv, "Проверка");
#ifdef AVM_DEBUG
            //            TuneKIV *TKIV = new TuneKIV;
            m->addDialogToList(new TuneKIVDialog(CKIV), "Регулировка");
#endif
            m->addDialogToList(new StartupKIVDialog, "Начальные\nзначения");
            //            connect(m->m_warn, &Warn::updateWarn, cdkiv, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdkiv, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Model::KTF:
        {
            JOUR = new JournKTF(m);
            ConfigKTF *CKTF = new ConfigKTF;
            m->addDialogToList(new ConfKTFDialog(CKTF), "Конфигурирование", "conf1");
            CheckKTFDialog *cdktf = new CheckKTFDialog;
            m->addDialogToList(cdktf);
#ifdef AVM_DEBUG
            //            TuneKTF *TKTF = new TuneKTF(0, s2Config);
//                        m->addDialogToList(new TuneKTFDialog(CKTF, TKTF));
#endif
            m->addDialogToList(new StartupKTFDialog, "Старение\nизоляции");
            m->addDialogToList(new CheckKTFHarmonicDialog, "Гармоники");
            // aw->addAlarm(new WarnKTF);
            // aw->addAlarm(new CritKTF);
            //            connect(m->m_warn, &Warn::updateWarn, cdktf, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdktf, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        case Model::KDV:
        {
            JOUR = new JournKDV(m);
            ConfigKDV *CKDV = new ConfigKDV;
            m->addDialogToList(new ConfKDVDialog(CKDV), "Конфигурирование", "conf1");
            CheckKDVDialog *cdkdv = new CheckKDVDialog;
            m->addDialogToList(cdkdv);
#ifdef AVM_DEBUG
//            TuneKDV *TKDV = new TuneKDV;
//            m->addDialogToList(new TuneKDVDialog(CKDV, TKDV));
#endif
            m->addDialogToList(new StartupKDVDialog, "Старение\nизоляции");
            m->addDialogToList(new CheckKDVHarmonicDialog, "Гармоники");
            m->addDialogToList(new CheckKDVVibrDialog, "Вибрации");
            //            VibrDialog = new CheckDialogVibrKDV(BoardTypes::BT_BASE);
            //            connect(BdaTimer, &QTimer::timeout, VibrDialog, &AbstractCheckDialog::USBUpdate);
            // aw->addAlarm(new WarnKDV);
            // aw->addAlarm(new CritKDV);
            //            connect(m->m_warn, &Warn::updateWarn, cdkdv, &AbstractCheckDialog::SetWarnColor);
            //            connect(m->m_alarm, &Alarm::updateAlarm, cdkdv, &AbstractCheckDialog::SetAlarmColor);
            break;
        }
        default:
            assert(false);
        }
    }
    auto *warnAlarm = new ModuleAlarm(m->settings()->alarms.value(AlarmType::Warning), m->settings()->alarmCount());
    aw->addAlarm(warnAlarm, tr("Предупредительная сигнализация"));
    auto *critAlarm = new ModuleAlarm(m->settings()->alarms.value(AlarmType::Critical), m->settings()->alarmCount());
    aw->addAlarm(critAlarm, tr("Аварийная сигнализация"));
    TimeDialog *tdlg = new TimeDialog;
    m->addDialogToList(tdlg, "Время", "time");

    //    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    if (board.interfaceType() != Board::InterfaceType::RS485)
    {
        Q_ASSERT(JOUR != nullptr);
        m->addDialogToList(new JournalDialog(JOUR), "Журналы");
    }
    else
        delete JOUR;
    //    if (Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    if (board.interfaceType() != Board::InterfaceType::RS485)
        m->addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    m->addDialogToList(new InfoDialog, "О приборе", "info");

    QList<UDialog *> dlgs = m->dialogs();
    for (auto *d : dlgs)
    {
        connect(updateTimer, &QTimer::timeout, d, &UDialog::reqUpdate);
        // d->setUpdatesDisabled();
        d->uponInterfaceSetting();
        //        d->setInterface(m->m_iface);
    }

    //    aw->addAlarm(warningAlarm);
    //    aw->addAlarm(criticalAlarm);

    return m;
}

QList<UDialog *> Module::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

QList<UDialog *> Module::confDialogs()
{
    QList<UDialog *> list;
    for (UDialog *dlg : m_dialogs)
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

// ModuleAlarm *Module::getAlarm()
//{
//    return m_accAlarm;
//}

// ModuleAlarm *Module::getWarn()
//{
//    return m_warnAlarm;
//}

// AlarmStateAll *Module::getAlarmStateAll()
//{
//    return m_alarmStateAllDialog;
//}

void Module::parentTWTabChanged(int index)
{
    if (index == -1)
        return;
    //    if (index == m_currentTabIndex) // to prevent double function invocation by doubleclicking on tab
    //        return;
    //    m_currentTabIndex = index;

    //    if (m_oldTabIndex >= m_dialogs.size())
    //    {
    //        DBGMSG("BdUIList size");
    //        return;
    //    }
    // UDialog *udlg = m_dialogs.at(m_oldTabIndex);
    for (auto &dialog : m_dialogs)
    {
        if (dialog->updatesEnabled())
            dialog->setUpdatesDisabled();
        // dialog->setEnabled(false);
    }
    //    UDialog *udlg = qobject_cast<UDialog *>(dlg);
    //    if (udlg)
    // udlg->setUpdatesDisabled();
    //    if (m_currentTabIndex >= m_dialogs.size())
    //    {
    //        DBGMSG("BdUIList size");
    //        return;
    //    }
    UDialog *udlg = m_dialogs.at(index);
    //    udlg = qobject_cast<UDialog *>(dlg);
    //    if (udlg)
    udlg->setEnabled(true);
    udlg->setUpdatesEnabled();
    udlg->reqUpdate();
    // m_oldTabIndex = m_currentTabIndex;
}

// void Module::setDefConf()
//{
//    foreach (QDialog *dlg, m_dialogs)
//    {
//        if (dlg->objectName().contains("conf"))
//        {
//            static_cast<AbstractConfDialog *>(dlg)->SetDefConf();
//            static_cast<AbstractConfDialog *>(dlg)->Fill();
//        }
//    }
//}

void Module::closeDialogs()
{
    if (!m_dialogs.isEmpty())
        for (auto &i : m_dialogs)
            i->close();
    delete S2::config;
}

ModuleSettings *Module::settings() const
{
    return m_settings.get();
}

quint32 Module::parseInt32(QDomElement domElement) const
{

    qDebug() << domElement.attribute("name", "") << domElement.text();
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
    qDebug() << domElement.attribute("name", "") << domElement.text();
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
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
    while (i != nodes.count())
    {
        description.push_back(nodes.item(i++).toElement().text());
    }
    return description;
}

InterfaceInfo<CommandsMBS::ModbusGroup> Module::parseModbus(QDomElement domElement)
{
    qDebug() << domElement.text();
    qDebug() << "TagName: " << domElement.tagName();
    const auto &nodes = domElement.childNodes();
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
    // ModbusSettings settings;
    InterfaceInfo<CommandsMBS::ModbusGroup> settings;
    while (i != nodes.count())
    {
        const auto &group = nodes.item(i++).toElement();
        auto test = CommandsMBS::ModbusGroup(group);
        settings.addGroup(test);
        qDebug() << group.attribute("id", "") << group.text();
    }

    qDebug() << settings.groups().count();
    return settings;
}

InterfaceInfo<Proto::ProtocomGroup> Module::parseProtocom(QDomElement domElement)
{
    qDebug() << domElement.text();
    qDebug() << "TagName: " << domElement.tagName();
    const auto &nodes = domElement.childNodes();
    Q_ASSERT(!nodes.isEmpty());
    int i = 0;
    InterfaceInfo<Proto::ProtocomGroup> settings;
    while (i != nodes.count())
    {
        const auto &group = nodes.item(i++).toElement();
        Proto::ProtocomGroup test(group);
        settings.addGroup(test);
        qDebug() << group.attribute("id", "") << group.text();
    }

    qDebug() << settings.groups().count();
    return settings;
}

InterfaceInfo<Commands104::Iec104Group> Module::parseIec104(QDomElement domElement)
{
    InterfaceInfo<Commands104::Iec104Group> settings;
    return settings;
}

DataTypes::Alarm Module::parseAlarm(QDomElement domElement)
{
    DataTypes::Alarm alarm;
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
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
    qDebug() << "TagName: " << domElement.tagName() << domElement.attribute("name", "");
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
    qDebug() << typem << mtypem;
    qDebug() << typem << mtypeb;
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

                    qDebug() << "Attr: " << domElement.attribute("name", "");
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

                    qDebug() << "Attr: " << domElement.attribute("name", "");

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
                        m_settings->ifaceSettings.settings = parseModbus(domElement);

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "protocom")
                {
                    if (Board::GetInstance().interfaceType() == Board::USB)
                        m_settings->ifaceSettings.settings = parseProtocom(domElement);

                    domNode = domNode.nextSibling();
                    continue;
                }
                if (domElement.tagName() == "iec60870")
                {
                    if (Board::GetInstance().interfaceType() == Board::Ethernet)
                        m_settings->ifaceSettings.settings = parseIec104(domElement);

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
    auto directory = QDir::current();
    directory.cdUp();
    // directory.cdUp();
    directory.cd("settings");
    qDebug() << directory;
    auto allFiles = directory.entryList(QDir::Files);
    auto xmlFiles = allFiles.filter(".xml");
    // xmlFiles.contains()
    qDebug() << xmlFiles;
    QDomDocument domDoc;
    QFile file;
    for (const auto &xmlFile : xmlFiles)
    {
        if (xmlFile.contains(moduleName, Qt::CaseInsensitive))
            file.setFileName(directory.filePath(xmlFile));
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
