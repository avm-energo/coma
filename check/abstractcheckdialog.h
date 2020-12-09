/* Абстрактный класс проверки работы модулей АВ-ТУК
 * Функции, которые необходимо реализовать при наследовании:
 * QWidget *AutoCheckUI() - реализация виджета автоматической проверки
 * работоспособности модуля (набор сообщений) QWidget *BdUI(int) - реализация
 * GUI для каждого блока данных, получаемого с модуля RefreshAnalogValues(int) -
 * обновление данных по полученному блоку Bd PrepareHeadersForFile(int) - запись
 * в указанной строке файла xlsx заголовков получаемых данных WriteToFile(int,
 * int) - запись в указанную строку полученных значений по блоку Bd При
 * наследовании необходимо в конструкторе предусмотреть следующее:
 *      - установить количество блоков данных в BdNum
 *      - для каждого блока данных, определённого в наследнике, требуется
 * создать элемент в Bd_blocks с указателем на блок и его длиной
 *      - в конце конструктора не забыть вызвать SetupUI()
 * */

#ifndef ABSTRACTCHECKDIALOG_H
#define ABSTRACTCHECKDIALOG_H

#include "../widgets/udialog.h"
//#include "../modbus/modbus.h"
#include "check.h"
#include "xlsxdocument.h"

#include <QBitArray>
#include <QElapsedTimer>
#include <QTimer>

// default timer interval to check analog values
#define ANMEASINT 2000

// блоки Bd для базовой платы нумеруются с 0 (0-й блок - общий для всех)
#define BT_STARTBD_BASE 0
// блоки Bd для мезонинной платы нумеруются с 101 (100 base + 1st block)
#define BT_STARTBD_MEZ 100

class AbstractCheckDialog : public UDialog
{
    Q_OBJECT
public:
    struct BdUIStruct
    {
        QString widgetCaption;
        UWidget *widget;
    };

    QXlsx::Document *xlsx;
    QTimer *Timer;
    int WRow;
    // количество вкладок с выводом блоков данных модуля, один блок может быть разделён на несколько вкладок
    //    int BdUINum;
    // тип платы
    //    QList<int> IndexWd;
    bool m_timerCounter;
    /*!
       \brief QList для вкладок текущего виджета

           QList для вкладок текущего виджета, по умолчанию обновление отключено
            для всех вкладок, если хотим обновлять какую-то вкладку сразу, то
            необходимо включить обновление для нее
    */
    QList<BdUIStruct> m_BdUIList;

    explicit AbstractCheckDialog(QWidget *parent = nullptr);
    ~AbstractCheckDialog();
    void SetupUI();

    // визуализация наборов текущих данных от модуля
    //    virtual QWidget *BdUI(int bdnum) = 0;
    // row - строка для записи заголовков
    virtual void PrepareHeadersForFile(int row);
    // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    virtual void WriteToFile(int row, int bdnum);

    //    virtual void ChooseValuesToWrite() = 0;
    //    virtual void SetDefaultValuesToWrite() = 0;
    // функция подготовки к измерениям (например,   запрос постоянных данных)
    void PrepareAnalogMeasurements();
    void SetBd(int bdnum, void *block, int blocksize, bool toxlsx = true);
    QWidget *BottomUI();

signals:

public slots:
    void StopAnalogMeasurements();
    //    virtual void SetWarnColor(int position, bool value) = 0;
    //    virtual void SetAlarmColor(int position, bool value) = 0;
    //    virtual void StartBdMeasurements();
    //    virtual void StopBdMeasurements();
    void reqUpdate() override;

private:
    struct BdBlocks
    {
        void *block;
        int blocknum;
        bool toxlsxwrite;
    };

    QMap<int, BdBlocks *> Bd_blocks;
    //    struct Bip
    //    {
    //        quint8 ip[4];
    //    };

    //    int m_newTWIndex;
    //    Bip Bip_block;
    bool m_readDataInProgress;
    QElapsedTimer *ElapsedTimeCounter;
    //  int m_currentTabIndex, m_oldTabIndex;

    //    void CheckIP();
    //    void GetIP();
    //    void Check1PPS();
    void ReadAnalogMeasurementsAndWriteToFile();

protected:
    Check *Ch;
    //    QTimer *BdTimer;
    //    int BdNum;
    bool XlsxWriting;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; "
                                 "color: blue; font: bold 10px;}";
    void uponInterfaceSetting() override;

private slots:
    void SetTimerPeriod();
    void StartAnalogMeasurementsToFile();
    void StartAnalogMeasurements();
    void TimerTimeout();
    void TWTabChanged(int index);

    // UWidget interface
};

#endif // ABSTRACTCHECKDIALOG_H
