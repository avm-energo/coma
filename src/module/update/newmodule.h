#pragma once

#include "../gen/datatypes.h"
#include "../module/alarmstateall.h"
#include "../s2/configv.h"
//#include "../widgets/alarmwidget.h"
//#include "../widgets/udialog.h"
#include "../widgets/widgetfactory.h"
#include "board.h"
#include "journals.h"
#include "modulealarm.h"

class NewModule : public QObject
{
    Q_OBJECT
public:
    explicit NewModule(QObject *parent = nullptr);

    /*
    virtual void createAlarm(AlarmWidget *aw);
    virtual void create(QTimer *updateTimer)
    {
        Q_UNUSED(updateTimer);
    };
    void create(UniquePointer<Journals> jour);
    void createCommon();
    virtual void createModule(Modules::Model model)
    {
        Q_UNUSED(model);
    };
    virtual void create(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM)
    {
        Q_UNUSED(typeB);
        Q_UNUSED(typeM);
    };

    QList<UDialog *> dialogs();
    void addDialogToList(UDialog *dlg, const QString &caption = "", const QString &name = "");
    void startTimeTimer();
    void stopTimeTimer();
    void parentTWTabChanged(int index);
    void closeDialogs();

    bool loadCheckSettings(CheckSettings &settings);
    bool loadCheckSettings(
        Modules::BaseBoard typeB, Modules::MezzanineBoard typeM, std::vector<CheckItem> &check) const;
    bool loadUsioSettings(const Modules::StartupInfoBlock &startupInfoBlock);
    bool obtainXmlFile(const QString &filename);
    bool obtainXmlConfig(const QString &filename, QList<DataTypes::RecordPair> &config);
    bool obtainXmlCheck(const QString &filename, std::vector<CheckItem> &check);
    bool obtainXmlAlarm(const QString &filename, AlarmMap &alarmMap, Modules::AlarmType type);
    */

    ModuleSettingsDeprecated *settings() const;
    bool loadSettings(const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        int interfaceType = Board::GetInstance().interfaceType());

private:
    bool isFileExist(const QString &filename);
    QDomElement getFileContent(const QString &filename);
    bool loadS2Settings();
    bool loadMainSettings(const QString &filename);

    // QList<UDialog *> m_dialogs;
    ConfigV configV;
    GlobalSettings m_gsettings;
    std::unique_ptr<ModuleSettingsDeprecated> m_settings;
};
