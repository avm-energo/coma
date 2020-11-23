#ifndef CONFIGKXX_H
#define CONFIGKXX_H

#include "../gen/datatypes.h"

#define BCI_IP 20
#define BCI_MASK 21
#define BCI_GW 22
#define BCI_PORT 23
#define BCI_SNTP 24
#define BCI_BAUD 25
#define BCI_PARITY 26
#define BCI_STOPBIT 27
#define BCI_ADRMB 28

//#define DEF_IP 172, 16, 29, 12
//#define DEF_MASK 255, 255, 252, 0
//#define DEF_GW 172, 16, 29, 1
//#define DEF_PORT 2404, 2405, 502, 502
//#define DEF_SNTP 172, 16, 31, 220
#define DEF_BAUD 9600
#define DEF_PARITY 0
#define DEF_STOPBIT 0
#define DEF_ADRMB 0
//#define DEF_ISNTP 0
//#define DEF_ISPPS 0

#define BCI_RTERM 1027
#define BCI_W100 1028

#define DEF_RTERM 100
#define DEF_W100 1.385f

#define BCI_TRELE_PRED 1037
#define BCI_TRELE_ALARM 1038

#define DEF_TRELE_PRED 0
#define DEF_TRELE_ALARM 0

#define BCI_MBMASTER 1060
#define BCI_MBMAB1 1061
#define BCI_MBMAB2 1062
#define BCI_MBMAB3 1063
#define BCI_MBMAB4 1064

#define DEF_MBMASTER 0

class ConfigKxx : public QObject
{
    Q_OBJECT
public:
    ConfigKxx(S2DataTypes::S2ConfigType *config, QObject *parent = nullptr);

    struct TempConfStruct
    {
        float RTerm;
        float W100;
    };
    TempConfStruct TempConf;

    typedef struct
    {
        quint32 Trele_pred;
        quint32 Trele_alarm;

    } StructTrele;
    StructTrele StrTrele;

    typedef struct
    {
        quint8 MBMaster;
        quint8 MBMab1[8];
        quint8 MBMab2[8];
        quint8 MBMab3[8];
        quint8 MBMab4[8];
    } StructModBus;
    StructModBus StrModBus;

    typedef struct
    {
        quint8 IP[4];
        quint8 Mask[4];
        quint8 GateWay[4];
        quint16 Port[4];
        quint8 SNTP[4];
        quint32 Baud;
        quint8 Parity;
        quint8 Stopbit;
        quint8 adrMB;
    } Com;
    Com Com_param;

    void setConfig(S2DataTypes::S2ConfigType *config);
    void SetDefConf();
    void Fill();
    void FillBack();
    QWidget *ComParam(QWidget *parent);
    QWidget *ModbusWidget(QWidget *parent);
    QWidget *VariousWidget(QWidget *parent);

private:
    QObject *ParentMB, *ParentSetup, *ParentSetupBl;
    QStringList m_constSBaudList { "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200" };
    QList<QWidget *> WidgetList;

private slots:
    void ChangeModbusGUI(int num);
};

#endif // CONFIGKXX_H
