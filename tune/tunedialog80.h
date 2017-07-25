#ifndef TUNEDIALOG80_H
#define TUNEDIALOG80_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "../config/config80.h"
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

// parameters for GetExtData
#define TD_GED_U    0x01 // напряжение
#define TD_GED_I    0x02 // ток
#define TD_GED_F    0x04 // частота
#define TD_GED_D    0x08 // угол нагрузки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz

class TuneDialog80 : public QDialog
{
    Q_OBJECT
public:
    explicit TuneDialog80(QVector<publicclass::DataRec> &S2Config, QWidget *parent = 0);

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
        sl.append("3. Отображение схемы подключения...");
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
    Config80 *C80;
    QVector<publicclass::DataRec> *S2Config;
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

    struct MipValues
    {
        double u;
        double i[3];
    };

    MipValues MVTC;

    struct MipTolerances
    {
        double u;
        double i;
    };

    MipTolerances MTTC;

    struct BdaValues
    {
        double tmk;
        double bat;
        double freq;
        double v1;
        double v2;
        double phi;
        double p;
        double s;
        double q;
        double cosphi;
    };

    BdaValues VTCG;

    struct BdaTolerances
    {
        double v1;
        double v2;
        double phi;
        double p;
        double cosphi;
    };

    BdaTolerances TTCG;

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

    struct RealDataStruct
    {
        float f[3]; // frequencies
        float u[3]; // voltages
        float i[3]; // currents
        float d[3]; // load phase
    };

    RealDataStruct RealData;
    float IUefNat_filt_old[6];      // для сохранения значений по п. 7.3.2
    float MipDat[41];
    int GED_Type;

    void SetupUI();
    void Tune3p();
    void WriteTuneCoefsToGUI();
    void ReadTuneCoefsFromGUI();
    int CheckTuneCoefs();
    int CheckMip();
    bool IsWithinLimits(double number, double base, double threshold);
    int ShowControlChooseDialog();
    int Show3PhaseScheme();
    void Show1RetomDialog(float U, float A);
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
    int ShowRetomDialog(double U, double I);
    int TuneDialog80::StartCheckAnalogValues(double u, double i, double deg, bool tol); // deg - угол в градусах между токами и напряжениями одной фазы, tol - 0: начальная точность, 1 - повышенная
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
    int SaveWorkConfig();
    int LoadWorkConfig();
    QHBoxLayout *MipPars(int parnum, const QString &groupname);
    float ToFloat(QString text);

private slots:
    void StartTune();
    void ReadTuneCoefs();
    void WriteTuneCoefs();
    void SaveToFile();
    void LoadFromFile();
    void StartMip();
    void StopMip();
    void ParseMipData(Parse104::Signals104 &);
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

#endif // TuneDialog80_H
