#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include <QDialog>

#include "s_tqcombobox.h"
#include "publicclass.h"

// Описание блока Bci
#define BCI_INTYPE      1
#define BCI_INMIN       2
#define BCI_INMAX       3
#define BCI_INVMIN      4
#define BCI_INVMAX      5
#define BCI_SETMINMIN   6
#define BCI_SETMIN      7
#define BCI_SETMAX      8
#define BCI_SETMAXMAX   9
#define BCI_DISCOSC     10
#define BCI_CTYPE       11
#define BCI_ABS_104     12
#define BCI_CYCLE_104   13
#define BCI_T1_104      14
#define BCI_T2_104      15
#define BCI_T3_104      16
#define BCI_K_104       17

class confdialog : public QDialog
{
    Q_OBJECT
public:
    explicit confdialog(QWidget *parent = 0);

private:
    struct Bci
    {
        qint8 in_type[16];	// Тип входа №0…15 (см. ниже)
        float in_min[16]; 	// Минимальное значение диапазона изменения сигнала
        float in_max[16];	// Максимальное значение диапазона
        float in_vmin[16];	// Минимальное значение сигнала в инженерных единицах
        float in_vmax[16];	// Максимальное значение сигнала в инженерных единицах
        float setminmin[16];	// Значение аварийной уставки по минимальному значению
        float setmin[16];	// Значение предупредительной уставки по минимальному значению
        float setmax[16];	// Значение предупредительной уставки по максимальному значению
        float setmaxmax[16];	// Значение аварийной уставки по максимальному значению
        qint16 discosc;		// Набор признаков необходимости запуска осциллограмм
        qint16 Ctype;		// Тип синхронизации времени от модуля Ц
        qint32 Abs_104; 	// Адрес базовой станции для протокола 104
        qint32 Cycle_104;	// Интервал циклического опроса по протоколу МЭК 60870-5-104
        qint32 T1_104;		// тайм-аут Т1 для протокола 104
        qint32 T2_104;		// тайм-аут Т2 для протокола 104
        qint32 T3_104;		// тайм-аут Т3 для протокола 104
        qint32 k_104;		// макс. кол-во неподтв. сообщений
    };

    Bci *Bci_block;
//    publicclass::DataRec Config[];

    void GetBci();
    int BciUnpack();
    int BciPack();

signals:

public slots:

private slots:
    void SetChTypData(int, s_tqComboBox *);
    void FillConfData();
};

#endif // CONFDIALOG_H
