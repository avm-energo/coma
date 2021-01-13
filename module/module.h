#ifndef MODULEFABRIC_H
#define MODULEFABRIC_H

#include "../gen/datatypes.h"
#include "../widgets/alarmstateall.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"
#include "modulealarm.h"

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
    // QList<DataTypes::Alarm> alarms;
    QMap<AlarmType, DataTypes::Alarm> alarms;
    // QList<DataTypes::Journal> journals;
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

    static Module *createModule(QTimer *updateTimer, AlarmWidget *aw);
    QList<UDialog *> dialogs();
    QList<UDialog *> confDialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    //    ModuleAlarm *getAlarm();
    //    ModuleAlarm *getWarn();
    //    AlarmStateAll *getAlarmStateAll();
    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabChanged(int index);
    //    void setDefConf();
    void closeDialogs();
    ModuleSettings *settings() const;

signals:

public slots:

private:
    QList<UDialog *> m_dialogs;
    //    ModuleAlarm *m_accAlarm;
    //    AlarmStateAll *m_alarmStateAllDialog;
    //    ModuleAlarm *m_warnAlarm;
    //    int m_currentTabIndex, m_oldTabIndex;
    //    BaseInterface *m_iface;
    bool loadSettings();
    std::unique_ptr<ModuleSettings> m_settings;
    void traverseNode(const QDomNode &node);

    DataTypes::Alarm parseAlarm(QDomElement domElement);
    DataTypes::Journal parseJournal(QDomElement domElement);
    quint32 parseInt32(QDomElement domElement) const;
    quint32 parseHexInt32(QDomElement domElement) const;
    QStringList parseStringList(QDomElement domElement) const;
    InterfaceInfo<CommandsMBS::ModbusGroup> parseModbus(QDomElement domElement);
    InterfaceInfo<Proto::ProtocomGroup> parseProtocom(QDomElement domElement);
    InterfaceInfo<Commands104::Iec104Group> parseIec104(QDomElement domElement);
};

#endif // MODULEFABRIC_H
