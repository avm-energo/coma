#ifndef TUNEDIALOG80_H
#define TUNEDIALOG80_H

#include <QDialog>
#include <QCloseEvent>
#include <QGroupBox>
#include <QByteArray>
#include <QStringList>
#include "config/config80.h"
#include "../iec104/iec104.h"

#define TUNEFILELENGTH  256

// voltages
#define V60     60.0
#define V57     57.74
// frequencies
#define HZ50    50.0
// currents
#define I1      1.0
#define I5      5.0
// zero signal
#define S0      0.0
// thresholds
#define TMAX    FLT_MAX
#define T25     25.0
#define T5      5.0
#define T1      1.0
#define T05     0.5
#define T01     0.1
#define T005    0.05
#define T002    0.02
#define T0005   0.005
// tune coefficients
#define TU0     0.0
#define TU1     1.0

class TuneDialog80 : public QDialog
{
    Q_OBJECT
public:
    explicit TuneDialog80(QWidget *parent = 0);

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
        sl.append("1. Сохранение текущей конфигурации...");
        sl.append("2. Отображение диалога выбора режима контроля показаний...");
        sl.append("7.2.3. Проверка связи РЕТОМ и МИП...");
        sl.append("3. Отображение схемы подключения...");
        sl.append("7.3.1. Получение настроечных коэффициентов...");
        sl.append("7.3.1.1. Установка настроечных коэффициентов по умолчанию...");
        sl.append("4. Установка коэффициентов...");
        sl.append("7.3.2. Получение текущих аналоговых данных...");
        sl.append("7.3.3. Расчёт коррекции смещений сигналов по фазе...");
        sl.append("7.3.4. Расчёт коррекции по частоте...");
        sl.append("7.3.5. Считывание напряжений при нулевых токах...");
        sl.append("7.3.6.0. Пересборка схемы подключения...");
        sl.append("7.3.6.1. Получение текущих аналоговых данных...");
        sl.append("7.3.6.2. Расчёт коррекции взаимного влияния каналов...");
        sl.append("7.3.7.1.1. Получение текущих аналоговых данных...");
        sl.append("7.3.7.1.2. Расчёт настроечных коэффициентов по напряжениям...");
        sl.append("7.3.7.2. Сохранение конфигурации и перезапуск...");
        sl.append("7.3.7.3. Получение текущих аналоговых данных...");
        sl.append("7.3.7.5. Расчёт настроечных коэффициентов по токам, напряжениям и углам...");
        sl.append("7.3.7.6. Сохранение конфигурации и перезапуск...");
        sl.append("7.3.7.8. Получение текущих аналоговых данных...");
        sl.append("7.3.7.10. Расчёт настроечных коэффициентов по токам, напряжениям и углам...");
        sl.append("7.3.8.1. Запись настроечных коэффициентов и переход на новую конфигурацию...");
        sl.append("7.3.8.2. Проверка аналоговых данных...");
        sl.append("7.3.9. Восстановление сохранённой конфигурации и проверка...");
        sl.append("Настройка окончена!");
        return sl;
    }

    bool Cancelled, DefConfig;
    Config80 *C80;
    QVector<publicclass::DataRec> S2Config;
    Config80::Bci Bci_block_work;
    iec104 *mipcanal;
    int TuneControlType;
    int SecondsToEnd15SecondsInterval;
    QHash <QString, int (TuneDialog80::*)()> pf;

    struct Bac
    {
        float KmU[6];
        float KmI_5[6];
        float KmI_1[6];
        float DPsi[6];
        float K_freq;
        float Kinter;
    };

    Bac Bac_block, Bac_newblock;

    struct BdaValues
    {
        float tmk;
        float bat;
        float freq;
        float v1;
        float v2;
        float phi;
        float p;
        float s;
        float q;
        float cosphi;
    };

    BdaValues VTCG;

    struct Bda
    {
        float Tmk;                  // 0 Температура кристалла микроконтроллера,
        float Vbat;                 // 1 напряжение батареи, В
        float Frequency;            // 1 частота сигналов, Гц,
        float IUefNat_filt[6];      // 2-7 Истинные действующие значения сигналов
                                    // в вольтах и амперах,
        float IUeff_filtered[6];    // 8-13 Действующие значения сигналов по 1-й гармонике,
        float phi_next_f[6];        // 14-19 Углы сдвига между сигналами по 1-й гармонике
                                    // в 6 каналах в градусах,
        float PNatf[3];             // 20-22 Истинная активная мощность, по фазам,
        float SNatf[3];             // 23-25 Кажущаяся полная мощность,
                                    // по эффективным  токам и напряжениям,
        float QNatf[3];             // 26-28 Реактивная мощность по кажущейся полной,
                                    // и истинной активной,
        float CosPhiNat[3];         // 29-31 cos phi по истинной активной мощности,
        float Pf[3];                // 32-34 Активная мощность по 1-й гармонике,
        float Qf[3];                // 35-37 Реактивная мощность по 1-й гармонике,
        float Sf[3];                // 38-40 Полная мощность по 1-й гармонике,
        float CosPhi[3];            // 41-43 cos phi по 1-й гармонике
    };

    Bda Bda_block;

    typedef struct
    {
        quint32 MType;          // Тип модуля, для которого предназначена конфигурация
        quint32 MType1;         // Подтип модуля, для которого предназначена конфигурация
        quint32 eq_type;        // Тип контролируемого оборудования: 0 - 1фТАТ, 1 - 3фТАТ, 2 - 1фР, 3 - 3фР
        quint32 npoints;        // Количество точек оцифровки на период (64/80/128/256)
        quint32 nfiltr;         // Интервал усреднения данных (постоянная фильтрации)
        quint32 nhfiltr;        // Постоянная фильтрации гармоник
        quint32 ddosc;          // События-инициаторы запуска осциллографирования
        float unom1;            // Класс напряжения 1-й группы
        float unom2;            // Класс напряжения 2-й группы
        float inom1[6];         // Номинальный первичный ток внешнего ТТ по каналам
        float inom2[6];         // Номинальный вторичный ток внешнего ТТ по каналам
        float duosc;            // Уставка скачка напряжения для запуска осциллографирования (в % от номинального напряжения ТН)
        float diosc;            // Уставка скачка тока для запуска осциллографирования (в % от номинального тока ТТ)
        float duimin;           // Уставка контроля минимума сигналов (в %)
        quint32 Ctype;   		// Тип синхронизации времени от модуля Ц
        quint32 Abs_104;     	// Адрес базовой станции для протокола 104
        quint32 Cycle_104;      // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104;         // тайм-аут Т1 для протокола 104
        quint32 T2_104;         // тайм-аут Т2 для протокола 104
        quint32 T3_104;         // тайм-аут Т3 для протокола 104
        quint32 k_104;          // макс. кол-во неподтв. сообщений
        quint32 w_104;          // макс. кол-во сообщений, после которых необх. выдать подтверждение
    } Bci;

    float IUefNat_filt_old[6];      // для сохранения значений по п. 7.3.2
    float mipd[13];
    float MipDat[41];

    void SetupUI();
    void Tune3p();
    void WriteTuneCoefsToGUI();
    void ReadTuneCoefsFromGUI();
    bool CheckTuneCoefs();
    int CheckAnalogValues();
    int CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    int ShowControlChooseDialog();
    void Show3PhaseScheme();
    void Show1RetomDialog(float U, float A);
    int Start7_2_3();
    int Start7_3_1();
    int Start7_3_1_1();
    int Start7_3_2();
    bool Start7_3_3();
    bool Start7_3_4();
    void Start7_3_5();
    void Start7_3_6_2();
    bool Start7_3_7_1();
    bool Start7_3_7_2();
    bool Start7_3_8();
    bool Start7_3_9();
    bool SetConfA(int i2nom);
    bool GetExternalData(int numexc); // ввод данных в зависимости от выбранного режима и номера опыта
    void MsgSetVisible(int msg, bool Visible=true);
    void OkMsgSetVisible(int msg, bool Visible=true);
    void ErMsgSetVisible(int msg, bool Visible=true);
    void SkMsgSetVisible(int msg, bool Visible=true);
    void MsgClear();
    void SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    void WaitNSeconds(int SecondsToWait);
    int SaveWorkConfig();
    bool LoadWorkConfig();
    QGroupBox *MipPars(int parnum, const QString &groupname);
    float ToFloat(QString text);

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void StartMip();
    void StopMip();
    void MipData(Parse104::Signals104 &);
    void SetTuneMode();
    void ReadAnalogMeasurements();
    void SetDefCoefs();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
    void UpdateNSecondsWidget();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // TuneDialog8080_H
