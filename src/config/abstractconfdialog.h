#pragma once
#include "../gen/s2.h"
#include "../module/modules.h"
#include "../widgets/udialog.h"
#include "../widgets/wd_func.h"
//#define MAXBYTEARRAY 65535
//#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public UDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    // заполнить значения полей вывода из структуры конфигурации
    virtual void Fill() = 0;
    // ввести информацию из полей вывода в конфигурацию
    virtual void FillBack() = 0;
    // проверить конфигурацию на корректность, признаком наличия некорректностей является непустой список
    // CheckConfErrors
    virtual void CheckConf() = 0;

    void PrereadConf();

public slots:
    //    void update() override;
    // задать конфигурацию по умолчанию
    virtual void SetDefConf() = 0;

protected:
    QStringList CheckConfErrors;
    QWidget *ConfButtons();

    virtual void SetupUI() = 0;
    bool PrepareConfToWrite();
    void uponInterfaceSetting() override;
    QString nameByValue(quint32 number)
    {
        return QString::number(number);
    }
    template <typename T> void SetCBIndex(QObject *parent, unsigned number)
    {
        WDFunc::SetCBIndex(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }
    template <typename T> void SetSPBData(QObject *parent, unsigned number)
    {
        WDFunc::SetSPBData(parent, nameByValue(number), S2::getRecord(number).value<T>());
    }

private slots:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    void confReceived(const QList<DataTypes::DataRecV> &list);
    void confParametersListReceived(const DataTypes::ConfParametersListStruct &cfpl);
};
