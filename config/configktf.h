#ifndef CONFIGKTF_H
#define CONFIGKTF_H

//#define ID8084_START    1012
#define ID_COM 20

#define BCI_NFILTR 1003
#define BCI_NHARMFILT 1004
#define BCI_DDOCS 1005
#define BCI_UNOM1 1006
#define BCI_DUOSC 1010
#define BCI_DIOSC 1011
#define BCI_DUIMIN 1012
#define BCI_IMIN 1013
#define BCI_RTERM 1027
#define BCI_W100 1028
#define BCI_T_DATA_REC 1031
#define BCI_TRELE_PRED 1037
#define BCI_TRELE_ALARM 1038
#define BCI_U2NOM 1050
#define BCI_ITT1NOM 1052
#define BCI_ITT2NOM 1053
#define BCI_MBMASTER 1060
#define BCI_MBMAB1 1061
#define BCI_MBMAB2 1062
#define BCI_MBMAB3 1063
#define BCI_MBMAB4 1064
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
//.............................

#define DEF_NFILTR 100
#define DEF_NHARMFILT 3
#define DEF_DDOCS 7
#define DEF_UNOM1 35
#define DEF_DUOSC 2
#define DEF_DIOSC 2
#define DEF_DUIMIN 0.5
#define DEF_IMIN 0.2
#define DEF_RTERM 100
#define DEF_W100 1.385
#define DEF_T_DATA_REC 300
#define DEF_TRELE_PRED 0
#define DEF_TRELE_ALARM 0
#define DEF_U2NOM 100
#define DEF_ITT1NOM 600
#define DEF_ITT2NOM 5
#define DEF_MBMASTER 0
#define DEF_MBMAB1 0, 0, 0, 0
#define DEF_MBMAB2 0, 0, 0, 0
#define DEF_MBMAB3 0, 0, 0, 0
#define DEF_MBMAB4 0, 0, 0, 0
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

#include "../gen/modulebsi.h"
#include "config.h"

class ConfigKTF
{
public:
    explicit ConfigKTF(QVector<S2::DataRec> *config);

    typedef struct
    {
        quint32 MTypeB;
        quint32 MTypeE;
        quint32 Ctype; // Тип синхронизации времени от модуля Ц
        quint32 Abs_104; // Адрес базовой станции для протокола 104
        quint32 Cycle_104; // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104; // тайм-аут Т1 для протокола 104
        quint32 T2_104; // тайм-аут Т2 для протокола 104
        quint32 T3_104; // тайм-аут Т3 для протокола 104
        quint32 k_104; // макс. кол-во неподтв. сообщений
        quint32 w_104; // макс. кол-во сообщений, после которых необх. выдать подтверждение
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
        float Umax;
        float Imax;
        float Iwnom;
        quint32 OscPoints;
        float GTnnt;
        float GOvc;
        quint32 TdatNum;

    } Bci;

    typedef struct
    {
        quint8 IP[4];
        quint8 Mask[4];
        quint8 GW[4];
        quint16 Port[4];
        quint8 SNTP[4];
        quint32 Baud;
        quint8 Parity;
        quint8 Stopbit;
        quint8 adrMB;
        quint8 IsNTP;
        quint8 IsPPS;

    } Com;

    Bci_Main MainBlk; // Основной блок (см. config.h)
    Com Com_param;
    Bci Bci_block;

    void SetDefConf();
};

#endif // CONFIGKTF_H
