#ifndef CONFIG1_H
#define CONFIG1_H

#include <QVector>
#include <QMap>
#include "../gen/s2.h"

/*
#define BCI_MTYPEB_ID      1
#define BCI_MTYPEM_ID      2
#define BCI_CTYPE       3
#define BCI_ABS_104     4
#define BCI_CYCLE_104   5
#define BCI_T1_104      6
#define BCI_T2_104      7
#define BCI_T3_104      8
#define BCI_K_104       9
#define BCI_W_104       10
*/

#define BCI_NFILTR_ID      1003
#define BCI_NHARMFILT_ID      1004
#define BCI_DDOCS_ID      1005
#define BCI_UNOM1       1006
#define BCI_DUOSC     1010
#define BCI_DIOSC_ID   1011
#define BCI_DUIMIN_ID      1012
#define BCI_IMIN      1013
#define BCI_RTERM      1027
#define BCI_W100       1028
#define BCI_T_DATA_REC       1031
#define BCI_TRELE_PRED      1037
#define BCI_TRELE_ALARM      1038
#define BCI_U2NOM       1050
#define BCI_ITT1NOM     1052
#define BCI_ITT2NOM   1053
#define BCI_MBMASTER      1060
#define BCI_MBMAB1      1061
#define BCI_MBMAB2      1062
#define BCI_MBMAB3       1063
#define BCI_MBMAB4       1064
#define BCI_COOL_TYPE   1070
#define BCI_W_MAT      1071
#define BCI_TNNTDOP      1072
#define BCI_TNNTPRED      1073
#define BCI_TAMB_NOM       1074
#define BCI_DTNNT_NOM       1075
#define BCI_KDOB      1076
#define BCI_TAUWNOM      1077
#define BCI_UMAXM       1078
#define BCI_IMAXM     1079
#define BCI_IWNOM   1080
#define BCI_OSCPOINTS      1081
#define BCI_GTNNT      1082
#define BCI_GOVC      1083
#define BCI_TDATNUM       1084


#define BCI_IP_ID      20
#define BCI_MASK_ID      21
#define BCI_GW_ID       22
#define BCI_PORT_ID     23
#define BCI_SNTP_ID      24
#define BCI_BAUD_ID      25
#define BCI_PARITY_ID    26
#define BCI_STOPBIT_ID   27
#define BCI_ADRMB_ID      28
#define BCI_ISNTP_ID      29
#define BCI_ISPPS_ID      30
//


//.............................
/*
#define DEF_MTYPEB      0xA2
#define DEF_MTYPEM      0x*7
#define DEF_CTYPE       1
#define DEF_ABS_104     205
#define DEF_CYCLE_104   5
#define DEF_T1_104      15
#define DEF_T2_104      10
#define DEF_T3_104      20
#define DEF_K_104       12
#define DEF_W_104       8
*/

#define DEF_NFILTR_ID      100
#define DEF_NHARMFILT_ID      3
#define DEF_DDOCS_ID      7
#define DEF_UNOM1       35
#define DEF_DUOSC     2
#define DEF_DIOSC_ID   2
#define DEF_DUIMIN_ID      0,5
#define DEF_IMIN      0,2
#define DEF_RTERM      100
#define DEF_W100       1.385
#define DEF_T_DATA_REC   300
#define DEF_TRELE_PRED      0
#define DEF_TRELE_ALARM      0
#define DEF_U2NOM       100
#define DEF_ITT1NOM     600
#define DEF_ITT2NOM   5
#define DEF_MBMASTER      0
#define DEF_MBMAB1      0,0,0,0
#define DEF_MBMAB2      0,0,0,0
#define DEF_MBMAB3       0,0,0,0
#define DEF_MBMAB4       0,0,0,0
#define DEF_COOL_TYPE   0
#define DEF_W_MAT      0
#define DEF_TNNTDOP      105
#define DEF_TNNTPRED      100
#define DEF_TAMB_NOM       25
#define DEF_DTNNT_NOM       70
#define DEF_KDOB      0,1
#define DEF_TAUWNOM      10
#define DEF_UMAXM       120
#define DEF_IMAXM     33
#define DEF_IWNOM   300
#define DEF_OSCPOINTS      256
#define DEF_GTNNT      2
#define DEF_GOVC      2
#define DEF_TDATNUM       0

#define DEF_IP_ID      172,16,29,12
#define DEF_MASK_ID      255,255,252,0
#define DEF_GW_ID       172,16,29,1
#define DEF_PORT_ID     24042405,502,502
#define DEF_SNTP_ID  172,16,31,220
#define DEF_BAUD_ID      0
#define DEF_PARITY_ID    0
#define DEF_STOPBIT_ID   1
#define DEF_ADRMB_ID      0
#define DEF_ISNTP_ID      0
#define DEF_ISPPS_ID   0

//.........................................


/*
// определение файлов
#define FILE_CONF   1 // configuration
#define FILE_FW     3 // firmware
#define FILE_CJ     17 // commutation journal
#define FILE_EJ     18 // events journal (12->62)
#define FILE_OSC    1000 // oscilloscope info
*/







#endif // CONFIG1_H
