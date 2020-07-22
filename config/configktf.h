#ifndef CONFIGKTF_H
#define CONFIGKTF_H


//#define ID8084_START    1012
#define ID_COM    20

#include "config.h"
#include "configdefktf.h"
#include "../gen/modulebsi.h"


class ConfigKTF
{
public:

     explicit ConfigKTF(QVector<S2::DataRec> *config);



    typedef struct
    {
        quint32 MTypeB_ID;
        quint32 MTypeE_ID;
        quint32 Ctype;   		// Тип синхронизации времени от модуля Ц
        quint32 Abs_104;     	// Адрес базовой станции для протокола 104
        quint32 Cycle_104;      // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104;         // тайм-аут Т1 для протокола 104
        quint32 T2_104;         // тайм-аут Т2 для протокола 104
        quint32 T3_104;         // тайм-аут Т3 для протокола 104
        quint32 k_104;          // макс. кол-во неподтв. сообщений
        quint32 w_104;          // макс. кол-во сообщений, после которых необх. выдать подтверждение
    } Bci_Main;

    typedef struct
    {

        quint32 NFiltr_ID;
        quint32 NHarmFilt_ID;
        quint32 DDosc_ID;
        float Unom1;
        float DUosc;
        float DIosc_ID;
        float DUImin_ID;
        float Imin;
        float RTerm;
        float W100;
        quint32 T_Data_Rec;
        quint32 Trele_pred;
        quint32 Trele_alarm;
        float U2nom;
        float ITT1nom;
        float ITT2nom;
        quint8 MBMaster;
        quint8 MBMab1[4];
        quint8 MBMab2[4];
        quint8 MBMab3[4];
        quint8 MBMab4[4];
        quint32 Cool_type;
        quint32 W_mat;
        float TNNTdop;
        float TNNTpred;
        float Tamb_nom;
        float dTNNTnom;
        float Kdob;
        float TauWnom;
        float Umaxm;
        float Imaxm;
        float Iwnom;
        quint32 OscPoints;
        float GTnnt;
        float GOvc;
        quint32 TdatNum;

    } Bci;

    typedef struct
    {
        quint8		IP_ID[4];
        quint8		Mask_ID[4];
        quint8  	GW_ID[4];
        quint16		Port_ID[4];
        quint8		SNTP_ID[4];
        quint32		Baud_ID;
        quint8		Parity_ID;
        quint8		Stopbit_ID;
        quint8      adrMB_ID;
        quint8      IsNTP_ID;
        quint8      IsPPS_ID;

    } Com;

    Bci_Main MainBlk;   // Основной блок (см. config.h)
    Com Com_param;
    Bci Bci_block;

    void SetDefConf();


};

#endif // CONFIGKTF_H
