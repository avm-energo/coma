#ifndef E_TUNEDIALOG_H
#define E_TUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
#include <QTimer>
#include "../iec104/iec104.h"

#define TUNEFILELENGTH  256

#define TUNEMIP 1
#define TUNERET 2
#define TUNEMAN 3

#define ANMEASINT   2000

class e_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_tunedialog(QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);
    void dataready(QByteArray);
    void error(int);

public slots:

private:
    iec104 *mipcanal;
    int TuneControlType;
    bool MipErrNeeded;
    QTimer *tmr;

    struct Bac
    {
        float KmU[6];
        float KmI_5[6];
        float KmI_1[6];
        float DPsi[6];
        float K_freq;
        float Kinter;
    };

    Bac Bac_block, Bac_defblock, Bac_newblock;

    struct Bda
    {
        float Tmk;                  // Температура кристалла микроконтроллера,
        float Frequency;            // частота сигналов, Гц,
        float IUefNat_filt[6];      // Истинные действующие значения сигналов
                                    // в вольтах и амперах,
        float IUeff_filtered[6];    // Действующие значения сигналов по 1-й гармонике,
        float phi_next_f[6];        // Углы сдвига между сигналами по 1-й гармонике
                                    // в 6 каналах в градусах,
        float PNatf[3];             // Истинная активная мощность, по фазам,
        float SNatf[3];             // Кажущаяся полная мощность,
                                    // по эффективным  токам и напряжениям,
        float QNatf[3];             // Реактивная мощность по кажущейся полной,
                                    // и истинной активной,
        float CosPhiNat[3];         // cos phi по истинной активной мощности,
        float Pf[3];                // Активная мощность по 1-й гармонике,
        float Qf[3];                // Реактивная мощность по 1-й гармонике,
        float Sf[3];                // Полная мощность по 1-й гармонике,
        float CosPhi[3];            // cos phi по 1-й гармонике
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
    void CalcNewTuneCoefs();
    void RefreshTuneCoefs();
    bool CheckTuneCoefs();
    bool CheckAnalogValues(int ntest);
    bool CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    void RefreshAnalogValues();
    void ShowControlChooseDialog();
    void Show1PhaseScheme();
    void Show3PhaseScheme();
    void Show1RetomDialog(int A);
    bool Start7_2_3();
    bool Start7_3_1();
    bool Start7_3_2(int num);
    bool Start7_3_3();
    bool Start7_3_4();
    void Start7_3_6_2();
    bool Start7_3_7_1();
    bool Start7_3_7_2();
    bool SetConfA(int i2nom);
    void GetExternalData(int numexc); // ввод данных в зависимости от выбранного режима и номера опыта
    void ShowMsg(int msg);
    void ShowOkMsg(int msg);
    void ShowErMsg(int msg);

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void StartMip();
    void StopMip();
    void DeleteMip();
    void EthConnected();
    void EthDisconnected();
    void MipData(Parse104::Signals104 &);
    void SetTuneMip();
    void SetTuneRetom();
    void SetTuneManual();
    void StartAnalogMeasurements();
    void StopAnalogMeasurements();
    void ReadAnalogMeasurements();
    void SetExtData();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // E_TUNEDIALOG_H
