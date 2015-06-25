#ifndef IEC104_H
#define IEC104_H

#define I104_START          0x68

#define I104_I              0x00
#define I104_S              0x01
#define I104_U              0x03

#define I104_STARTDT_ACT    0x07
#define I104_STARTDT_CON    0x0B
#define I104_STOPDT_ACT     0x13
#define I104_STOPDT_CON     0x23
#define I104_TESTFR_ACT     0x43
#define I104_TESTFR_CON     0x83

// определения возвращаемого значения функции isIncomeDataValid
#define I104_RCVNORM        0x00
#define I104_RCVSMSIZE      0x01
#define I104_RCVWRONG       0x02

#define I104_CMD_LISTEN  0 // нет текущих команд для МИП, режим ожидания
#define I104_CMD_START   1 // реализуется команда START_DT
//#define CMD_

#include <QObject>
#include <QTimer>

class iec104 : public QObject
{
    Q_OBJECT

public:
    iec104(QObject *parent = 0);
    ~iec104();

    typedef struct
    {
        quint8 start;
        quint8 APDUlength;
        quint8 contrfield[4];
    } APCI;

    typedef QByteArray ASDU;

public slots:
    void Send(APCI, ASDU=QByteArray());
    void Start();
    void ParseIncomeData(QByteArray);
    int isIncomeDataValid(QByteArray);

signals:
    void writedatatoeth(QByteArray);
    void stopall();
    void error(int);

private:
    int cmd;
    quint8 APDULength;
    quint8 APDUFormat;
    QTimer *TTimer;
    quint16 V_S, V_R, Ack;
    QByteArray ReadData;

private slots:
    void CanalError(int);
};

#endif // IEC104_H
