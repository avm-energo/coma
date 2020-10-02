#pragma once

#define MAXBYTEARRAY 65535

#include "config.h"

#include <QDialog>

#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    S2ConfigType *S2Config;

    QStringList CheckConfErrors;
    bool IsNeededDefConf = false;
    //    int confIndex; //, timeIndex; //, checkIndex;

    QWidget *ConfButtons();
    virtual void Fill() = 0; // заполнить значения полей вывода из структуры конфигурации
    virtual void FillBack() = 0; // ввести информацию из полей вывода в конфигурацию
    virtual void SetDefConf() = 0; // задать конфигурацию по умолчанию
    virtual void CheckConf() = 0;  // проверить конфигурацию на корректность,
                                   // признаком наличия некорректностей
    void ReadConf();
    // является непустой список CheckConfErrors
    void PrereadConf();
    // int GetChNumFromObjectName(QString ObjectName);

    Error::Msg WriteCheckPassword();

public slots:
    void WriteConfMessageOk();

private:
    bool ok;
    virtual void SetupUI() = 0;
    bool PrepareConfToWrite();

private slots:
    void SaveConfToFile();
    void LoadConfFromFile();
    void ButtonReadConf();

    void WritePasswordCheck(QString psw);

public slots:
    void WriteConf();
    void FillConf(S2ConfigType *);

signals:
    void writeConfFile(S2ConfigType *);
    void ReadConfig(char);
    void BsiIsNeedToBeAcquiredAndChecked(); // signal to reload start block
                                            // emitted when new configuration has
                                            // been sent to module
    void NewConfToBeLoaded();               // signal to load configuration in all appropriate
                                            // windows (main conf, base conf, mez conf)
    void DefConfToBeLoaded();               // signal to load default configuration
                                            //    void StopRead(int);
    void WritePasswordChecked();
};
