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
        quint32 MTypeB;
        quint32 MTypeE;
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
        quint32 NFiltr;
        quint32 NHarmFilt;
        quint32 DDosc;
        float Unom1;
        float DUosc;
        float DIosc;
        float DUImin;
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
        quint8		IP[4];
        quint8		Mask[4];
        quint8  	GW[4];
        quint16		Port[4];
        quint8		SNTP[4];
        quint32		Baud;
        quint8		Parity;
        quint8		Stopbit;
        quint8      adrMB;
        quint8      IsNTP;
        quint8      IsPPS;

    } Com;

    Bci_Main MainBlk;   // Основной блок (см. config.h)
    Com Com_param;
    Bci Bci_block;

    void SetDefConf();


};

#endif // CONFIGKTF_H
