#include "abstractcheckdialog.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"
//#include "xlsxdocument.h"
#include "../widgets/helper.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTime>
#include <QVBoxLayout>
#include <QXlsx/xlsxdocument.h>
//#include <QtMath>

AbstractCheckDialog::AbstractCheckDialog(QWidget *parent) : UDialog(parent)
{
    // m_newTWIndex = 0;
    XlsxWriting = false;
    m_readDataInProgress = false;
    xlsx = nullptr;
    WRow = 0;
    // m_oldTabIndex = m_currentTabIndex = 0;
    Timer = new QTimer(this);
    Timer->setObjectName("checktimer");
    connect(Timer, &QTimer::timeout, this, &AbstractCheckDialog::TimerTimeout);
    Timer->setInterval(1000);
}

AbstractCheckDialog::~AbstractCheckDialog()
{
    qDeleteAll(Bd_blocks);
    Bd_blocks.clear();
}

void AbstractCheckDialog::SetupUI()
{

    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *CheckTW = new QTabWidget;
    connect(&DataManager::GetInstance(), &DataManager::singlePointReceived, this, &AbstractCheckDialog::updateSPData);
    for (auto &w : m_BdUIList)
    {
        w.widget->uponInterfaceSetting();
        CheckTW->addTab(w.widget, " " + w.widgetCaption + " ");
        connect(&DataManager::GetInstance(), &DataManager::floatReceived, w.widget, &UWidget::updateFloatData,
            Qt::QueuedConnection);
    }

    lyout->addWidget(CheckTW);

    setLayout(lyout);
    connect(CheckTW, &QTabWidget::currentChanged, this, &AbstractCheckDialog::TWTabChanged);
}

void AbstractCheckDialog::PrepareHeadersForFile(int row)
{
    Q_UNUSED(row)
}

void AbstractCheckDialog::WriteToFile(int row, int bdnum)
{
    Q_UNUSED(row)
    Q_UNUSED(bdnum)
}

void AbstractCheckDialog::PrepareAnalogMeasurements()
{
}

void AbstractCheckDialog::SetBd(int bdnum, void *block, int blocksize, bool toxlsx)
{
    BdBlocks *bdblock = new BdBlocks;
    bdblock->block = block;
    bdblock->blocknum = blocksize;
    bdblock->toxlsxwrite = toxlsx;
    Bd_blocks[bdnum] = bdblock;
}

QWidget *AbstractCheckDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Период обновления данных измерения, сек:");
    hlyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("1000");
    rb->setText("1");
    connect(rb, &QAbstractButton::clicked, this, &AbstractCheckDialog::SetTimerPeriod);
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb, &QAbstractButton::clicked, this, &AbstractCheckDialog::SetTimerPeriod);
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb, &QAbstractButton::clicked, this, &AbstractCheckDialog::SetTimerPeriod);
    hlyout->addWidget(rb);
    lyout->addLayout(hlyout);

    QPushButton *pb = new QPushButton("Запустить чтение аналоговых сигналов");
    pb->setObjectName("pbmeasurements");
    connect(pb, &QAbstractButton::clicked, this, &AbstractCheckDialog::StartAnalogMeasurements);

    lyout->addWidget(pb);
    pb = new QPushButton("Запустить чтение аналоговых сигналов в файл");
    pb->setObjectName("pbfilemeasurements");
    connect(pb, &QAbstractButton::clicked, this, &AbstractCheckDialog::StartAnalogMeasurementsToFile);

    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение аналоговых сигналов");
    connect(pb, &QAbstractButton::clicked, this, &AbstractCheckDialog::StopAnalogMeasurements);

    lyout->addWidget(pb);
    w->setLayout(lyout);
    return w;
}

void AbstractCheckDialog::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    bool status = sp.sigVal;

    if (m_highlightCrit.contains(sp.sigAdr))
    {
        const QList<HighlightMap::mapped_type> regs = m_highlightCrit.values(sp.sigAdr);
        const QString color = "red";
        for (const auto reg : qAsConst(regs))
        {
            QLabel *lbl = findChild<QLabel *>(QString::number(reg));
            if (!lbl)
                continue;
            if (status)
            {
                lbl->setStyleSheet(
                    "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; background-color:"
                    + color + "; color : black; }");
            }
        }
    }
    else if (m_highlightWarn.contains(sp.sigAdr))
    {
        const QList<HighlightMap::mapped_type> regs = m_highlightWarn.values(sp.sigAdr);
        const QString color = "yellow";
        for (const auto reg : qAsConst(regs))
        {
            QLabel *lbl = findChild<QLabel *>(QString::number(reg));
            if (!lbl)
                continue;
            if (status)
            {
                lbl->setStyleSheet(
                    "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; background-color:"
                    + color + "; color : black; }");
            }
            else
            {
                lbl->setStyleSheet(ValuesFormat);
            }
        }
    }
}

void AbstractCheckDialog::StartAnalogMeasurementsToFile()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString Filename = dlg->getSaveFileName(this, "Сохранить данные", StdFunc::GetHomeDir(), "Excel files (*.xlsx)",
        Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    if (Filename == "")
    {
        ERMSG("Не задано имя файла");
        return; // !!! ошибка - не задано имя файла
    }
    if (Filename.split(".").last() != "xlsx")
        Filename += ".xlsx";
    // удаляем файл, если он есть
    QFile fn;
    fn.setFileName(Filename);
    if (fn.exists())
        fn.remove();
    XlsxWriting = true;
    xlsx = new QXlsx::Document(Filename);
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "Модуль" : "Прибор");

    xlsx->write(1, 1,
        QVariant(tmps + ": " + Board::GetInstance().moduleName() + " сер. ном. "
            + QString::number(Board::GetInstance().serialNumber(Board::BaseMezzAdd))));
    xlsx->write(2, 1, QVariant("Дата начала записи: " + QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(3, 1, QVariant("Время начала записи: " + QDateTime::currentDateTime().toString("hh:mm:ss")));
    xlsx->write(5, 1, QVariant("Дата и время отсчёта"));
    PrepareHeadersForFile(6); // в 6 ряду пишем заголовки
    WRow = 7;
    QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
    if (pb != nullptr)
        pb->setEnabled(false);
    pb = this->findChild<QPushButton *>("pbmeasurements");
    if (pb != nullptr)
        pb->setEnabled(false);
    ElapsedTimeCounter = new QElapsedTimer;
    ElapsedTimeCounter->start();
    StartAnalogMeasurements();
}

void AbstractCheckDialog::ReadAnalogMeasurementsAndWriteToFile()
{

    // получение текущих аналоговых сигналов от модуля
    if (m_readDataInProgress)
    {
        ERMSG("Ещё не завершена предыдущая операция");
        return;
    }

    m_readDataInProgress = true;
    if (XlsxWriting)
    {
        xlsx->write(WRow, 1, QVariant(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != nullptr)
        {
            int MSecs = ElapsedTimeCounter->elapsed();
            QString TimeElapsed = QTime::fromMSecsSinceStartOfDay(MSecs).toString("hh:mm:ss.zzz");
            pb->setText("Идёт запись: " + TimeElapsed);
        }
    }
    int bdkeyssize = Bd_blocks.size();
    for (int bdnum = 0; bdnum < bdkeyssize; ++bdnum)
    {
        if (Bd_blocks.value(bdnum)->toxlsxwrite)
        {
            if (XlsxWriting)
                WriteToFile(WRow, bdnum);
        }
    }

    WRow++;
    m_readDataInProgress = false;
}

void AbstractCheckDialog::uponInterfaceSetting()
{
    SetupUI();
}

// void AbstractCheckDialog::StartBdMeasurements() { BdTimer->start(); }

// void AbstractCheckDialog::StopBdMeasurements() { BdTimer->stop(); }

void AbstractCheckDialog::StartAnalogMeasurements()
{
    //    CurBdNum = 1;
    PrepareAnalogMeasurements();
    Timer->start();
}

void AbstractCheckDialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            QMessageBox::information(this, "Внимание", "Файл создан успешно");
            delete xlsx;
        }
        QPushButton *pb = this->findChild<QPushButton *>("pbfilemeasurements");
        if (pb != nullptr)
        {
            pb->setEnabled(true);
            pb->setText("Запустить чтение аналоговых сигналов в файл");
        }
        pb = this->findChild<QPushButton *>("pbmeasurements");
        if (pb != nullptr)
            pb->setEnabled(true);
        XlsxWriting = false;
    }
    Timer->stop();
}

void AbstractCheckDialog::reqUpdate()
{
    if (!updatesEnabled())
        return;
    for (auto &bd : m_BdUIList)
    {
        bd.widget->reqUpdate();
    }
}

void AbstractCheckDialog::TimerTimeout()
{
    ReadAnalogMeasurementsAndWriteToFile();
}

void AbstractCheckDialog::TWTabChanged(int index)
{
    if (index == -1)
        return;
    for (auto &item : m_BdUIList)
    {
        if (item.widget->updatesEnabled())
            item.widget->setUpdatesDisabled();
    }
    Q_ASSERT(m_BdUIList.size() >= index);
    if (m_BdUIList.size() < index)
        return;
    UWidget *w = m_BdUIList.at(index).widget;

    w->setUpdatesEnabled();
    w->reqUpdate();
}

void AbstractCheckDialog::SetTimerPeriod()
{
    bool TimerIsActive = false;
    if (Timer->isActive())
        TimerIsActive = true;
    bool ok;
    int per = sender()->objectName().toInt(&ok);
    if (!ok)
    {
        ERMSG("Ошибка считывания интервала таймера");
        return;
    }
    Timer->stop();
    Timer->setInterval(per);
    if (TimerIsActive)
        Timer->start();
}

CheckDialog::CheckDialog(const CheckSettings &settings, QWidget *parent)
    : AbstractCheckDialog(parent), m_settings(settings)
{
    Timer->setInterval(ANMEASINT);
    setupUI();
}

CheckDialog::~CheckDialog()
{
}

void CheckDialog::setupUI()
{
    QMultiMap<uint16_t, decltype(m_settings.items)::value_type> itemByGroup;
    for (auto &&item : m_settings.items)
    {
        std::visit(overloaded { [&](const check::detail::Record &arg) { itemByGroup.insert(arg.group.value(), arg); },
                       [&](const check::detail::RecordList &arg) { itemByGroup.insert(arg.group, arg); } },
            item);
    }
    auto keys = itemByGroup.uniqueKeys();
    for (auto &&key : keys)
    {
        UWidget *w = new UWidget;
        QVBoxLayout *lyout = new QVBoxLayout;
        auto values = itemByGroup.values(key);
        for (auto it = values.crbegin(); it != values.crend(); it++)
        //    for (auto &&value : itemByGroup.values(key))
        {
            //...//
            QGroupBox *gb = new QGroupBox();
            std::visit(overloaded { [&](const check::detail::Record &arg) { setup(arg, gb); },
                           [&](const check::detail::RecordList &arg) {
                               size_t rowCount = 0;
                               setup(arg, gb, rowCount);
                           } },
                *it);
            lyout->addWidget(gb);
        }
        lyout->addStretch(100);
        w->setLayout(lyout);
        m_BdUIList.push_back({ m_settings.categories.value(key), w });
    }
    m_BdUIList.first().widget->setUpdatesEnabled();
}

void CheckDialog::setup(const check::detail::Record &arg, QGroupBox *gb)
{
    gb->setTitle(arg.header.value());

    auto count = std::size(arg.desc.value());
    auto itemsOneLine = detail::goldenRatio(count);

    QGridLayout *gridlyout = new QGridLayout;
    for (auto i = 0; i != count; ++i)
    {
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(new QLabel(arg.desc.value().at(i)));
        QLabel *valueLabel = new QLabel;
        valueLabel->setObjectName(QString::number(arg.start_addr + i));
        valueLabel->setStyleSheet(ValuesFormat);
        layout->addWidget(valueLabel);
        gridlyout->addLayout(layout, i / itemsOneLine, i % itemsOneLine);
    }

    gb->setLayout(gridlyout);
}

void CheckDialog::setup(const check::detail::RecordList &arg, QGroupBox *gb, size_t &rowCount)
{
    gb->setTitle(arg.header);

    QGridLayout *gridlyout = new QGridLayout;

    //    for (auto &&record : arg.records)
    //    {
    //        auto itemsOneLine = detail::goldenRatio(record.desc->count());
    //        rowCount += (record.desc->count() / itemsOneLine);
    //    }
    int count = std::size(arg.records);
    for (int i = 0; i != count; ++i)
    {

        // one item per record
        const auto &currentRecord = arg.records.at(i);

        auto itemsOneLine = detail::goldenRatio(currentRecord.desc->count());
        for (int j = 0; j < currentRecord.desc->count(); ++j)
        {
            QHBoxLayout *layout = new QHBoxLayout;
            // layout->setStretchFactor()
            QLabel *textLabel = new QLabel(currentRecord.desc->at(j));
            // textLabel->setStyleSheet(ValuesFormat);
            textLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            // textLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            QFontMetrics fn(textLabel->font());
            textLabel->setMaximumHeight(fn.height());

            layout->addWidget(textLabel /*, 2, Qt::AlignLeft*/);

            QLabel *valueLabel = new QLabel;
            valueLabel->setMaximumHeight(fn.height());

            valueLabel->setObjectName(QString::number(currentRecord.start_addr + j));
            valueLabel->setStyleSheet(ValuesFormat);
            valueLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            layout->addWidget(valueLabel /*, 3, Qt::AlignLeft*/);
            auto currentRow = (i * j) / itemsOneLine;
            auto currentColumn = (i * j) % itemsOneLine;
            gridlyout->addLayout(layout, i, j);
        }
        //  }
    }
    gb->setLayout(gridlyout);
}
