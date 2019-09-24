#ifndef ABSTRACTCONFDIALOG_H
#define ABSTRACTCONFDIALOG_H

#define MAXBYTEARRAY    65535

#include <QDialog>
#include "config.h"

#define MAXCONFSIZE 4096 // максимальный размер файла конфигурации

class AbstractConfDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractConfDialog(QWidget *parent = nullptr);

    QVector<S2::DataRec> *S2Config;

    QStringList CheckConfErrors;
    bool IsNeededDefConf = false;

    QWidget *ConfButtons();
    virtual void Fill() = 0; // заполнить значения полей вывода из структуры конфигурации
    virtual void FillBack() = 0; // ввести информацию из полей вывода в конфигурацию
    virtual void SetDefConf() = 0; // задать конфигурацию по умолчанию
    virtual void CheckConf() = 0; // проверить конфигурацию на корректность, признаком наличия некорректностей
                                    // является непустой список CheckConfErrors
#if PROGSIZE != PROGSIZE_EMUL
    void PrereadConf();
#endif
    int GetChNumFromObjectName(QString ObjectName);


private:

    virtual void SetupUI() = 0;
    bool PrepareConfToWrite();

private slots:
    void SaveConfToFile();
    void LoadConfFromFile();
#if PROGSIZE != PROGSIZE_EMUL
    void ReadConf();
    void WriteConf();
    void Fill_tg_init(float *tg);
#endif
signals:
    void BsiIsNeedToBeAcquiredAndChecked(); // signal to reload start block emitted when new configuration has been sent to module
    void NewConfToBeLoaded(); // signal to load configuration in all appropriate windows (main conf, base conf, mez conf)
    void DefConfToBeLoaded(); // signal to load default configuration
};

#endif // ABSTRACTCONFDIALOG_H
