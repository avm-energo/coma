#pragma once

#define MAXBYTEARRAY 65535

#include "../widgets/udialog.h"
#include "config.h"

#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public UDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    //    bool IsNeededDefConf = false;
    //    int confIndex; //, timeIndex; //, checkIndex;

    // заполнить значения полей вывода из структуры конфигурации
    virtual void Fill() = 0;
    // ввести информацию из полей вывода в конфигурацию
    virtual void FillBack() = 0;
    // проверить конфигурацию на корректность, признаком наличия некорректностей является непустой список
    // CheckConfErrors
    virtual void CheckConf() = 0;
    //    void setConnections() override;

    //    void ReadConf();
    //
    void PrereadConf();
    // int GetChNumFromObjectName(QString ObjectName);

    // bool WriteCheckPassword();

public slots:
    void WriteConfMessageOk(const DataTypes::GeneralResponseStruct &rsp);
    //    void update() override;
    // задать конфигурацию по умолчанию
    virtual void SetDefConf() = 0;

protected:
    // bool ok;
    // S2DataTypes::S2ConfigType *S2Config;
    QStringList CheckConfErrors;
    QWidget *ConfButtons();

    virtual void SetupUI() = 0;
    bool PrepareConfToWrite();
    void uponInterfaceSetting() override;

private slots:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ReadConf();
    void WriteConf();
    void confParametersListReceived(const DataTypes::ConfParametersListStruct &cfpl);
    //    void confParameterReceived(const DataTypes::ConfParameterStruct &cfp);

    // public slots:
    //    void FillConf(S2ConfigType *);

    // signals:
    //    void writeConfFile(S2ConfigType *);
    //    void ReadConfig(char);
    //    // signal to reload start block  emitted when new configuration has been sent to module
    //    void BsiIsNeedToBeAcquiredAndChecked();
    //    // signal to load configuration in all windows (main conf, base conf, mez conf)
    //    void NewConfToBeLoaded();
    //    // signal to load default configuration
    //    void DefConfToBeLoaded();
    //    void StopRead(int);
    //    void WritePasswordChecked();

    // UWidget interface
};