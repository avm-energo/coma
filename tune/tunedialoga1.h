#ifndef TUNEDIALOGA1_H
#define TUNEDIALOGA1_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "../config/configa1.h"
#include "../check/checka1.h"

#define TDA1_MEASNUM    4 // количество проводимых измерений для усреднения

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz

class TuneDialogA1 : public QDialog
{
    Q_OBJECT
public:
    explicit TuneDialogA1(QWidget *parent = 0);

signals:
    void PasswordChecked();
    void stopall();
    void dataready(QByteArray);
    void SecondsRemaining(QString);

public slots:

private:
    static QStringList lbls()
    {
        QStringList sl;
        sl.append("1. Ввод пароля...");
        sl.append("2. Отображение схемы подключения...");
        sl.append("6.2. Проверка правильности измерения сигналов переменного напряжения...");
        sl.append("6.3.1. Получение настроечных коэффициентов...");
        sl.append("6.3.2.1. КПТ: получение блока данных и усреднение...");
        sl.append("6.3.2.2. КПТ: ввод данных от энергомонитора...");
        sl.append("6.3.2.3. КПТ: расчёт регулировочных коэффициентов...");
        sl.append("6.3.3.1. КТС: подтверждение установки 80 Ом...");
        sl.append("6.3.3.2. КТС: получение блока данных...");
        sl.append("6.3.3.3. КТС: подтверждение установки 120 Ом...");
        sl.append("6.3.3.4. КТС: получение блока данных и расчёт регулировочных коэффициентов...");
        sl.append("6.3.4. КМТ2: подтверждение установки 4 мА...");
        sl.append("6.3.5.1. КМТ2: получение блока данных...");
        sl.append("6.3.5.2. КМТ2: подтверждение установки 20 мА...");
        sl.append("6.3.5.3. КМТ2: получение блока данных и расчёт регулировочных коэффициентов...");
        sl.append("6.3.6. КМТ1: подтверждение установки 4 мА...");
        sl.append("6.3.7.1. КМТ1: получение блока данных...");
        sl.append("6.3.7.2. КМТ1: подтверждение установки 20 мА...");
        sl.append("6.3.7.3. КМТ1: получение блока данных и расчёт регулировочных коэффициентов...");
        sl.append("6.3.8. Запись настроечных коэффициентов и переход на новую конфигурацию...");
        sl.append("6.3.9. Проверка аналоговых данных...");
        sl.append("6.3.10. Проверка аналоговых данных...");
        sl.append("Настройка окончена!");
        return sl;
    }

    bool Cancelled, Skipped, DefConfig, ok;
    ConfigA1 *CA1;
    Check_A1 ChA1;
    QVector<publicclass::DataRec> S2Config;
    int SecondsToEnd15SecondsInterval;
    QHash <QString, int (TuneDialogA1::*)()> pf;

    struct Bac
    {
        float KmU[2];		// калибровочные коэффициенты по напряжению в 2 каналах
        float K_freq;		// коррекция частоты
        float DPhy;         // коррекция фазы в приборе (в градусах)
        float U1kDN[6];     // измеренные при калибровке напряжения на выходе своего ДН для значений вблизи 12, 24, 36, 48, 60 и 72 В
        float U2kDN[6];     // и соответствующие им значения на выходе эталонного делителя.
        float PhyDN[6]; 	// фазовый сдвиг ДН на частоте 50 Гц для значений напряжения U1kDN[6]
        float Art;          // коэффициент в канале Pt100, ед.АЦП/Ом
        float Brt;          // смещение в канале Pt100, ед.АЦП
        float Ama1; 		// коэффициент в канале A1, ед./мА
        float Bma1; 		// смещение в канале A1, ед.
        float Ama2; 		// коэффициент в канале A2, ед./мА
        float Bma2; 		// смещение в канале A2, ед.
    };

    Bac Bac_block;

    struct EMData
    {
        float u1;
        float u2;
        float phy;
        float freq;
    };

    EMData RealData;

    float RegData;

    void SetupUI();

    int CheckPassword();
    int ShowScheme();
    int Start6_2();
    int CheckBdaValues();
    int Start6_3_1();
    int Start6_3_2_1();
    int CheckAnalogValues(bool isPrecise); // проверка Bda_in на корректность
    int Start6_3_2_2();
    int Start6_3_2_3();
    int Start6_3_3_1();
    int Start6_3_3_2();
    int Start6_3_3_3();
    int Start6_3_3_4();
    int Start6_3_4();
    int Start6_3_5_1();
    int Start6_3_5_2();
    int Start6_3_5_3();
    int Start6_3_6();
    int Start6_3_7_1();
    int Start6_3_7_2();
    int Start6_3_7_3();
    int Start6_3_8();
    int Start6_3_9();
    int Start6_3_10();

    bool IsWithinLimits(double number, double base, double threshold);
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    void MsgSetVisible(int msg, bool Visible=true);
    void OkMsgSetVisible(int msg, bool Visible=true);
    void ErMsgSetVisible(int msg, bool Visible=true);
    void SkMsgSetVisible(int msg, bool Visible=true);
    void MsgClear();
    void WaitNSeconds(int SecondsToWait);
    void SaveToFileEx();

private slots:
    void PasswordCheck(QString psw);
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    int ReadAnalogMeasurements();
    void FillBac();
    void FillBackBac();
    void SetDefCoefs();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
    void UpdateNSecondsWidget();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // TUNEDIALOGA1_H
