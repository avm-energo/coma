#pragma once

#include "../module/errconfstate.h"
#include "../module/modules.h"
#include "../widgets/udialog.h"
#include "../widgets/widgetfactory.h"

//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class ConfigDialog : public UDialog
{
public:
    explicit ConfigDialog(ConfigV *config, const QList<S2DataTypes::RecordPair> &defaultConfig, bool prereadConf = true,
        QWidget *parent = nullptr);
    void prereadConfig();
    void fillBack() const;
    void setDefaultConfig();

private:
    QWidget *ConfButtons();

    void setupUI();
    void createTabs(QTabWidget *tabWidget);
    void fill();
    bool prepareConfigToWrite();
    void uponInterfaceSetting() override;
    void checkConfig();
    void saveConfigToFile();
    void loadConfigFromFile();
    void readConfig();
    void writeConfig();
    void checkForDiff(const QList<S2DataTypes::DataRecV> &list);
    void configReceived(const QVariant &msg);

    void showConfigErrState();

    bool m_prereadConf;
    QStringList CheckConfErrors;
    const QList<S2DataTypes::RecordPair> m_defaultValues;
    ConfigV *configV;
    UniquePointer<DataTypesProxy> proxyDRL;
    ErrConfState *errConfState;
};
