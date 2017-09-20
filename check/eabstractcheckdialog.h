/* Абстрактный класс проверки работы модулей АВ-ТУК
 * Функции, которые необходимо реализовать при наследовании:
 * QWidget *AutoCheckUI() - реализация виджета автоматической проверки работоспособности модуля (набор сообщений)
 * QWidget *BdUI(int) - реализация GUI для каждого блока данных, получаемого с модуля
 * RefreshAnalogValues(int) - обновление данных по полученному блоку Bd
 * PrepareHeadersForFile(int) - запись в указанной строке файла xlsx заголовков получаемых данных
 * WriteToFile(int, int) - запись в указанную строку полученных значений по блоку Bd
 * При наследовании необходимо в конструкторе предусмотреть следующее:
 *      - установить количество блоков данных в BdNum
 *      - для каждого блока данных, определённого в наследнике, требуется создать элемент в Bd_blocks с указателем на блок и его длиной
 *      - в конце конструктора не забыть вызвать SetupUI()
 * */

#ifndef EABSTRACTCHECKDIALOG_H
#define EABSTRACTCHECKDIALOG_H

#include <QDialog>
#include <QTimer>
#include "QtXlsx/xlsxdocument.h"

#define ANMEASINT   2000 // default timer interval to check analog values

class EAbstractCheckDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EAbstractCheckDialog(QWidget *parent = 0);
    virtual void SetupUI()=0;

    virtual void RefreshAnalogValues(int bdnum) = 0; // обновление полей в GUI из полученного соответствующего Bd_block
    virtual void PrepareHeadersForFile(int row) = 0; // row - строка для записи заголовков
    virtual void WriteToFile(int row, int bdnum) = 0; // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    virtual void ChooseValuesToWrite() = 0;
    virtual void SetDefaultValuesToWrite() = 0;
    void SetBd(int bdnum, void *block, int blocksize);
    QWidget *BottomUI();

    QXlsx::Document *xlsx;
    QTimer *timer;
    int WRow, BdNum, BdUINum; // BdNum - количество блоков данных модуля, BdUINum - количество вкладок с выводом блоков данных модуля
                                        // BdUINum >= BdNum, т.е. один блок может быть разделён на несколько вкладок
// CurBdNum,

signals:

public slots:

private:
    struct BdBlocks
    {
        void *block;
        int blocknum;
    };

    QList<BdBlocks *> Bd_blocks;
    struct Bip
    {
        quint8 ip[4];
    };

    Bip Bip_block;
    bool XlsxWriting;
    QTime *ElapsedTimeCounter;
//    QWidget *Parent;

    void CheckIP();
    void GetIP();
    void Check1PPS();
    void ReadAnalogMeasurementsAndWriteToFile();

private slots:
    void StartAnalogMeasurementsToFile();
    void StartAnalogMeasurements();
    void StopAnalogMeasurements();
    void SetTimerPeriod();
    void TimerTimeout();
};

#endif // EABSTRACTCHECKDIALOG_H
