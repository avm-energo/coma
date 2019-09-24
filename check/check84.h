#ifndef CHECK84_H
#define CHECK84_H
#include <QWidget>
#include <QLineEdit>
#include "../gen/maindef.h"
#include "eabstractcheckdialog.h"

#define C84_BDA_IN   1
#define C84_BDA_IN2  2

class Check_84 : public EAbstractCheckDialog
{
     Q_OBJECT
public:
     explicit Check_84(BoardTypes board, QWidget *parent);
     ~Check_84();

    QDialog *ask;
    QLineEdit *ledit;

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

    //Check_84();
    QWidget *Bd1W(QWidget *parent);
    QWidget *Bd2W(QWidget *parent);
    QWidget *Bd3W(QWidget *parent);
    void FillBd(QWidget *parent);
    //void FillBd2(QWidget *parent);
    //void FillBd3(QWidget *parent);
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    QWidget *CustomTab();
#if PROGSIZE != PROGSIZE_EMUL
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
    void PrepareAnalogMeasurements();
#endif

private:
    QString ValuesFormat, WidgetFormat;

public slots:
    void CalcTginit(void);
    void EnterTg();

#if PROGSIZE != PROGSIZE_EMUL
private slots:
void SendErt(void);
#endif

};


#endif // CHECK84_H
