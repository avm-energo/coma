#ifndef MODULEFABRIC_H
#define MODULEFABRIC_H

#include "../gen/datatypes.h"
#include "../module/alarmstateall.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"
#include "journals.h"
#include "modulealarm.h"
#include "modules.h"
enum AlarmType
{
    Warning,
    Critical,
    All
};
enum JournalType
{
    Work,
    Meas,
    System
};
struct ModuleSettings
{
    int alarmCount() const
    {
        int counter = 0;
        for (const auto &alarm : alarms)
            counter += alarm.desc.size();
        return counter;
    }
    QMap<AlarmType, DataTypes::Alarm> alarms;
    QMap<JournalType, DataTypes::Journal> journals;
    InterfaceSettings ifaceSettings;
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
    virtual void create(AlarmWidget *aw);
    virtual void create(QTimer *updateTimer) = 0;
    void create(UniquePointer<Journals> jour);
    virtual void create(Modules::Model model) = 0;
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

private:
    QList<UDialog *> m_dialogs;

    std::unique_ptr<ModuleSettings> m_settings;
    void traverseNode(const QDomNode &node);

    DataTypes::Alarm parseAlarm(QDomElement domElement);
    DataTypes::Journal parseJournal(QDomElement domElement);
    quint32 parseInt32(QDomElement domElement) const;
    quint32 parseHexInt32(QDomElement domElement) const;
    QStringList parseStringList(QDomElement domElement) const;
};

#endif // MODULEFABRIC_H
