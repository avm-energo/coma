#ifndef CONFIGKDV_H
#define CONFIGKDV_H

//..............
#define BCI_EQ_TYPE 1001
#define BCI_NFILTR 1003
#define BCI_NHARMFILT 1004
#define BCI_DDOCS 1005
#define BCI_UNOM1 1006
#define BCI_DUOSC 1010
#define BCI_DIOSC 1011
#define BCI_DUIMIN 1012
#define BCI_IMIN 1013
#define BCI_T_DATA_REC 1031
#define BCI_U2NOM 1050
#define BCI_ITT1NOM 1052
#define BCI_ITT2NOM 1053
#define BCI_COOL_TYPE 1070
#define BCI_W_MAT 1071
#define BCI_TNNTDOP 1072
#define BCI_TNNTPRED 1073
#define BCI_TAMB_NOM 1074
#define BCI_DTNNT_NOM 1075
#define BCI_KDOB 1076
#define BCI_TAUWNOM 1077
#define BCI_UMAX 1078
#define BCI_IMAX 1079
#define BCI_IWNOM 1080
#define BCI_OSCPOINTS 1081
#define BCI_GTNNT 1082
#define BCI_GOVC 1083
#define BCI_TDATNUM 1084

#define BCI_FNOM 1201
#define BCI_NOM_SLIP 1202
#define BCI_UVMAX 1203
#define BCI_POLES 1204
#define BCI_STATOR_SLOTES 1205
#define BCI_ROTOR_BARS 1206
#define BCI_VIBROTYPE 1207
#define BCI_SENSORS 1208
#define BCI_KVIBR 1209
#define BCI_VIBRA_PRED 1210
#define BCI_VIBRV_PRED 1211
#define BCI_VIBRD_PRED 1212
#define BCI_VIBRA_ALARM 1213
#define BCI_VIBRV_ALARM 1214
#define BCI_VIBRD_ALARM 1215
#define BCI_VVIBRA_PRED 1216
#define BCI_VVIBRV_PRED 1217
#define BCI_VVIBRD_PRED 1218
#define BCI_VVIBRA_ALARM 1219
#define BCI_VVIBRV_ALARM 1220
#define BCI_VVIBRD_ALARM 1221
#define BCI_NUMA 1222

//.............................

#define DEF_NFILTR 100
#define DEF_NHARMFILT 3
#define DEF_DDOCS 7
#define DEF_UNOM1 0.38
#define DEF_DUOSC 2
#define DEF_DIOSC 2
#define DEF_DUIMIN 0.5
#define DEF_IMIN 0.2
#define DEF_T_DATA_REC 900
#define DEF_U2NOM 100
#define DEF_ITT1NOM 600
#define DEF_ITT2NOM 5
#define DEF_COOL_TYPE 0
#define DEF_W_MAT 0
#define DEF_TNNTDOP 105
#define DEF_TNNTPRED 100
#define DEF_TAMB_NOM 25
#define DEF_DTNNT_NOM 70
#define DEF_KDOB 0.1
#define DEF_TAUWNOM 10
#define DEF_UMAX 120
#define DEF_IMAX 33
#define DEF_IWNOM 300
#define DEF_OSCPOINTS 32
#define DEF_GTNNT 2
#define DEF_GOVC 2
#define DEF_TDATNUM 0
#define DEF_EQ_TYPE 0
#define DEF_FNOM 50
#define DEF_NOM_SLIP 0.03
#define DEF_UVMAX 5.0
#define DEF_POLES 2
#define DEF_STATOR_SLOTES 36
#define DEF_ROTOR_BARS 24
#define DEF_VIBROTYPE 0
#define DEF_SENSORS 0x03
#define DEF_KVIBR 0.1
#define DEF_VIBRA_PRED 8.0
#define DEF_VIBRV_PRED 25.0
#define DEF_VIBRD_PRED 80.0
#define DEF_VIBRA_ALARM 9.85
#define DEF_VIBRV_ALARM 31.4
#define DEF_VIBRD_ALARM 100.0
#define DEF_VVIBRA_PRED 0.3
#define DEF_VVIBRV_PRED 1.0
#define DEF_VVIBRD_PRED 3.0
#define DEF_VVIBRA_ALARM 0.6
#define DEF_VVIBRV_ALARM 2.0
#define DEF_VVIBRD_ALARM 6.0
#define DEF_NUMA 1

#include "config.h"

class ConfigKDV
{
public:
    explicit ConfigKDV(S2DataTypes::S2ConfigType *config);

    typedef struct
    {
        quint32 Eq_type; //тип контролируемого оборудования: 0 - асинхронный
                         //двигатель, 1- трансформатор, 2 - реактор
        quint32 Cool_type; //вид охлаждения: 0 – естественное, 1 – принудительное
        quint32 W_mat;     //материал обмотки: 0 – медь, 1 – алюминий
        quint32 NFiltr;    // Интервал усреднения данных – 100 периодов
        quint32 NHarmFilt; // Постоянная времени фильтрации гармоник (3 цикла)
        quint32 DDosc;     // Разрешение запуска осциллограммы
                           // áèò 0 - îò ÖÊÓ, áèò 1 - ïî ñèãíàëó PD1, áèò 2 - ïî ñêà÷êó
                           // ðåæèìà.
        float Unom1;       // Номинальное линейное первичное напряжение, кВ
        float U2nom;       // Номинальное линейное вторичное напряжение ТН, В
        float ITT1nom;     // Номинальный первичный ток
        float ITT2nom;     // Номинальный вторичный ток , А
        float Iwnom;       //Номинальный ток контролируемой обмотки, À
        float DUosc; // Уставка скачка напряжения для запуска осциллографирования -
                     // % от номинала
        float DIosc; // Уставка скачка тока для запуска осциллографирования -  % от
                     // I2nom
        float DUImin; // Уставка порога минимального напряжения - % от номинального
                      // уровня
        float Imin; // Уставка порога минимального тока - % от номинального уровня
        float TNNTdop; // Предельно допустимая температура ННТ в°С
        float TNNTpred; // Уставка предупредительной сигнализации по температуре ННТ
                        // в °С
        float Tamb_nom; // Номинальная температура окружающей среды,  °С
        float dTNNTnom; // Превышение температуры ННТ при номинальной нагрузке, °С
        float Kdob; // Коэффициент добавочных потерь
        float TauWnom; // Постоянная времени нагрева обмотки в номинальном режиме, мин
        float Umax; // Максимальное измеряемое фазное напряжение на входе прибора, В
                    // эфф (не более 305В)
        float Imax; // Максимальный измеряемый ток на входе прибора, А эфф  (не
                    // более 33А)
        float GTnnt; // Гистерезис сигнализации по температуре ННТ,  град.С
        float GOvc; // Гистерезис сигнализации по токовой перегрузке, % от Iwnom
        float Fnom;     //номинальная частота, Гц
        float nom_slip; // номинальное скольжение
        float UVmax; // Максимальная амплитуда сигналов вибраций (1,25, 2,5, 5,0В)

        float Kvibr; // Коэффициент передачи датчиков вибрации
                     // для типа 0 - в В/(м/с/с), для типа 1 - в В/(мм/с), для типа
                     // 2 - в В/мкм.
        float VibrA_pred; // предупредительная уставка по СКЗ виброускорения, м/с/с
        float VibrV_pred; // предупредительная уставка по СКЗ виброскорости, мм/с
        float VibrD_pred; // предупредительная уставка по СКЗ виброперемещения, мкм
        float VibrA_alarm;  // аварийная уставка по СКЗ виброускорения, м/с/с
        float VibrV_alarm;  // аварийная уставка по СКЗ виброскорости, мм/с
        float VibrD_alarm;  // аварийная уставка по СКЗ виброперемещения, мкм
        float VVibrA_pred;  // предупредительная уставка по скорости роста СКЗ
                            // виброускорения, м/с/с
        float VVibrV_pred;  // предупредительная уставка по скорости роста СКЗ
                            // виброскорости, мм/с
        float VVibrD_pred;  // предупредительная уставка по скорости роста СКЗ
                            // виброперемещения, мкм
        float VVibrA_alarm; // аварийная уставка по скорости роста СКЗ
                            // виброускорения, м/с/с
        float VVibrV_alarm; // аварийная уставка по скорости роста СКЗ
                            // виброскорости, мм/с
        float VVibrD_alarm; // аварийная уставка по скорости роста СКЗ
                            // виброперемещения, мкм

        quint32 NumA;          // порядковый номер двигателя
        quint32 Poles;         //число пар полюсов (от 1 до 10)
        quint32 Stator_Slotes; // число пазов на статоре
                               // для асинхронных двигателей:
        quint32 Rotor_bars;    // число стержней ротора
        quint32 VibroType;     // Тип вибродатчика: 0 - виброускорение, 1 -
                               // виброскорость, 2 - виброперемещение.
        quint32 Sensors;       // Подключенные датчики вибрации:
                               // (отметки "1" в битах 0..5 для датчиков в каналах 1..6
        quint32 T_Data_Rec;    // Интервал записи данных в ПЗУ (тренд), с
        quint32 OscPoints; // Количество точек осциллограммы на период основной частоты
        quint32 TdatNum; // Количество датчиков температуры обмоток, подключенных по
                         // Modbus Master

    } Bci;

    Bci Bci_block;

    S2DataTypes::S2ConfigType *S2Config();
    void SetDefConf();

private:
    S2DataTypes::S2ConfigType *m_S2Config;
};

#endif // CONFIGKDV_H
