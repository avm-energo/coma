#pragma once
#include "../gen/modules.h"
#include "../widgets/udialog.h"
#include "widgetfactory.h"

//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class ConfigDialog : public UDialog
{
public:
    explicit ConfigDialog(ConfigV *config, const QList<DataTypes::RecordPair> &defaultConfig, bool prereadConf = true,
        QWidget *parent = nullptr);

    void PrereadConf();

    void FillBack() const;
    void SetDefConf();

private:
    QWidget *ConfButtons();

    void SetupUI();
    void createTabs(QTabWidget *tabWidget);
    void Fill();
    bool PrepareConfToWrite();
    void uponInterfaceSetting() override;
    void CheckConf();
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    void checkForDiff(const QList<DataTypes::DataRecV> &list);
    void confReceived(const QList<DataTypes::DataRecV> &list);

    bool m_prereadConf;
    QStringList CheckConfErrors;
    const QList<DataTypes::RecordPair> m_defaultValues;
    ConfigV *configV;
};
