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

#define BT_STARTBD_BASE 0 // блоки Bd для базовой платы нумеруются с 0 (0-й блок - общий для всех)
#define BT_STARTBD_MEZ  100 // блоки Bd для мезонинной платы нумеруются с 101 (100 base + 1st block)


class EAbstractCheckDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EAbstractCheckDialog(int board, QWidget *parent = 0);
    void SetupUI(QStringList &tabnames);

    virtual QWidget *BdUI(int bdnum) = 0; // визуализация наборов текущих данных от модуля
    virtual void RefreshAnalogValues(int bdnum) = 0; // обновление полей в GUI из полученного соответствующего Bd_block
    virtual void PrepareHeadersForFile(int row) = 0; // row - строка для записи заголовков
    virtual void WriteToFile(int row, int bdnum) = 0; // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    virtual void ChooseValuesToWrite() = 0;
    virtual void SetDefaultValuesToWrite() = 0;
    virtual void PrepareAnalogMeasurements() = 0; // функция подготовки к измерениям (например, запрос постоянных данных)
    virtual QWidget *CustomTab() = 0; // если требуется для модуля специфичный вывод данных
    void SetBd(int bdnum, void *block, int blocksize, bool toxlsx=true);
    QWidget *BottomUI();

    QXlsx::Document *xlsx;
    QTimer *timer;
    int WRow, BdUINum; // BdUINum - количество вкладок с выводом блоков данных модуля, один блок может быть разделён на несколько вкладок
    int Board; // тип платы

// CurBdNum,

signals:

public slots:

private:
    struct BdBlocks
    {
        void *block;
        int blocknum;
        bool toxlsxwrite;
    };

    QMap<int, BdBlocks *> Bd_blocks;
    struct Bip
    {
        quint8 ip[4];
    };

    Bip Bip_block;
    bool XlsxWriting, Busy;
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
