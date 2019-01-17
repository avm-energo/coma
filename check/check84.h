#ifndef CHECK84_H
#define CHECK84_H
#include <QWidget>

#define C84_BDA_IN   1
#define C84_BDA_IN2  2

class Check_84
{
public:

    struct Bd1
    {
        quint32	NUM;		// номер
        quint32 Time;	    // время
        float Ueff[3];		// действующие значения напряжений по 1-й гармонике, кВ
        float Ieff[3];		// действующие значения токов по 1-й гармонике, мА
        float Frequency;		// частота, Гц
        float U0;			// напряжение нулевой последовательности
        float U1;			// напряжение прямой последовательности
        float U2; 			// напряжение обратной последовательности
        float I0; 			// ток нулевой последовательности
        float I1; 			// ток прямой последовательности
        float I2; 			// ток обратной последовательности
        float Cbush[3];		// емкости вводов, пФ
        float Tg_d[3];		// тангенсы дельта вводов, %
        float dCbush[3];		// изменение емкостей вводов, пФ
        float dTg_d[3];		// изменение тангенсов дельта вводов, %
        float Iunb;			// действующее значение тока небаланса, мА
        float Phy_unb;		// угол тока небаланса относительно тока ф.А, град.
        float Tmk;			// температура микроконтроллера, °С
        float Tamb;			// температура окружающей среды, °С
        quint32 res;	       // чтобы было 128 байт
    };
    Bd1 Bd_block1;

    Check_84();
    QWidget *Bd1W(QWidget *parent);
    QWidget *Bd2W(QWidget *parent);
    void FillBd1(QWidget *parent);
    void FillBd2(QWidget *parent);

private:
    QString ValuesFormat, WidgetFormat;
};

#endif // CHECK84_H
