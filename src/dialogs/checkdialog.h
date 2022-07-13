/* Абстрактный класс проверки работы модулей АВ-ТУК
 * Функции, которые необходимо реализовать при наследовании:
 * - QWidget *AutoCheckUI() - реализация виджета автоматической проверки работоспособности модуля (набор сообщений);
 * - QWidget *BdUI(int) - реализация GUI для каждого блока данных, получаемого с модуля RefreshAnalogValues(int) -
 *   обновление данных по полученному блоку;
 * - Bd PrepareHeadersForFile(int) - запись в указанной строке файла xlsx заголовков получаемых данных;
 * - WriteToFile(int, int) - запись в указанную строку полученных значений по блоку Bd.
 * При наследовании необходимо в конструкторе предусмотреть следующее:
 *  - установить количество блоков данных в BdNum;
 *  - для каждого блока данных, определённого в наследнике, требуется создать элемент в Bd_blocks
 *    с указателем на блок и его длиной;
 *  - в конце конструктора не забыть вызвать SetupUI().
 */

#ifndef CHECKDIALOG_H
#define CHECKDIALOG_H

#include "../s2/s2settings.h"
#include "../widgets/udialog.h"
// #include "check.h"

#include <QBitArray>
#include <QElapsedTimer>
#include <QGroupBox>
#include <QTimer>

// default timer interval to check analog values
constexpr int ANMEASINT = 2000;

namespace QXlsx
{
class Document;
}

class CheckDialog : public UDialog
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

    explicit CheckDialog(const CheckItem &item, const categoryMap &categories, QWidget *parent = nullptr);
    ~CheckDialog() override;

    void setupUI();
    void setupUIAbs();

    // row - строка для записи заголовков
    // void PrepareHeadersForFile(int row);
    // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    // void WriteToFile(int row, int bdnum);
    // функция подготовки к измерениям (например,   запрос постоянных данных)
    // void PrepareAnalogMeasurements();

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
    void updateSPData(const QVariant &msg) override;

protected:
    /*! \brief QList для вкладок текущего виджета
     *  \details QList для вкладок текущего виджета, по умолчанию обновление отключено
     *  для всех вкладок, если хотим обновлять какую-то вкладку сразу, то
     *  необходимо включить обновление для нее
     */
    QList<BdUIStruct> m_BdUIList;
    QTimer *Timer;
    QXlsx::Document *xlsx;

    const CheckItem &m_item;
    const categoryMap &m_categories;
    static constexpr char ValuesFormat[]
        = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
    void addSignals(unsigned int key, UWidget *widget);
    bool XlsxWriting;
    void uponInterfaceSetting() override;

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
    int WRow;
    UniquePointer<DataTypesProxy> proxySP, proxyFS;

    void ReadAnalogMeasurementsAndWriteToFile();
    void setup(const check::detail::Record &arg, QGroupBox *gb);
    void setup(const check::detail::RecordList &arg, QGroupBox *gb);

public slots:
    void StopAnalogMeasurements();
    void reqUpdate() override;

private slots:
    void SetTimerPeriod();
    void StartAnalogMeasurementsToFile();
    void StartAnalogMeasurements();
    void TimerTimeout();
    void TWTabChanged(int index);
};

#endif // CHECKDIALOG_H
