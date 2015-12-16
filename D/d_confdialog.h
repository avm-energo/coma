#ifndef D_CONFDIALOG_H
#define D_CONFDIALOG_H

#include <QDialog>

#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqSpinBox.h"
#include "../publicclass.h"
//#include "../canal.h"

// Описание блока Bci
#define DBCI_MTYPE      1
#define DBCI_MTYPE1     2
#define DBCI_CTYPE      3
#define DBCI_ABS_104    4
#define DBCI_CYCLE_104  5
#define DBCI_T1_104     6
#define DBCI_T2_104     7
#define DBCI_T3_104     8
#define DBCI_K_104      9
#define DBCI_W_104      10
#define DBCI_INTYPE     11
#define DBCI_DLY1       12
#define DBCI_DLY2       13
#define DBCI_DLY3       14
#define DBCI_DLY4       15
#define DBCI_PAIR       16
#define DCONF_NUM       17

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
        quint32 dly_time2;      // маска дребезга 2 мс
        quint32 dly_time3;      // маска дребезга 8 мс
        quint32 dly_time4;      // маска дребезга 16 мс
        quint32 pair[16];

    } Bci;

    QByteArray confba;
    Bci Bci_block, Bci_defblock;

    publicclass::DataRec Config[DCONF_NUM];

    void FillConfData();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:
    void UpdateProper(bool);

private slots:
    void SetChTypData(int, s_tqComboBox *);
    void SetChOsc(int, s_tqCheckBox *);
    void SetChOscSrc(int, s_tqComboBox *);
    void SetInMin(double, s_tqSpinBox *);
    void SetInMax(double, s_tqSpinBox *);
    void SetInVMin(double, s_tqSpinBox *);
    void SetInVMax(double, s_tqSpinBox *);
    void SetMinMin(double, s_tqSpinBox *);
    void SetMin(double, s_tqSpinBox *);
    void SetMax(double, s_tqSpinBox *);
    void SetMaxMax(double, s_tqSpinBox *);
    void Set104(double, s_tqSpinBox *);
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void WriteConfDataToModule();
    void SetupUI();
    void SetNewConf();
    void SetDefConf();
};

#endif // D_CONFDIALOG_H
