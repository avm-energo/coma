#ifndef TUNEDIALOGA1_H
#define TUNEDIALOGA1_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "../config/configa1.h"

#define TUNEFILELENGTH  256

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz

class TuneDialogA1 : public QDialog
{
    Q_OBJECT
public:
    explicit TuneDialogA1(QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);
    void dataready(QByteArray);
    void SecondsRemaining(QString);

public slots:

private:
    enum TuneModes
    {
        TUNEMIP,
        TUNERET,
        TUNEMAN
    };

    static QStringList lbls()
    {
        QStringList sl;
        sl.append("1. Ввод пароля...");
        sl.append("2. Отображение схемы подключения...");
        sl.append("7.2.3. Проверка связи РЕТОМ и МИП...");
        sl.append("7.3.1. Получение настроечных коэффициентов...");
        sl.append("7.3.1.1. Установка настроечных коэффициентов по умолчанию...");
        sl.append("4. Установка коэффициентов...");
        sl.append("7.3.2. Получение текущих аналоговых данных...");
        sl.append("5. Сохранение значений фильтра...");
        sl.append("7.3.3. Расчёт коррекции смещений сигналов по фазе...");
        sl.append("7.3.4. Расчёт коррекции по частоте...");
        sl.append("7.3.5. Отображение ввода трёхфазных значений...");
        sl.append("7.3.6.1. Получение текущих аналоговых данных...");
        sl.append("7.3.6.2. Расчёт коррекции взаимного влияния каналов...");
        sl.append("7.3.7.1. Получение текущих аналоговых данных и расчёт настроечных коэффициентов по напряжениям...");
        sl.append("7.3.7.2. Сохранение конфигурации...");
        sl.append("7.3.7.3. Получение текущих аналоговых данных...");
        sl.append("7.3.7.4. Ввод измеренных значений...");
        sl.append("7.3.7.5. Расчёт настроечных коэффициентов по токам, напряжениям и углам...");
        sl.append("7.3.7.6. Сохранение конфигурации...");
        sl.append("7.3.7.7. Отображение ввода трёхфазных значений...");
        sl.append("7.3.7.8. Получение текущих аналоговых данных...");
        sl.append("7.3.7.10. Расчёт настроечных коэффициентов по токам, напряжениям и углам...");
        sl.append("7.3.8.1. Запись настроечных коэффициентов и переход на новую конфигурацию...");
        sl.append("7.3.8.2. Проверка аналоговых данных...");
        sl.append("7.3.9. Восстановление сохранённой конфигурации и проверка...");
        sl.append("Настройка окончена!");
        return sl;
    }

    bool Cancelled, DefConfig;
    ConfigA1 *CA1;
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
        float Ama1; 		// коэффициент в канале A1, ед./мА,
        float Bma1; 		// смещение в канале A1, ед.,
        float Ama2; 		// коэффициент в канале A2, ед./мА,
        float Bma2; 		// смещение в канале A2, ед.,
    };

    Bac Bac_block;

    // Блок Bdа – оцифрованные сигналы в масштабах АЦП и частота в Гц
    struct Bda
    {
        float Ueff_ADC[2];          // 0-1 Действующие значения напряжений в единицах АЦП
        float Frequency;            // 2 частота сигналов, Гц,
        float Pt100;                // 3 значение температурного сигнала
        float EXTmA1;               // 4 значение первого сигнала (4-20) мА
        float EXTmA2;               // 5 значение второго сигнала (4-20) мА
    };

    Bda Bda_block;

    void SetupUI();
    int CheckPassword();

    void WriteTuneCoefsToGUI();
    void ReadTuneCoefsFromGUI();
    int CheckTuneCoefs();
    bool IsWithinLimits(double number, double base, double threshold);
    void ShowScheme();
    int Start7_2_3();
    int Start7_3_1();
    int Start7_3_1_1();
    int Start7_3_2();
    int Start7_3_3();
    int Start7_3_4();
    int Start7_3_5();
    int Start7_3_6_2();
    int Start7_3_7_1();
    int Start7_3_7_2();
    int Start7_3_7_3();
    int Start7_3_7_4();
    int Start7_3_7_5();
    int Start7_3_7_6();
    int Start7_3_7_7();
    int Start7_3_7_8();
    int Start7_3_7_10();
    int Start7_3_8_1();
    int Start7_3_8_2();
    int Start7_3_9();
    int SaveUeff();
    int TuneDialogA1::StartCheckAnalogValues(double u, double i, double deg, bool tol); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность, 1 - повышенная
    int CheckAnalogValues(double u, double i, double p, double q, double s, double phi, double cosphi, double utol, double itol, double pht, double pt, double ct);
    bool SetConfA(int i2nom);
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    void MsgSetVisible(int msg, bool Visible=true);
    void OkMsgSetVisible(int msg, bool Visible=true);
    void ErMsgSetVisible(int msg, bool Visible=true);
    void SkMsgSetVisible(int msg, bool Visible=true);
    void MsgClear();
    int SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    void WaitNSeconds(int SecondsToWait);
    float ToFloat(QString text);

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void ReadAnalogMeasurements(int BdNum);
    void SetDefCoefs();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
    void UpdateNSecondsWidget();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // TUNEDIALOGA1_H
