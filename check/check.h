#ifndef CHECK_H
#define CHECK_H

#include <QWidget>

#define BD_COMMON   0
#define BD_MEZONIN  100

class Check
{
public:

    struct Bd0
    {
        float Tmk;	// Температура кристалла микроконтроллера, °С
        float Vbat;	// Напряжение аккумуляторной батареи, В
    };

    Bd0 Bd_block0;

    Check();
    QWidget *Bd0W(QWidget *parent);
    void FillBd0(QWidget *parent);
};

#endif // CHECK_H
