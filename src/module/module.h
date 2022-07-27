#pragma once

#include "../gen/datatypes.h"
#include "../module/alarmstateall.h"
#include "../s2/configv.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/udialog.h"
#include "../widgets/widgetfactory.h"
#include "board.h"
#include "journals.h"
#include "modulealarm.h"
constexpr char resourceDirectory[] = ":/module";

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
    ModuleSettingsDeprecated *settings() const;
    bool loadUsioSettings(const Modules::StartupInfoBlock &startupInfoBlock);
    bool loadSettings(const Modules::StartupInfoBlock &startupInfoBlock = Board::GetInstance().baseSerialInfo(),
        int interfaceType = Board::GetInstance().interfaceType());
    bool loadMainSettings(const QString &filename);
    bool obtainXmlFile(const QString &filename) const;
    bool obtainXmlConfig(const QString &filename, QList<DataTypes::RecordPair> &config) const;
    bool obtainXmlCheck(const QString &filename, std::vector<CheckItem> &check) const;
    bool obtainXmlAlarm(const QString &filename, AlarmMap &alarmMap, Modules::AlarmType type) const;
    bool loadS2Settings();
    bool loadCheckSettings(CheckSettings &settings);
    bool loadCheckSettings(
        Modules::BaseBoard typeB, Modules::MezzanineBoard typeM, std::vector<CheckItem> &check) const;
    quint64 configVersion() const;
    bool isConfigOutdated() const;
    void eraseSettings() const;
    void putConfigVersion() const;

    const QString &directory() const;
    void setDirectory(const QString &newDirectory);

    const GlobalSettings &gsettings() const
    {
        return m_gsettings;
    }

protected:
    ConfigV configV;
    GlobalSettings m_gsettings;

private:
    QList<UDialog *> m_dialogs;
    QString m_directory;
    std::unique_ptr<ModuleSettingsDeprecated> m_settings;
};
