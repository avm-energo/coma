#ifndef E_CONFDIALOG_H
#define E_CONFDIALOG_H

#include <QDialog>

#include "../widgets/s_tqcombobox.h"
#include "../widgets/s_tqcheckbox.h"
#include "../widgets/s_tqspinbox.h"
#include "../publicclass.h"
#include "../canal.h"

// Описание блока Bci
#define BCI_EQTYPE      1
#define BCI_NPOINTS     2
#define BCI_NFILTR      3
#define BCI_NHFILTR     4
#define BCI_DDOSC       5
#define BCI_UNOM1       6
#define BCI_UNOM2       7
#define BCI_INOM1       8
#define BCI_INOM2       9
#define BCI_DUOSC       10
#define BCI_DIOSC       11
#define BCI_DUIMIN      12
#define BCI_CTYPE       13
#define BCI_ABS_104     14
#define BCI_CYCLE_104   15
#define BCI_T1_104      16
#define BCI_T2_104      17
#define BCI_T3_104      18
#define BCI_K_104       19
#define BCI_W_104       20
#define ECONF_NUM       21

// Подтипы модуля Э
#define ET_2T0N         0
#define ET_1T1N         1
#define ET_0T2N         2

class e_confdialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_confdialog(QWidget *parent = 0);

private:
    typedef struct
    {
        quint32 eq_type;        // Тип контролируемого оборудования: 0 - 1фТАТ, 1 - 3фТАТ, 2 - 1фР, 3 - 3фР
        quint32 npoints;        // Количество точек оцифровки на период (64/80/128/256)
        quint32 nfiltr;         // Интервал усреднения данных (постоянная фильтрации)
        quint32 nhfiltr;        // Постоянная фильтрации гармоник
        quint32 ddosc;          // События-инициаторы запуска осциллографирования
        float unom1;            // Класс напряжения 1-й группы
        float unom2;            // Класс напряжения 2-й группы
        float inom1[6];         // Номинальный первичный ток внешнего ТТ по каналам
        float inom2[6];         // Номинальный вторичный ток внешнего ТТ по каналам
        float duosc;            // Уставка скачка напряжения для запуска осциллографирования (в % от номинального напряжения ТН)
        float diosc;            // Уставка скачка тока для запуска осциллографирования (в % от номинального тока ТТ)
        float duimin;           // Уставка контроля минимума сигналов (в %)
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
    Bci Bci_block;
    publicclass::DataRec Config[ECONF_NUM];

    void ShowErrMsg(int ermsg);

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
    void SetChTypData(int, s_tqComboBox *);
    void SetChOsc(int, s_tqCheckBox *);
    void SetChOscSrc(int, s_tqComboBox *);
    void SetThreshold(double, s_tqspinbox *);
    void SetCurrent(double, s_tqspinbox *);
    void SetSecCurrent(int, s_tqComboBox *);
    void SetEqType(int);
    void SetVoltageClass(int, s_tqspinbox *);
    void Set104(double, s_tqspinbox *);
    void SetCType(int);
    void SetOscDly(int);
    void GetBci();
    void FillConfData();
    void WriteConfDataToModule();
    void WriteCompleted();
    void SetupUI();
    void SetNewConf();
    void UpdateBsi();
};

#endif // E_CONFDIALOG_H
