#pragma once

#include "../module/errconfstate.h"
#include "../module/modules.h"
#include "../s2/s2datamanager.h"
#include "../widgets/udialog.h"
#include "../widgets/widgetfactory.h"

//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class QTabWidget;

class ConfigDialog : public UDialog
{
public:
    explicit ConfigDialog(S2DataManager &s2manager, const S2BoardType boardType, QWidget *parent = nullptr);
    void prereadConfig();
    void setDefaultConfig();

private:
    QWidget *ConfButtons();

    quint32 tabForId(quint16 id);
    void createTabs(QTabWidget *tabWidget);
    void setupUI();

    void fill();
    void fillBack() const;

    bool prepareConfigToWrite();
    void uponInterfaceSetting() override;
    void checkConfig();
    void saveConfigToFile();
    void loadConfigFromFile();
    void readConfig();
    void writeConfig();
    void checkForDiff();
    bool isVisible(const quint16 id) const;
    void configReceived(const QByteArray &rawData);

    void showConfigErrState();

    S2DataManager &m_datamanager;
    S2BoardConfig &m_boardConfig;
    WidgetFactory m_factory;
    UniquePointer<DataTypesProxy> m_proxyDRL;
    ErrConfState *m_errConfState;
    QStringList m_confErrors;
};
