#ifndef CONFDIALOG_H
#define CONFDIALOG_H

#include <QDialog>

#include "s_tqcombobox.h"

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
