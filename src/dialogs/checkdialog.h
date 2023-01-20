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

#include "../module/modulesettings.h"
#include "../widgets/udialog.h"

#include <QBitArray>
#include <QElapsedTimer>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

/// \brief Default timer interval to check analog values
constexpr int ANMEASINT = 2000;

namespace QXlsx
{
class Document;
}

struct BdUIStruct
{
    QString widgetCaption;
    UWidget *widget;
};

class CheckDialog : public UDialog
{
    Q_OBJECT
public:
    using HighlightMap = QMultiMap<quint32, quint32>;

    explicit CheckDialog(const ModuleTypes::Section &mSection, QWidget *parent = nullptr);
    ~CheckDialog() override;
    void setupUI();
    void setupUIAbs();
    // void SetBd(int bdnum, void *block, int blocksize, bool toxlsx = true);
    // QWidget *BottomUI();
    void setHighlights(Modules::AlarmType type, const HighlightMap &map);
    void updateSPData(const DataTypes::SinglePointWithTimeStruct &sp) override;

protected:
    /*! \brief QList для вкладок текущего виджета
     *  \details QList для вкладок текущего виджета, по умолчанию обновление отключено
     *  для всех вкладок, если хотим обновлять какую-то вкладку сразу, то
     *  необходимо включить обновление для нее
     */
    QList<BdUIStruct> m_BdUIList;
    // QTimer *Timer;
    // QXlsx::Document *xlsx;

    const ModuleTypes::Section &mSection;
    const ModuleTypes::TabsMap &mTabs;

    static constexpr auto ValuesFormat
        = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
    void addSignals(const QList<ModuleTypes::SGroup> &groups, UWidget *widget);
    // bool XlsxWriting;
    void uponInterfaceSetting() override;

private:
    struct BdBlocks
    {
        void *block;
        int blocknum;
        bool toxlsxwrite;
    };

    HighlightMap m_highlightWarn, m_highlightCrit;
    QMap<int, BdBlocks *> Bd_blocks;
    // bool m_readDataInProgress;
    QElapsedTimer *ElapsedTimeCounter;
    // int WRow;

    // void ReadAnalogMeasurementsAndWriteToFile();
    QString getFormated(const ModuleTypes::MWidget &widget, const QString &form, const int &number);
    void updatePixmap(const ModuleTypes::MWidget &mwidget, DataTypes::BitStringStruct &bs, UWidget *uwidget);
    QLabel *createPixmapIndicator(const ModuleTypes::MWidget &mwidget, const quint32 &index);
    QVBoxLayout *setupGroup(const ModuleTypes::SGroup &arg, UWidget *uwidget);
    QGridLayout *setupFloatWidget(const ModuleTypes::MWidget &mwidget, const int &wCount);
    QVBoxLayout *setupBitsetWidget(const ModuleTypes::MWidget &mwidget, UWidget *dataUpdater);

public slots:
    // void StopAnalogMeasurements();
    void reqUpdate() override;

private slots:
    // void SetTimerPeriod();
    // void StartAnalogMeasurementsToFile();
    // void StartAnalogMeasurements();
    // void TimerTimeout();
    void TWTabChanged(int index);
};

#endif // CHECKDIALOG_H
