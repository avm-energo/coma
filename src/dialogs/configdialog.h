#pragma once

#include "../module/errconfstate.h"
#include "../module/modules.h"
#include "../module/s2requestservice.h"
#include "../widgets/udialog.h"
#include "../widgets/widgetfactory.h"

#include <s2/s2datamanager.h>

//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class QTabWidget;

class ConfigDialog : public UDialog
{
public:
    explicit ConfigDialog(S2RequestService &s2service, S2DataManager &s2manager, //
        const S2BoardType boardType, QWidget *parent = nullptr);
    void setDefaultConfig();

private slots:
    void parseStatusHandle(const Error::Msg status);
    void noConfigurationHandle();
    void fillBack();

private:
    QWidget *ConfButtons();
    quint32 tabForId(quint16 id);
    void createTabs(QTabWidget *tabWidget);
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
    void showConfigErrState();

    S2RequestService &m_requestService;
    S2DataManager &m_datamanager;
    S2BoardConfig &m_boardConfig;
    WidgetFactory m_factory;
    ErrConfState *m_errConfState;
    QStringList m_confErrors;
};
