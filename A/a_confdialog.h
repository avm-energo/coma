#ifndef A_CONFDIALOG_H
#define A_CONFDIALOG_H

#include <QDialog>

#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "../publicclass.h"
#include "../canal.h"

// Описание блока Bci
#define ABCI_MTYPE       1
#define ABCI_MTYPE1      2
#define ABCI_INTYPE      3
#define ABCI_INMIN       4
#define ABCI_INMAX       5
#define ABCI_INVMIN      6
#define ABCI_INVMAX      7
#define ABCI_SETMINMIN   8
#define ABCI_SETMIN      9
#define ABCI_SETMAX      10
#define ABCI_SETMAXMAX   11
#define ABCI_DISCOSC     12
#define ABCI_OSCSRC      13
#define ABCI_OSCDLY      14
#define ABCI_CTYPE       15
#define ABCI_ABS_104     16
#define ABCI_CYCLE_104   17
#define ABCI_T1_104      18
#define ABCI_T2_104      19
#define ABCI_T3_104      20
#define ABCI_K_104       21
#define ABCI_W_104       22
#define ACONF_NUM       23

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
        qint8 in_type[16];      // Тип входа №0…15 (см. ниже)
        float in_min[16];       // Минимальное значение диапазона изменения сигнала
        float in_max[16];       // Максимальное значение диапазона
        float in_vmin[16];      // Минимальное значение сигнала в инженерных единицах
        float in_vmax[16];      // Максимальное значение сигнала в инженерных единицах
        float setminmin[16];	// Значение аварийной уставки по минимальному значению
        float setmin[16];       // Значение предупредительной уставки по минимальному значению
        float setmax[16];       // Значение предупредительной уставки по максимальному значению
        float setmaxmax[16];	// Значение аварийной уставки по максимальному значению
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

    canal *cn;
    QByteArray confba;
    Bci Bci_block, Bci_defblock;
/*    static Bci Bci_defblock = {
        3,
        0x810001,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,
        10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
        50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,
        950,950,950,950,950,950,950,950,950,950,950,950,950,950,950,950,
        990,990,990,990,990,990,990,990,990,990,990,990,990,990,990,990,
        0,
        0,
        0,
        2,
        205,
        5,
        15,
        10,
        20,
        12,
        8
    }; */


    publicclass::DataRec Config[ACONF_NUM];

    void ShowErrMsg(int ermsg);
    void FillConfData();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:
    void UpdateProper(bool);

private slots:
    void SetChTypData(int, s_tqComboBox *);
    void SetChOsc(int, s_tqCheckBox *);
    void SetChOscSrc(int, s_tqComboBox *);
    void SetInMin(double, s_tqspinbox *);
    void SetInMax(double, s_tqspinbox *);
    void SetInVMin(double, s_tqspinbox *);
    void SetInVMax(double, s_tqspinbox *);
    void SetMinMin(double, s_tqspinbox *);
    void SetMin(double, s_tqspinbox *);
    void SetMax(double, s_tqspinbox *);
    void SetMaxMax(double, s_tqspinbox *);
    void Set104(double, s_tqspinbox *);
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void WriteConfDataToModule();
    void WriteCompleted();
    void SetupUI();
    void SetNewConf();
    void SetDefConf();
    void UpdateBsi();
    void CheckConfAndFill();
};

#endif // A_CONFDIALOG_H
