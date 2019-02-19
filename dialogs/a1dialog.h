#ifndef A1DIALOG_H
#define A1DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QCloseEvent>
#include "../tune/eabstracttunedialoga1dn.h"
#include "../gen/report.h"
#include "../gen/maindef.h"
#include "../config/configa1.h"
#include "../check/checka1.h"

#define TUNE_POINTSPER  500 // столько миллисекунд должно усредняться при регулировке
#define TUNEVARIANTSNUM 3

class A1Dialog : public EAbstractTuneDialogA1DN
{
    Q_OBJECT
public:
    explicit A1Dialog(const QString &filename = "", QWidget *parent = nullptr);
    ~A1Dialog();

    struct MainDataStruct
    {
        float S; // Значение мощности нагрузки, установленное на i-м испытании
        float U; // Относительное значение напряжения
        float dUp; // Относительная погрешность измерения по rms
        float ddUp; // Среднеквадратическое отклонение dUp
        float dPp; // Абсолютная погрешность по углу
        float ddPp; // Среднеквадратическое отклонение dPp
        float dUd; // Погрешность поверочной установки по rms
        float dPd; // Погрешность поверочной установки по углу
        float ddUd; // Среднеквадратичное отклонение dUd
        float ddPd; // Среднеквадратичное отклонение dPd
    };
    struct ResultsStruct
    {
        quint64 Time;
        quint32 GOST;
        quint64 SerNum;
        quint64 DNFNum;
        quint32 VarNum;
        float Temp;
        float Humidity;
        float THD;
        float Frequency;
    };


private:
    int Index, Counter;
    int TempPovType; // тип поверяемого оборудования (по какому ГОСТу)

    bool Autonomous; // =1, если производится формирование протокола из файла, =0 - при работе с прибором
    bool TempFromLE, HumFromLE; // =1, если данные в протокол надо брать из поля ввода, =0 - если из прибора
//    bool Cancelled;

    void SetupUI();
#if PROGSIZE != PROGSIZE_EMUL
    int GetConf();
#endif
    void SetLbls() {}; // заполнить список сообщений
    void SetPf() {}; // заполнить список функций настройки
    void GenerateReport(); // сгенерировать протокол
    void Go();

signals:

private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartWork();
    void Decline();
    void Proceed();
#endif
    void Cancel();
    void ParsePKDNFile(const QString &filename);
    void RBToggled();
    int ReadAnalogMeasurements();

private:

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // A1DIALOG_H
