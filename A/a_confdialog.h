#ifndef A_CONFDIALOG_H
#define A_CONFDIALOG_H

#include <QDialog>

#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "../publicclass.h"
//#include "../canal.h"

// Описание блока Bci
#define ABCI_MTYPE       1
#define ABCI_MTYPE1      2
#define ABCI_INTYPE      101
#define ABCI_INMIN       102
#define ABCI_INMAX       103
#define ABCI_INVMIN      104
#define ABCI_INVMAX      105
#define ABCI_SETMINMIN   106
#define ABCI_SETMIN      107
#define ABCI_SETMAX      108
#define ABCI_SETMAXMAX   109
#define ABCI_DISCOSC     110
#define ABCI_OSCSRC      111
#define ABCI_OSCDLY      112
#define ABCI_HYSTERESIS  113
#define ABCI_CTYPE       3
#define ABCI_ABS_104     4
#define ABCI_CYCLE_104   5
#define ABCI_T1_104      6
#define ABCI_T2_104      7
#define ABCI_T3_104      8
#define ABCI_K_104       9
#define ABCI_W_104       10
#define ACONF_NUM       24

#define ACONFER(a)       ERMSG(publicclass::ER_ACONF,__LINE__,a)
#define ACONFDBG         DBGMSG(publicclass::ER_ACONF,__LINE__)
#define ACONFWARN        WARNMSG(publicclass::ER_ACONF,__LINE__)
#define ACONFINFO(a)     INFOMSG(publicclass::ER_ACONF,__LINE__,a)

class a_confdialog : public QDialog
{
    Q_OBJECT
public:
    explicit a_confdialog(QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    typedef struct
    {
        quint32 MType;          // Тип модуля, для которого предназначена конфигурация
        quint32 MType1;         // Подтип модуля, для которого предназначена конфигурация
        qint8 in_type[16];      // Тип входа №0…15 (0 - не исп., 1 - мА, 2 - В, 3 - резерв)
        float in_min[16];       // Минимальное значение диапазона изменения сигнала
        float in_max[16];       // Максимальное значение диапазона
        float in_vmin[16];      // Минимальное значение сигнала в инженерных единицах
        float in_vmax[16];      // Максимальное значение сигнала в инженерных единицах
        float setminmin[16];	// Значение аварийной уставки по минимальному значению
        float setmin[16];       // Значение предупредительной уставки по минимальному значению
        float setmax[16];       // Значение предупредительной уставки по максимальному значению
        float setmaxmax[16];	// Значение аварийной уставки по максимальному значению
        float hysteresis;       // Значение гистерезиса на сравнение с уставками в % от макс. знач.
        quint32 discosc;		// Набор признаков необходимости запуска осциллограмм
        quint32 oscsrc;         // События-инициаторы запуска осциллографирования
        quint16 oscdly;         // Задержка в мс начала фиксации максимумов
        quint16 Ctype;   		// Тип синхронизации времени от модуля Ц
        quint32 Abs_104;     	// Адрес базовой станции для протокола 104
        quint32 Cycle_104;      // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104;         // тайм-аут Т1 для протокола 104
        quint32 T2_104;         // тайм-аут Т2 для протокола 104
        quint32 T3_104;         // тайм-аут Т3 для протокола 104
        quint32 k_104;          // макс. кол-во неподтв. сообщений
        quint32 w_104;          // макс. кол-во сообщений, после которых необх. выдать подтверждение
    } Bci;

    QByteArray confba;
    Bci Bci_block, Bci_defblock;
    publicclass::DataRec Config[ACONF_NUM];

    void FillConfData();
    int GetIndexFromName(QObject *obj);
    bool CheckConf();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();
    void ChannelDisabled(int, bool);

public slots:
    void UpdateProper(bool);

private slots:
    void SetChTypData();
    void SetChOsc(int, s_tqCheckBox *);
    void SetChOscSrc(int, s_tqComboBox *);
    void SetInMin();
    void SetInMax();
    void SetInVMin();
    void SetInVMax();
    void SetMinMin();
    void SetMin();
    void SetMax();
    void SetMaxMax();
    void SetRangemA();
    void SetRangeV();
    void Set104();
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void WriteConfDataToModule();
    void SetupUI();
    void SetNewConf();
    void SetDefConf();
    void SaveConf();
    void LoadConf();
    void DisableChannel(int ChNum, bool Disable);
};

#endif // A_CONFDIALOG_H
