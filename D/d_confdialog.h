#ifndef D_CONFDIALOG_H
#define D_CONFDIALOG_H

#include <QDialog>

#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "../publicclass.h"
//#include "../canal.h"

// Описание блока Bci
#define DBCI_MTYPE       1
#define DBCI_MTYPE1      2
#define DBCI_INTYPE      3
#define DBCI_INMIN       4
#define DBCI_INMAX       5
#define DBCI_INVMIN      6
#define DBCI_INVMAX      7
#define DBCI_SETMINMIN   8
#define DBCI_SETMIN      9
#define DBCI_SETMAX      10
#define DBCI_SETMAXMAX   11
#define DBCI_DISCOSC     12
#define DBCI_OSCSRC      13
#define DBCI_OSCDLY      14
#define DBCI_CTYPE       15
#define DBCI_ABS_104     16
#define DBCI_CYCLE_104   17
#define DBCI_T1_104      18
#define DBCI_T2_104      19
#define DBCI_T3_104      20
#define DBCI_K_104       21
#define DBCI_W_104       22
#define DCONF_NUM       23

class d_confdialog : public QDialog
{
    Q_OBJECT
public:
    explicit d_confdialog(QWidget *parent = 0);

    bool NoProperConf; // в модуле нет нормальной конфигурации

private:
    typedef struct
    {
        quint32 MType;          // Тип модуля, для которого предназначена конфигурация
        quint32 MType1;         // Подтип модуля, для которого предназначена конфигурация
        quint16 Ctype;   		// Тип синхронизации времени от модуля Ц
        quint32 Abs_104;     	// Адрес базовой станции для протокола 104
        quint32 Cycle_104;      // Интервал циклического опроса по протоколу МЭК 60870-5-104
        quint32 T1_104;         // тайм-аут Т1 для протокола 104
        quint32 T2_104;         // тайм-аут Т2 для протокола 104
        quint32 T3_104;         // тайм-аут Т3 для протокола 104
        quint32 k_104;          // макс. кол-во неподтв. сообщений
        quint32 w_104;          // макс. кол-во сообщений, после которых необх. выдать подтверждение
        quint8 in_type[32];     // тип входа 0…15: 0 - вход не используется
                                // 1 – питание «замочки»
                                // 2 – неинвертированный сигнал без контроля целостности цепи
                                // 3 – инвертированный сигнал без контроля целостности цепи
                                // 4 – неинвертированный сигнал с контролем целостности цепи
                                // 5 – инвертированный сигнал с контролем целостности цепи
        quint32 dly_time1;      // маска дребезга 0,5 мс
        quint32 dly_time2;      // маска дребезга 1 мс
        quint32 dly_time3;      // маска дребезга 3 мс
        quint32 dly_time4;      // маска дребезга 20 мс
        quint32 pair[16];

    } Bci;

//    canal *cn;
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
    void SetupUI();
    void SetNewConf();
    void SetDefConf();
};

#endif // D_CONFDIALOG_H
