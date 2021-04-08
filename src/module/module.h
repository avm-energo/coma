#ifndef MODULEFABRIC_H
#define MODULEFABRIC_H

#include "../config/baseconfig.h"
#include "../config/widgetfactory.h"
#include "../gen/datatypes.h"
#include "../module/alarmstateall.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"
#include "journals.h"
#include "modulealarm.h"
#include "modules.h"

struct ModuleSettings
{
    int alarmCount() const
    {
        int counter = 0;
        for (const auto &alarm : alarms)
            counter += alarm.desc.size();
        return counter;
    }
    QMap<Modules::AlarmType, DataTypes::Alarm> alarms;
    QMap<Modules::JournalType, DataTypes::JournalDesc> journals;
    InterfaceSettings ifaceSettings;
    QList<DataTypes::RecordPair> configSettings;
};

struct GlobalSettings
{
    S2DataTypes::valueTypeMap *s2filesMap;
    widgetMap *s2widgetMap;
};

class Module : public QObject
{
    Q_OBJECT
public:
    enum DialogTypes
    {
        CHECK = 0,
        CONF = 1,
        TUNE = 2
    };

    explicit Module(QObject *parent = nullptr);
    virtual void createAlarm(AlarmWidget *aw);
    virtual void create(QTimer *updateTimer) = 0;
    void create(UniquePointer<Journals> jour);
    void createCommon();
    virtual void createModule(Modules::Model model) = 0;
    virtual void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM) = 0;
    QList<UDialog *> dialogs();
    QList<UDialog *> confDialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    void insertDialogToList(UDialog *dlg, int pos = 0, const QString &caption = "", const QString &name = "");

    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabChanged(int index);
    void closeDialogs();
    ModuleSettings *settings() const;
    bool loadSettings();
    bool loadS2Settings();
    quint64 configVersion() const;
    bool isConfigOutdated() const;
    void eraseSettings() const;
    void putConfigVersion() const;

private:
    QList<UDialog *> m_dialogs;

    std::unique_ptr<ModuleSettings> m_settings;

    // void traverseNode(const QDomNode &node);
};

#endif // MODULEFABRIC_H
