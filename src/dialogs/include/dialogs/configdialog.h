#pragma once

#include <dialogs/dialogcreator.h>
#include <dialogs/errconfstate.h>
#include <s2/s2datamanager.h>
#include <set>
#include <widgets/udialog.h>
#include <widgets/widgetfactory.h>

class QTabWidget;

class ConfigDialog final : public UDialog
{
public:
    explicit ConfigDialog(Device::CurrentDevice *device, const S2BoardType boardType, QWidget *parent = nullptr);
    void setDefaultConfig();

private slots:
    void parseStatusHandle(const Error::Msg status);
    void noConfigurationHandle();
    void fillBack();

private:
    QWidget *ConfButtons();
    quint32 tabForId(quint16 id);
    std::set<delegate::WidgetGroup> createTabs(QTabWidget *tabWidget);
    void setupUI();

    void fill();
    bool prepareConfigToWrite();
    void uponInterfaceSetting() override;
    void checkConfig();
    void saveConfigToFile();
    void loadConfigFromFile();
    void readConfig();
    void writeConfig();
    void checkForDiff();
    bool isVisible(const quint16 id) const;
    bool isDebugWidget(const quint16 id) const;
    void showConfigErrState();

    S2DataManager &m_datamanager;
    S2BoardConfig &m_boardConfig;
    WidgetFactory m_factory;
    ErrConfState *m_errConfState;
    QStringList m_confErrors;
};
