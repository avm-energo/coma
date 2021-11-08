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

#include "../gen/settings.h"
#include "../widgets/udialog.h"
#include "check.h"

#include <QBitArray>
#include <QElapsedTimer>
#include <QTimer>

// default timer interval to check analog values
constexpr int ANMEASINT = 2000;

// блоки Bd для базовой платы нумеруются с 0 (0-й блок - общий для всех)
constexpr int BT_STARTBD_BASE = 0;
// блоки Bd для мезонинной платы нумеруются с 101 (100 base + 1st block)
constexpr int BT_STARTBD_MEZ = 100;

namespace QXlsx
{
class Document;
}

class AbstractCheckDialog : public UDialog
{
    Q_OBJECT
public:
    struct BdUIStruct
    {
        QString widgetCaption;
        UWidget *widget;
    };
    enum Highlight
    {
        Warning,
        Critical
    };
    using HighlightMap = QMultiMap<quint32, quint32>;

    /*!
       \brief QList для вкладок текущего виджета

           QList для вкладок текущего виджета, по умолчанию обновление отключено
            для всех вкладок, если хотим обновлять какую-то вкладку сразу, то
            необходимо включить обновление для нее
    */

    explicit AbstractCheckDialog(QWidget *parent = nullptr);
    ~AbstractCheckDialog() override;
    void SetupUI();

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
    void setHighlights(Highlight type, const HighlightMap &map)
    {
        switch (type)
        {
        case Warning:

            m_highlightWarn = map;
            break;
        case Critical:

            m_highlightCrit = map;
            break;
        }
    }
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp) override;
signals:

public slots:
    void StopAnalogMeasurements();

    void reqUpdate() override;

private:
    struct BdBlocks
    {
        void *block;
        int blocknum;
        bool toxlsxwrite;
    };
    QMultiMap<quint32, quint32> m_highlightWarn, m_highlightCrit;
    QMap<int, BdBlocks *> Bd_blocks;

    bool m_readDataInProgress;
    QElapsedTimer *ElapsedTimeCounter;

    void ReadAnalogMeasurementsAndWriteToFile();

protected:
    QTimer *Timer;
    int WRow;
    QXlsx::Document *xlsx;
    QList<BdUIStruct> m_BdUIList;

    bool XlsxWriting;
    const QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
    void uponInterfaceSetting() override;

private slots:
    void SetTimerPeriod();
    void StartAnalogMeasurementsToFile();
    void StartAnalogMeasurements();
    void TimerTimeout();
    void TWTabChanged(int index);
};

class QGroupBox;

class CheckDialog : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialog(const CheckItem &item, const categoryMap &categories, QWidget *parent = nullptr);
    ~CheckDialog() override;

private:
    const CheckItem &m_item;
    const categoryMap &m_categories;

    void setupUI();

    void setup(const check::detail::Record &arg, QGroupBox *gb);
    void setup(const check::detail::RecordList &arg, QGroupBox *gb, size_t &rowCount);
};

#endif // ABSTRACTCHECKDIALOG_H
