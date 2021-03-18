#pragma once

#include "../module/modules.h"
#include "../widgets/udialog.h"
#include "baseconfig.h"
//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public UDialog, protected BaseConfig
{
    Q_OBJECT
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    void PrereadConf();

protected:
    QStringList CheckConfErrors;
    QWidget *ConfButtons();

    virtual void SetupUI() = 0;
    bool PrepareConfToWrite();
    void uponInterfaceSetting() override;
    template <typename T> void SPBData(QObject *parent, unsigned number)
    {
        S2::setRecordValue({ number, WDFunc::SPBData<T>(parent, nameByValue(number)) });
    }

private slots:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    void confReceived(const QList<DataTypes::DataRecV> &list);
    void confParametersListReceived(const DataTypes::ConfParametersListStruct &cfpl);
};
