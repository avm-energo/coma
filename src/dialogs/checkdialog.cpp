#include "checkdialog.h"

#include "../module/board.h"
#include "../module/configstorage.h"
#include "../widgets/wd_func.h"

#include <QGroupBox>
#include <QTabWidget>
#include <gen/colors.h>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <set>

constexpr auto maxIndicatorCountInRow = 10;
constexpr auto circleRadius = 12;
constexpr auto normalColor = Qt::gray;
constexpr auto activeColor = Qt::yellow;
constexpr auto defaultStyle = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
constexpr auto errStyle = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; "
                          "background-color: %1; color: black;}";

CheckDialog::CheckDialog(const ModuleTypes::Section &section, QWidget *parent) : UDialog(parent), mSection(section)
{
}

CheckDialog::~CheckDialog()
{
    qDeleteAll(Bd_blocks);
    Bd_blocks.clear();
}

void CheckDialog::setHighlights(Modules::AlarmType type, const HighlightMap &map)
{
    switch (type)
    {
    case Modules::AlarmType::Warning:
        m_highlightWarn = map;
        break;
    case Modules::AlarmType::Critical:
        m_highlightCrit = map;
        break;
    default:
        break;
    }
}

void CheckDialog::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    bool status = sp.sigVal;
    if (m_highlightCrit.contains(sp.sigAdr) && status)
    {
        const QList<HighlightMap::mapped_type> regs = m_highlightCrit.values(sp.sigAdr);
        const auto errorStyle = QString(errStyle).arg("red");
        for (const auto reg : qAsConst(regs))
        {
            auto label = findChild<QLabel *>(QString::number(reg));
            if (label)
                label->setStyleSheet(errorStyle);
        }
    }
    else if (m_highlightWarn.contains(sp.sigAdr))
    {
        const QList<HighlightMap::mapped_type> regs = m_highlightWarn.values(sp.sigAdr);
        const auto errorStyle = QString(errStyle).arg("yellow");
        for (const auto reg : qAsConst(regs))
        {
            auto label = findChild<QLabel *>(QString::number(reg));
            if (!label)
                continue;
            if (status)
                label->setStyleSheet(errorStyle);
            else
                label->setStyleSheet(defaultStyle);
        }
    }
}

void CheckDialog::uponInterfaceSetting()
{
    setupUI();
}

void CheckDialog::reqUpdate()
{
    if (updatesEnabled())
    {
        for (auto &tab : m_TabList)
            tab.widget->reqUpdate();
    }
}

void CheckDialog::tabChanged(int newIndex)
{
    if (newIndex >= 0 && newIndex < m_TabList.size())
    {
        // Disable updates on old tab
        m_TabList[currentTabIndex].widget->engine()->setUpdatesEnabled(false);
        // Enabling updates on new tab
        auto widget = m_TabList[newIndex].widget;
        widget->engine()->setUpdatesEnabled(true);
        widget->reqUpdate();
        // Update current tab index
        currentTabIndex = newIndex;
    }
    else
        qCritical("Undefined check tab selected");
}

void CheckDialog::setupUI()
{
    const auto tabIds = mSection.sgMap.uniqueKeys();
    for (auto &&tabId : tabIds)
    {
        auto widget = new UWidget(this);
        auto layout = new QVBoxLayout;
        const auto groups = mSection.sgMap.values(tabId);
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
        const auto &tabs = ConfigStorage::GetInstance().getModuleSettings().getTabs();
        m_TabList.push_back({ tabs.value(tabId), widget });
    }
    m_TabList.first().widget->engine()->setUpdatesEnabled();
    currentTabIndex = 0;
    setupTabWidget();
}

void CheckDialog::setupTabWidget()
{
    auto layout = new QVBoxLayout;
    auto checkTabWidget = new QTabWidget(this);
    for (auto &tab : m_TabList)
    {
        tab.widget->uponInterfaceSetting();
        checkTabWidget->addTab(tab.widget, " " + tab.widgetCaption + " ");
    }
    layout->addWidget(checkTabWidget);
    setLayout(layout);
    connect(checkTabWidget, &QTabWidget::currentChanged, this, &CheckDialog::tabChanged);
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

QString CheckDialog::getFormatted(const ModuleTypes::MWidget &widget, //
    const QString &form, const quint32 number, const quint32 start)
{
    if (!widget.subItemList.empty() && number < widget.subItemList.count())
        return form.arg(widget.subItemList.at(number));
    else
        return (widget.count > 1) ? form.arg(start + number) : form;
}

QVBoxLayout *CheckDialog::setupGroup(const ModuleTypes::SGroup &arg, UWidget *uwidget)
{
    auto groupLayout = new QVBoxLayout;
    for (auto &&mwidget : arg.widgets)
    {
        // Float
        if (mwidget.view == ModuleTypes::ViewType::Float)
        {
            auto widgetLayout = setupFloatWidget(mwidget, arg.widgets.size());
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
        // Bitset
        else if (mwidget.view == ModuleTypes::ViewType::Bitset)
        {
            auto widgetLayout = setupBitsetWidget(mwidget, uwidget);
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
    }
    return groupLayout;
}

QGridLayout *CheckDialog::setupFloatWidget(const ModuleTypes::MWidget &mwidget, const int wCount)
{
    auto gridLayout = new QGridLayout;
    auto count = mwidget.count;
    auto itemsOneLine = StdFunc::goldenRatio(count);
    for (auto i = 0; i < count; i++)
    {
        QBoxLayout *layout = nullptr;
        if (wCount == 1)
            layout = new QVBoxLayout;
        else
            layout = new QHBoxLayout;
        auto textLabel = new QLabel(getFormatted(mwidget, mwidget.desc, i), this);
        layout->addWidget(textLabel);
        auto valueLabel = new QLabel(this);
        if (!mwidget.tooltip.isEmpty())
            valueLabel->setToolTip(getFormatted(mwidget, mwidget.tooltip, i));

        valueLabel->setObjectName(QString::number(mwidget.startAddr + i));
        valueLabel->setStyleSheet(defaultStyle);
        layout->addWidget(valueLabel);
        gridLayout->addLayout(layout, i / itemsOneLine, i % itemsOneLine);
    }
    return gridLayout;
}

void CheckDialog::updatePixmap(
    const ModuleTypes::MWidget &mwidget, const DataTypes::BitStringStruct &bs, UWidget *uwidget)
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

QLabel *CheckDialog::createPixmapIndicator(const ModuleTypes::MWidget &mwidget, const quint32 index)
{
    // TODO: Must be member of mwidget
    constexpr auto startIndex = 0;

    auto pixmap = WDFunc::NewCircle(normalColor, circleRadius);
    auto indicatorLabel = new QLabel(this);
    indicatorLabel->setObjectName(QString::number(mwidget.startAddr) + "_" + QString::number(index));
    indicatorLabel->setPixmap(pixmap);
    if (!mwidget.tooltip.isEmpty())
        indicatorLabel->setToolTip(getFormatted(mwidget, mwidget.tooltip, index, startIndex));
    return indicatorLabel;
}

QVBoxLayout *CheckDialog::setupBitsetWidget(const ModuleTypes::MWidget &mwidget, UWidget *widget)
{
    auto widgetLayout = new QVBoxLayout;
    auto bitsetWidget = new QWidget(this);
    bitsetWidget->setObjectName(QString::number(mwidget.startAddr));
    auto gridLayout = new QVBoxLayout;

    // TODO: Must be member of mwidget
    constexpr auto startIndex = 0;

    for (auto i = 0; i < mwidget.count; i++)
    {
        auto layout = new QHBoxLayout;
        if (mwidget.desc.isEmpty())
        {
            // По 10 индикаторов в строке, если нет описания
            const auto limit = i + maxIndicatorCountInRow;
            for (; (i < mwidget.count) && (i < limit); i++)
                layout->addWidget(createPixmapIndicator(mwidget, i));
            i--;
        }
        else
        {
            // По 1 индикатору и описанию в строку, если оно есть
            auto textLabel = new QLabel(getFormatted(mwidget, mwidget.desc, i, startIndex), this);
            layout->addWidget(textLabel);
            layout->addWidget(createPixmapIndicator(mwidget, i));
        }
        gridLayout->addLayout(layout);
    }

    auto dataUpdater = widget->engine();
    connect(dataUpdater, &ModuleDataUpdater::itsTimeToUpdateBitStringSignal, this,
        [mwidget, widget, this](const DataTypes::BitStringStruct &bs) -> void //
        { updatePixmap(mwidget, bs, widget); });

    bitsetWidget->setLayout(gridLayout);
    widgetLayout->addWidget(bitsetWidget);
    return widgetLayout;
}
