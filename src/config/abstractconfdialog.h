#pragma once
#include "../module/modules.h"
#include "../widgets/udialog.h"
#include "baseconfig.h"
#include "widgetfactory.h"

#include <set>
//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public UDialog, protected BaseConfig
{
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    void PrereadConf();

    void FillBack() const override;

protected:
    std::set<BciNumber> m_list;
    QStringList CheckConfErrors;
    QWidget *ConfButtons();

    virtual void SetupUI();
    void Fill() override;
    bool PrepareConfToWrite();
    void uponInterfaceSetting() override;

private:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    void checkForDiff(const QList<DataTypes::DataRecV> &list) const;
    void confReceived(const QList<DataTypes::DataRecV> &list);
};
