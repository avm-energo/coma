#ifndef E_TUNEDIALOG_H
#define E_TUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
#include <QTimer>
#include <QLabel>
#include "../iec104/iec104.h"
#include "e_confdialog.h"

#define TUNEFILELENGTH  256

#define TUNEMIP 1
#define TUNERET 2
#define TUNEMAN 3

#define ETUNEER(a)       ERMSG(publicclass::ER_ETUNE,__LINE__,a)
#define ETUNEDBG         DBGMSG(publicclass::ER_ETUNE,__LINE__)
#define ETUNEWARN        WARNMSG(publicclass::ER_ETUNE,__LINE__)
#define ETUNEINFO(a)     INFOMSG(publicclass::ER_ETUNE,__LINE__,a)

#define MSG_7_2_3       0
#define MSG_7_3_1       1
#define MSG_7_3_1_1     2
#define MSG_7_3_2       3
#define MSG_7_3_3       4
#define MSG_7_3_4       5
#define MSG_7_3_5       6
#define MSG_7_3_6_0     7
#define MSG_7_3_6_1     8
#define MSG_7_3_6_2     9
#define MSG_7_3_7_1_1   10
#define MSG_7_3_7_1_2   11
#define MSG_7_3_7_2     12
#define MSG_7_3_7_3     13
#define MSG_7_3_7_5     14
#define MSG_7_3_7_6     15
#define MSG_7_3_7_8     16
#define MSG_7_3_7_10    17
#define MSG_7_3_8_1     18
#define MSG_7_3_8_2     19
#define MSG_7_3_9       20
#define MSG_END         21
#define MSG_COUNT       22

class e_tunedialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_tunedialog(QWidget *parent = 0);

signals:
    void stopall();
    void SendMip(QByteArray);
    void dataready(QByteArray);
    void SecondsRemaining(QString);

public slots:

private:
    bool Cancelled;
    e_config *econf;
    e_config::Bci Bci_block_work;
    iec104 *mipcanal;
    int TuneControlType;
    QTimer *tmr;
    int SecondsToEnd15SecondsInterval;
    QLabel *Label15Seconds;

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
        float Tmk;                  // 0 Температура кристалла микроконтроллера,
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
    bool CheckAnalogValues(int ntest);
    bool CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    void ShowControlChooseDialog();
    void Show1PhaseScheme();
    void Show3PhaseScheme();
    void Show1RetomDialog(float U, float A);
    bool Start7_2_3();
    bool Start7_3_1(bool DefConfig);
    bool Start7_3_2(int num);
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
    void MsgClear();
    void SetNewTuneCoefs(); // заполнение Bac_newblock, чтобы не было пурги после настройки
    void WaitNSeconds(int SecondsToWait);
    bool SaveWorkConfig();
    bool LoadWorkConfig();
    void SetTunePbEnabled(bool Enabled);

private slots:
    void StartTune();
    void StartTune3p(); // начало калибровки с пп. 7.3.6
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
    void ReadAnalogMeasurements();
    void SetDefCoefs();
    void SetExtData();
    void CancelExtData();
    void CancelTune();
    void UpdateNSecondsWidget();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // E_TUNEDIALOG_H
