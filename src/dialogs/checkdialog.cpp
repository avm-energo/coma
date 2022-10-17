#include "checkdialog.h"

#include "../module/board.h"
#include "../module/configstorage.h"
#include "../s2/s2helper.h"
#include "../widgets/wd_func.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTime>
#include <QVBoxLayout>
#include <QXlsx/xlsxdocument.h>
#include <gen/colors.h>
#include <gen/datamanager/typesproxy.h>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <set>
#include <utility>

constexpr auto circleRadius = 12;
constexpr auto normalColor = Qt::gray;
constexpr auto activeColor = Qt::yellow;

CheckDialog::CheckDialog(const ModuleTypes::Section &section, QWidget *parent)
    : UDialog(parent), mSection(section), mTabs(ConfigStorage::GetInstance().getModuleSettings().getTabs())
{
    XlsxWriting = false;
    m_readDataInProgress = false;
    xlsx = nullptr;
    WRow = 0;
    Timer = new QTimer(this);
    Timer->setObjectName("checktimer");
    connect(Timer, &QTimer::timeout, this, &CheckDialog::TimerTimeout);
    Timer->setInterval(1000);
    Timer->setInterval(ANMEASINT);
}

CheckDialog::~CheckDialog()
{
    qDeleteAll(Bd_blocks);
    Bd_blocks.clear();
}

void CheckDialog::SetBd(int bdnum, void *block, int blocksize, bool toxlsx)
{
    auto bdblock = new BdBlocks;
    bdblock->block = block;
    bdblock->blocknum = blocksize;
    bdblock->toxlsxwrite = toxlsx;
    Bd_blocks[bdnum] = bdblock;
}

QWidget *CheckDialog::BottomUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl = new QLabel("Период обновления данных измерения, сек:");
    hlyout->addWidget(lbl);
    QRadioButton *rb = new QRadioButton;
    rb->setObjectName("1000");
    rb->setText("1");
    connect(rb, &QAbstractButton::clicked, this, &CheckDialog::SetTimerPeriod);
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("2000");
    rb->setText("2");
    rb->setChecked(true);
    connect(rb, &QAbstractButton::clicked, this, &CheckDialog::SetTimerPeriod);
    hlyout->addWidget(rb);
    rb = new QRadioButton;
    rb->setObjectName("10000");
    rb->setText("10");
    connect(rb, &QAbstractButton::clicked, this, &CheckDialog::SetTimerPeriod);
    hlyout->addWidget(rb);
    lyout->addLayout(hlyout);

    QPushButton *pb = new QPushButton("Запустить чтение аналоговых сигналов");
    pb->setObjectName("pbmeasurements");
    connect(pb, &QAbstractButton::clicked, this, &CheckDialog::StartAnalogMeasurements);

    lyout->addWidget(pb);
    pb = new QPushButton("Запустить чтение аналоговых сигналов в файл");
    pb->setObjectName("pbfilemeasurements");
    connect(pb, &QAbstractButton::clicked, this, &CheckDialog::StartAnalogMeasurementsToFile);

    lyout->addWidget(pb);
    pb = new QPushButton("Остановить чтение аналоговых сигналов");
    connect(pb, &QAbstractButton::clicked, this, &CheckDialog::StopAnalogMeasurements);

    lyout->addWidget(pb);
    w->setLayout(lyout);
    return w;
}

void CheckDialog::setHighlights(Highlight type, const HighlightMap &map)
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

void CheckDialog::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
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
                lbl->setStyleSheet("QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; "
                                   "background-color:"
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
                lbl->setStyleSheet("QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; "
                                   "background-color:"
                    + color + "; color : black; }");
            }
            else
            {
                lbl->setStyleSheet(ValuesFormat);
            }
        }
    }
}

void CheckDialog::StartAnalogMeasurementsToFile()
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

void CheckDialog::ReadAnalogMeasurementsAndWriteToFile()
{
    // получение текущих аналоговых сигналов от модуля
    if (m_readDataInProgress)
    {
        qCritical("Ещё не завершена предыдущая операция");
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
    WRow++;
    m_readDataInProgress = false;
}

void CheckDialog::uponInterfaceSetting()
{
    setupUI();
}

void CheckDialog::StartAnalogMeasurements()
{
    // PrepareAnalogMeasurements();
    Timer->start();
}

void CheckDialog::StopAnalogMeasurements()
{
    if (XlsxWriting)
    {
        if (xlsx)
        {
            xlsx->save();
            QMessageBox::information(this, "Внимание", "Файл создан успешно");
            delete xlsx;
        }
        auto pb = this->findChild<QPushButton *>("pbfilemeasurements");
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

void CheckDialog::reqUpdate()
{
    if (updatesEnabled())
    {
        for (auto &bd : m_BdUIList)
            bd.widget->reqUpdate();
    }
}

void CheckDialog::TimerTimeout()
{
    ReadAnalogMeasurementsAndWriteToFile();
}

void CheckDialog::TWTabChanged(int index)
{
    if (index != -1)
    {
        for (auto &item : m_BdUIList)
            item.widget->engine()->setUpdatesEnabled(false);

        if (m_BdUIList.size() >= index)
        {
            auto w = m_BdUIList.at(index).widget;
            w->engine()->setUpdatesEnabled();
            w->reqUpdate();
        }
        else
            qCritical("Undefined check tab selected");
    }
}

void CheckDialog::SetTimerPeriod()
{
    bool TimerIsActive = false;
    if (Timer->isActive())
        TimerIsActive = true;
    bool ok;
    int per = sender()->objectName().toInt(&ok);
    if (!ok)
    {
        qCritical("Ошибка считывания интервала таймера");
        return;
    }
    Timer->stop();
    Timer->setInterval(per);
    if (TimerIsActive)
        Timer->start();
}

void CheckDialog::setupUI()
{
    const auto tabs = mSection.sgMap.uniqueKeys();
    for (auto &&tab : tabs)
    {
        auto widget = new UWidget(this);
        auto layout = new QVBoxLayout;
        const auto groups = mSection.sgMap.values(tab);
        for (auto &&group : groups)
        {
            auto groupBox = new QGroupBox(widget);
            groupBox->setTitle(group.name);
            groupBox->setLayout(setupGroup(group, widget));
            layout->addWidget(groupBox);
        }
        layout->addStretch(100);
        widget->setLayout(layout);
        addSignals(groups, widget);
        m_BdUIList.push_back({ mTabs.value(tab), widget });
    }
    m_BdUIList.first().widget->engine()->setUpdatesEnabled();
    setupUIAbs();
}

void CheckDialog::setupUIAbs()
{
    auto layout = new QVBoxLayout;
    auto checkTabWidget = new QTabWidget;
    for (auto &UIPair : m_BdUIList)
    {
        UIPair.widget->uponInterfaceSetting();
        checkTabWidget->addTab(UIPair.widget, " " + UIPair.widgetCaption + " ");
    }
    layout->addWidget(checkTabWidget);
    setLayout(layout);
    connect(checkTabWidget, &QTabWidget::currentChanged, this, &CheckDialog::TWTabChanged);
}

void CheckDialog::addSignals(const QList<ModuleTypes::SGroup> &groups, UWidget *widget)
{
    std::set<quint32> sigIds;
    auto &sigMap = ConfigStorage::GetInstance().getModuleSettings().getSignals();

    // Для каждой группы...
    for (auto &&group : groups)
    {
        // ... для каждого виджета...
        for (auto &&widget : group.widgets)
        {
            // ... среди сигналов ищем такой, чтобы...
            auto search = std::find_if(sigMap.cbegin(), sigMap.cend(),
                // ... виджет попадал в допустимый диапазон сигнала...
                [&](const ModuleTypes::Signal &signal) -> bool {
                    auto start = widget.startAddr;
                    auto end = (widget.view == ModuleTypes::ViewType::Bitset) ? (start + 1) : (start + widget.count);
                    auto acceptStart = signal.startAddr;
                    auto acceptEnd = acceptStart + signal.count;
                    return ((start >= acceptStart && start < acceptEnd) && (end > acceptStart && end <= acceptEnd));
                });
            // ... и когда находим...
            if (search != sigMap.cend())
                // ... то добавляем в множество неповторяющихся ключей.
                sigIds.insert(search.key());
        }
    }

    // Для каждого найденного ID добавляем обработчик сигнала из SignalMap в виджет
    for (auto it = sigIds.cbegin(); it != sigIds.cend(); it++)
    {
        auto &id = *it;
        auto signal = sigMap.value(id);
        switch (signal.sigType)
        {
        case ModuleTypes::SignalType::Float:
            widget->engine()->addFloat({ signal.startAddr, signal.count });
            break;
        case ModuleTypes::SignalType::SinglePointWithTime:
            widget->engine()->addSp({ signal.startAddr, signal.count });
            break;
        case ModuleTypes::SignalType::BitString:
            widget->engine()->addBs({ signal.startAddr, signal.count });
            break;
        default:
            qWarning() << "Unknown signal type: " << signal.sigType;
            break;
        }
    }
}

QString CheckDialog::getFormated(const ModuleTypes::MWidget &widget, const QString &form, const int &number)
{
    if (!widget.subItemList.empty() && number < widget.subItemList.count())
        return form.arg(widget.subItemList.at(number));
    else
        return (widget.count > 1) ? form.arg(number + 1) : form;
}

QVBoxLayout *CheckDialog::setupGroup(const ModuleTypes::SGroup &arg, UWidget *uwidget)
{
    auto groupLayout = new QVBoxLayout;
    for (auto &&mwidget : arg.widgets)
    {
        // Float
        if (mwidget.view == ModuleTypes::ViewType::Float)
        {
            auto widgetLayout = setupFloatWidget(mwidget, arg.widgets.count());
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
        // Bitset
        else if (mwidget.view == ModuleTypes::ViewType::Bitset)
        {
            auto widgetLayout = setupBitsetWidget(mwidget, uwidget);
            if (widgetLayout != nullptr)
            {
                groupLayout->addLayout(widgetLayout);
            }
        }
    }
    return groupLayout;
}

QGridLayout *CheckDialog::setupFloatWidget(const ModuleTypes::MWidget &mwidget, const int &wCount)
{
    auto gridLayout = new QGridLayout;
    auto &count = mwidget.count;
    auto itemsOneLine = StdFunc::goldenRatio(count);
    for (auto i = 0; i < count; ++i)
    {
        QBoxLayout *layout = nullptr;
        if (wCount == 1)
            layout = new QVBoxLayout;
        else
            layout = new QHBoxLayout;
        auto textLabel = new QLabel(getFormated(mwidget, mwidget.desc, i), this);
        layout->addWidget(textLabel);
        auto valueLabel = new QLabel(this);
        if (!mwidget.tooltip.isEmpty())
            valueLabel->setToolTip(getFormated(mwidget, mwidget.tooltip, i));

        valueLabel->setObjectName(QString::number(mwidget.startAddr + i));
        valueLabel->setStyleSheet(ValuesFormat);
        layout->addWidget(valueLabel);
        gridLayout->addLayout(layout, i / itemsOneLine, i % itemsOneLine);
    }
    return gridLayout;
}

void CheckDialog::updatePixmap(const ModuleTypes::MWidget &mwidget, DataTypes::BitStringStruct &bs, UWidget *uwidget)
{
    if (bs.sigAdr == mwidget.startAddr && bs.sigQuality == DataTypes::Quality::Good)
    {
        auto stringAddr = QString::number(mwidget.startAddr);
        std::bitset<sizeof(bs.sigVal) * 8> bitSet(bs.sigVal);
        for (auto i = 0; i < mwidget.count; ++i)
        {
            auto isSet = bitSet.test(i);
            auto pixmap = WDFunc::NewCircle((isSet) ? activeColor : normalColor, circleRadius);
            WDFunc::SetLBLImage(uwidget, stringAddr + "_" + QString::number(i), &pixmap);
        }
    }
}

QVBoxLayout *CheckDialog::setupBitsetWidget(const ModuleTypes::MWidget &mwidget, UWidget *widget)
{
    auto widgetLayout = new QVBoxLayout;
    auto bitsetWidget = new QWidget(this);
    bitsetWidget->setObjectName(QString::number(mwidget.startAddr));
    auto gridLayout = new QVBoxLayout;
    for (auto i = 0; i < mwidget.count; ++i)
    {
        auto layout = new QHBoxLayout;
        auto textLabel = new QLabel(mwidget.desc + ": " + QString::number(i) + " бит", this);
        layout->addWidget(textLabel);
        auto pixmap = WDFunc::NewCircle(normalColor, circleRadius);
        auto indicatorLabel = new QLabel(this);
        indicatorLabel->setObjectName(QString::number(mwidget.startAddr) + "_" + QString::number(i));
        indicatorLabel->setPixmap(pixmap);
        if (!mwidget.tooltip.isEmpty())
            indicatorLabel->setToolTip(mwidget.tooltip + ": " + QString::number(i) + " бит");
        layout->addWidget(indicatorLabel);
        gridLayout->addLayout(layout);
    }

    auto dataUpdater = widget->engine();
    connect(dataUpdater, &ModuleDataUpdater::itsTimeToUpdateBitStringSignal, this,
        [mwidget, widget, this](DataTypes::BitStringStruct &bs) -> void //
        { updatePixmap(mwidget, bs, widget); });

    bitsetWidget->setLayout(gridLayout);
    widgetLayout->addWidget(bitsetWidget);
    return widgetLayout;
}
