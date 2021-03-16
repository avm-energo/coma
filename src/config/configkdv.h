#ifndef CONFIGKDV_H
#define CONFIGKDV_H

#include "../gen/datatypes.h"
#include "../module/modules.h"
#include "config.h"
#include "configkxx.h"

namespace AVM_KDV
{
constexpr unsigned DEF_NFILTR = 100;
constexpr unsigned DEF_NHARMFILT = 3;
constexpr unsigned DEF_DDOCS = 7;
constexpr float DEF_UNOM1 = 0.38f;
constexpr unsigned DEF_DUOSC = 2;
constexpr unsigned DEF_DIOSC = 2;
constexpr float DEF_DUIMIN = 0.5;
constexpr float DEF_IMIN = 0.2f;
constexpr unsigned DEF_T_DATA_REC = 900;
constexpr unsigned DEF_U2NOM = 100;
constexpr unsigned DEF_ITT1NOM = 600;
constexpr unsigned DEF_ITT2NOM = 5;
constexpr unsigned DEF_COOL_TYPE = 0;
constexpr unsigned DEF_W_MAT = 0;
constexpr unsigned DEF_TNNTDOP = 105;
constexpr unsigned DEF_TNNTPRED = 100;
constexpr unsigned DEF_TAMB_NOM = 25;
constexpr unsigned DEF_DTNNT_NOM = 70;
constexpr float DEF_KDOB = 0.1f;
constexpr unsigned DEF_TAUWNOM = 10;
constexpr unsigned DEF_UMAX = 120;
constexpr unsigned DEF_IMAX = 33;
constexpr unsigned DEF_IWNOM = 300;
constexpr unsigned DEF_OSCPOINTS = 32;
constexpr unsigned DEF_GTNNT = 2;
constexpr unsigned DEF_GOVC = 2;
constexpr unsigned DEF_TDATNUM = 0;
constexpr unsigned DEF_EQ_TYPE = 0;
constexpr unsigned DEF_FNOM = 50;
constexpr float DEF_NOM_SLIP = 0.03f;
constexpr float DEF_UVMAX = 5.0f;
constexpr unsigned DEF_POLES = 2;
constexpr unsigned DEF_STATOR_SLOTES = 36;
constexpr unsigned DEF_ROTOR_BARS = 24;
constexpr unsigned DEF_VIBROTYPE = 0;
constexpr unsigned DEF_SENSORS = 0x03;
constexpr float DEF_KVIBR = 0.1f;
constexpr float DEF_VIBRA_PRED = 8.0f;
constexpr float DEF_VIBRV_PRED = 25.0f;
constexpr float DEF_VIBRD_PRED = 80.0f;
constexpr float DEF_VIBRA_ALARM = 9.85f;
constexpr float DEF_VIBRV_ALARM = 31.4f;
constexpr float DEF_VIBRD_ALARM = 100.0f;
constexpr float DEF_VVIBRA_PRED = 0.3f;
constexpr float DEF_VVIBRV_PRED = 1.0f;
constexpr float DEF_VVIBRD_PRED = 3.0f;
constexpr float DEF_VVIBRA_ALARM = 0.6f;
constexpr float DEF_VVIBRV_ALARM = 2.0f;
constexpr float DEF_VVIBRD_ALARM = 6.0f;
constexpr unsigned DEF_NUMA = 1;
}

class ConfigKDV
{
public:
    explicit ConfigKDV();

    AVM_KDV::Bci Bci_block;
    void setConfig();
    void SetDefConf();
    Config *MainConfig();
    ConfigKxx *KxxConfig();

    //    typedef struct
    //    {
    //        quint32 Eq_type; //тип контролируемого оборудования: 0 - асинхронный
    //                         //двигатель, 1- трансформатор, 2 - реактор
    //        quint32 Cool_type; //вид охлаждения: 0 – естественное, 1 – принудительное
    //        quint32 W_mat;     //материал обмотки: 0 – медь, 1 – алюминий
    //        quint32 NFiltr;    // Интервал усреднения данных – 100 периодов
    //        quint32 NHarmFilt; // Постоянная времени фильтрации гармоник (3 цикла)
    //        quint32 DDosc;     // Разрешение запуска осциллограммы
    //                           // áèò 0 - îò ÖÊÓ, áèò 1 - ïî ñèãíàëó PD1, áèò 2 - ïî ñêà÷êó
    //                           // ðåæèìà.
    //        float Unom1;       // Номинальное линейное первичное напряжение, кВ
    //        float U2nom;       // Номинальное линейное вторичное напряжение ТН, В
    //        float ITT1nom;     // Номинальный первичный ток
    //        float ITT2nom;     // Номинальный вторичный ток , А
    //        float Iwnom;       //Номинальный ток контролируемой обмотки, À
    //        float DUosc; // Уставка скачка напряжения для запуска осциллографирования -
    //                     // % от номинала
    //        float DIosc; // Уставка скачка тока для запуска осциллографирования -  % от
    //                     // I2nom
    //        float DUImin; // Уставка порога минимального напряжения - % от номинального
    //                      // уровня
    //        float Imin; // Уставка порога минимального тока - % от номинального уровня
    //        float TNNTdop; // Предельно допустимая температура ННТ в°С
    //        float TNNTpred; // Уставка предупредительной сигнализации по температуре ННТ
    //                        // в °С
    //        float Tamb_nom; // Номинальная температура окружающей среды,  °С
    //        float dTNNTnom; // Превышение температуры ННТ при номинальной нагрузке, °С
    //        float Kdob; // Коэффициент добавочных потерь
    //        float TauWnom; // Постоянная времени нагрева обмотки в номинальном режиме, мин
    //        float Umax; // Максимальное измеряемое фазное напряжение на входе прибора, В
    //                    // эфф (не более 305В)
    //        float Imax; // Максимальный измеряемый ток на входе прибора, А эфф  (не
    //                    // более 33А)
    //        float GTnnt; // Гистерезис сигнализации по температуре ННТ,  град.С
    //        float GOvc; // Гистерезис сигнализации по токовой перегрузке, % от Iwnom
    //        float Fnom;     //номинальная частота, Гц
    //        float nom_slip; // номинальное скольжение
    //        float UVmax; // Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)

    //        float Kvibr; // Коэффициент передачи датчиков вибрации
    //                     // для типа 0 - в В/(м/с/с), для типа 1 - в В/(мм/с), для типа
    //                     // 2 - в В/мкм.
    //        float VibrA_pred; // предупредительная уставка по СКЗ виброускорения, м/с/с
    //        float VibrV_pred; // предупредительная уставка по СКЗ виброскорости, мм/с
    //        float VibrD_pred; // предупредительная уставка по СКЗ виброперемещения, мкм
    //        float VibrA_alarm;  // аварийная уставка по СКЗ виброускорения, м/с/с
    //        float VibrV_alarm;  // аварийная уставка по СКЗ виброскорости, мм/с
    //        float VibrD_alarm;  // аварийная уставка по СКЗ виброперемещения, мкм
    //        float VVibrA_pred;  // предупредительная уставка по скорости роста СКЗ
    //                            // виброускорения, м/с/с
    //        float VVibrV_pred;  // предупредительная уставка по скорости роста СКЗ
    //                            // виброскорости, мм/с
    //        float VVibrD_pred;  // предупредительная уставка по скорости роста СКЗ
    //                            // виброперемещения, мкм
    //        float VVibrA_alarm; // аварийная уставка по скорости роста СКЗ
    //                            // виброускорения, м/с/с
    //        float VVibrV_alarm; // аварийная уставка по скорости роста СКЗ
    //                            // виброскорости, мм/с
    //        float VVibrD_alarm; // аварийная уставка по скорости роста СКЗ
    //                            // виброперемещения, мкм

    //        quint32 NumA;          // порядковый номер двигателя
    //        quint32 Poles;         //число пар полюсов (от 1 до 10)
    //        quint32 Stator_Slotes; // число пазов на статоре
    //                               // для асинхронных двигателей:
    //        quint32 Rotor_bars;    // число стержней ротора
    //        quint32 VibroType;     // Тип вибродатчика: 0 - виброускорение, 1 -
    //                               // виброскорость, 2 - виброперемещение.
    //        quint32 Sensors;       // Подключенные датчики вибрации:
    //                               // (отметки "1" в битах 0..5 для датчиков в каналах 1..6
    //        quint32 T_Data_Rec;    // Интервал записи данных в ПЗУ (тренд), с
    //        quint32 OscPoints; // Количество точек осциллограммы на период основной частоты
    //        quint32 TdatNum; // Количество датчиков температуры обмоток, подключенных по
    //                         // Modbus Master

    //    } Bci;

    //    Bci Bci_block;

    //    S2DataTypes::S2ConfigType *S2Config();

private:
    //    S2DataTypes::S2ConfigType *m_S2Config;

private:
    Config *m_mainConfig;
    ConfigKxx *m_KxxConfig;

    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKDV_H
