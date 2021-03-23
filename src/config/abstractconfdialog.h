#pragma once

#include "../module/modules.h"
#include "../widgets/udialog.h"
#include "baseconfig.h"
//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public UDialog, protected BaseConfig
{
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    void PrereadConf();

    static widgetMap widgetMap()
    {
        return m_widgetMap;
    }
    static void setWidgetMap(const ::widgetMap &widgetMap)
    {
        m_widgetMap = widgetMap;
    }

protected:
    QStringList CheckConfErrors;
    QWidget *ConfButtons();

    virtual void SetupUI() = 0;
    bool PrepareConfToWrite();
    void uponInterfaceSetting() override;

private:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    void confReceived(const QList<DataTypes::DataRecV> &list);
    void confParametersListReceived(const DataTypes::ConfParametersListStruct &cfpl);

    static ::widgetMap m_widgetMap;
};
