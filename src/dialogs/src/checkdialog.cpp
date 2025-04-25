#include "dialogs/checkdialog.h"

#include <device/current_device.h>
#include <gen/colors.h>
#include <gen/error.h>
#include <gen/stdfunc.h>
#include <widgets/chbfunc.h>
#include <widgets/graphfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/pbfunc.h>

#include <QGroupBox>
#include <QTabWidget>
#include <bitset>
#include <set>

using namespace Device::XmlDataTypes;

constexpr auto c_maxIndicatorCountInRow = 10;
constexpr auto c_circleRadius = 12;
constexpr auto c_normalColor = Qt::gray;
constexpr auto c_activeColor = Qt::yellow;
constexpr auto c_defaultStyle = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; }";
constexpr auto c_errStyle = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; font: bold; "
                            "background-color: %1; color: black;}";
const auto c_errorStyle = QString(c_errStyle).arg("red");
const auto c_warnStyle = QString(c_errStyle).arg("yellow");

CheckDialog::CheckDialog(const Section &section, Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent)
    , mSection(section)
{
    auto &settings = m_device->getConfigStorage()->getDeviceSettings();
    setHighlights(AlarmType::Critical, settings.getHighlights(AlarmType::Critical));
    setHighlights(AlarmType::Warning, settings.getHighlights(AlarmType::Warning));

    // collect all highlight regs
    QList<HighlightMap::mapped_type> regs = m_highlightWarn.values();
    for (const auto reg : std::as_const(regs))
        m_curHighlight[reg] = Highlights::CLEAN;
    regs = m_highlightCrit.values();
    for (const auto reg : std::as_const(regs))
        m_curHighlight[reg] = Highlights::CLEAN;
}

CheckDialog::~CheckDialog()
{
    qDeleteAll(Bd_blocks);
    Bd_blocks.clear();
}

void CheckDialog::setHighlights(AlarmType type, const HighlightMap &map)
{
    switch (type)
    {
    case AlarmType::Warning:
        m_highlightWarn = map;
        break;
    case AlarmType::Critical:
        m_highlightCrit = map;
        break;
    default:
        break;
    }
}

void CheckDialog::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    bool needUpdate = false;
    bool status = sp.sigVal;
    if (m_highlightWarn.contains(sp.sigAdr))
    {
        needUpdate = true;
        const QList<HighlightMap::mapped_type> regs = m_highlightWarn.values(sp.sigAdr);
        for (const auto reg : std::as_const(regs))
        {
            if (status)
                setYellow(reg);
            else
                clearYellow(reg);
        }
    }
    if (m_highlightCrit.contains(sp.sigAdr))
    {
        needUpdate = true;
        const QList<HighlightMap::mapped_type> regs = m_highlightCrit.values(sp.sigAdr);
        for (const auto reg : std::as_const(regs))
        {
            if (status)
                setRed(reg);
            else
                clearRed(reg);
        }
    }
    if (needUpdate)
        setHighlights();
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
    const auto &tabs = m_device->getConfigStorage()->getDeviceSettings().getTabs();
    const auto tabIds = mSection.sgMap.uniqueKeys();
    for (auto &&tabId : tabIds)
    {
        if (tabs.value(tabId).notDenied)
        {
            auto widget = new UWidget(m_device, this);
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
            m_TabList.push_back({ tabs.value(tabId).name, widget });
        }
    }
    if (!m_TabList.empty())
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

void CheckDialog::addSignals(const QList<SGroup> &groups, UWidget *widget)
{
    std::set<quint32> sigIds;
    auto &sigMap = m_device->getConfigStorage()->getDeviceSettings().getSignals();

    // Для каждой группы...
    for (auto &&group : groups)
    {
        // ... для каждого виджета...
        for (auto &&widget : group.widgets)
        {
            // ... среди сигналов ищем такой, чтобы...
            auto search = std::find_if(sigMap.cbegin(), sigMap.cend(),
                // ... виджет попадал в допустимый диапазон сигнала...
                [&](const Device::SigMapValue &element) -> bool
                {
                    auto &signal = element.second;
                    auto start = widget.startAddr;
                    auto end = (widget.view == ViewType::Bitset) ? (start + 1) : (start + widget.count);
                    auto acceptStart = signal.startAddr;
                    auto acceptEnd = acceptStart + signal.count;
                    return ((start >= acceptStart && start < acceptEnd) && (end > acceptStart && end <= acceptEnd));
                });
            // ... и когда находим...
            if (search != sigMap.cend())
                // ... то добавляем в множество неповторяющихся ключей.
                sigIds.insert(search->first);
        }
    }

    // Для каждого найденного ID добавляем обработчик сигнала из SignalMap в виджет
    for (auto it = sigIds.cbegin(); it != sigIds.cend(); it++)
    {
        auto &id = *it;
        auto signal = sigMap.at(id);
        switch (signal.sigType)
        {
        case SignalType::Float:
            widget->engine()->addFloat({ signal.startAddr, signal.count });
            break;
        case SignalType::SinglePointWithTime:
            widget->engine()->addSp({ signal.startAddr, signal.count });
            break;
        case SignalType::BitString:
            widget->engine()->addBs({ signal.startAddr, signal.count });
            break;
        default:
            qWarning() << "Unknown signal type: " << signal.sigType;
            break;
        }
    }
}

QString CheckDialog::getFormatted(const MWidget &widget, //
    const QString &form, const quint32 number, const quint32 start)
{
    if (!widget.subItemList.empty() && number < widget.subItemList.count())
        return form.arg(widget.subItemList.at(number));
    else
        return (widget.count > 1) ? form.arg(start + number) : form;
}

QVBoxLayout *CheckDialog::setupGroup(const SGroup &arg, UWidget *uwidget)
{
    auto groupLayout = new QVBoxLayout;
    for (auto &&mwidget : arg.widgets)
    {
        // Float
        if (mwidget.view == ViewType::Float)
        {
            auto widgetLayout = setupFloatWidget(mwidget, arg.widgets.size());
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
        // Bitset
        else if (mwidget.view == ViewType::Bitset)
        {
            auto widgetLayout = setupBitsetWidget(mwidget, uwidget);
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
        else if (mwidget.view == ViewType::SinglePoint)
        {
            auto widgetLayout = setupSinglePointWidget(mwidget, uwidget);
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
        else if ((mwidget.view == ViewType::Command) || (mwidget.view == ViewType::CommandWValue))
        {
            auto widgetLayout = setupCommandWidget(mwidget, arg.widgets.size(), mwidget.view == ViewType::Command);
            if (widgetLayout != nullptr)
                groupLayout->addLayout(widgetLayout);
        }
    }
    return groupLayout;
}

QGridLayout *CheckDialog::setupFloatWidget(const MWidget &mwidget, const int wCount)
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
        valueLabel->setStyleSheet(c_defaultStyle);
        layout->addWidget(valueLabel);
        gridLayout->addLayout(layout, i / itemsOneLine, i % itemsOneLine);
    }
    return gridLayout;
}

void CheckDialog::updatePixmap(const MWidget &mwidget, const DataTypes::BitStringStruct &bs, UWidget *uwidget)
{
    if (bs.sigAdr == mwidget.startAddr && bs.sigQuality == DataTypes::Quality::Good)
    {
        auto stringAddr = QString::number(mwidget.startAddr);
        std::bitset<sizeof(bs.sigVal) * 8> bitSet(bs.sigVal);
        for (auto i = 0; i < mwidget.count; ++i)
        {
            auto isSet = bitSet.test(i);
            auto pixmap = GraphFunc::NewCircle((isSet) ? c_activeColor : c_normalColor, c_circleRadius);
            LBLFunc::SetLBLImage(uwidget, stringAddr + "_" + QString::number(i), &pixmap);
        }
    }
}

void CheckDialog::updatePixmap(
    const Device::XmlDataTypes::MWidget &mwidget, const DataTypes::SinglePointWithTimeStruct &sp, UWidget *uwidget)
{
    if ((sp.sigAdr >= mwidget.startAddr) && (sp.sigAdr < (mwidget.startAddr + mwidget.count))
        && sp.sigQuality == DataTypes::Quality::Good)
    {
        if (sp.sigAdr == 1300)
            qDebug() << "1";
        auto pixmap = GraphFunc::NewCircle((sp.sigVal != 0) ? c_activeColor : c_normalColor, c_circleRadius);
        LBLFunc::SetLBLImage(uwidget, QString::number(sp.sigAdr), &pixmap);
    }
}

QLabel *CheckDialog::createPixmapIndicator(const MWidget &mwidget, const quint32 index)
{
    auto pixmap = GraphFunc::NewCircle(c_normalColor, c_circleRadius);
    auto indicatorLabel = new QLabel(this);
    indicatorLabel->setObjectName(QString::number(mwidget.startAddr) + "_" + QString::number(index));
    indicatorLabel->setPixmap(pixmap);
    if (!mwidget.tooltip.isEmpty())
        indicatorLabel->setToolTip(getFormatted(mwidget, mwidget.tooltip, index + 1, 0));
    return indicatorLabel;
}

QLabel *CheckDialog::createPixmapIndicator(const Device::XmlDataTypes::MWidget &mwidget, const QString name)
{
    auto pixmap = GraphFunc::NewCircle(c_normalColor, c_circleRadius);
    auto indicatorLabel = new QLabel(this);
    indicatorLabel->setObjectName(name);
    indicatorLabel->setPixmap(pixmap);
    return indicatorLabel;
}

QVBoxLayout *CheckDialog::setupBitsetWidget(const MWidget &mwidget, UWidget *widget)
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
        const auto limit = i + c_maxIndicatorCountInRow;
        for (; (i < mwidget.count) && (i < limit); i++)
        {
            auto textLabel = new QLabel(getFormatted(mwidget, mwidget.desc, i + 1, startIndex), this);
            layout->addWidget(textLabel);
            layout->addWidget(createPixmapIndicator(mwidget, i));
        }
        layout->addStretch(100);
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

QVBoxLayout *CheckDialog::setupSinglePointWidget(const MWidget &mwidget, UWidget *widget)
{
    auto widgetLayout = new QVBoxLayout;
    auto spWidget = new QWidget(this);
    auto vlyout = new QVBoxLayout;

    for (auto i = 0; i < mwidget.count; i++)
    {
        auto hlyout = new QHBoxLayout;
        const auto limit = i + c_maxIndicatorCountInRow;
        for (; (i < mwidget.count) && (i < limit); i++)
        {
            auto textLabel = new QLabel(getFormatted(mwidget, mwidget.desc, i + 1, 0), this);
            hlyout->addWidget(textLabel);
            auto indicator = createPixmapIndicator(mwidget, QString::number(mwidget.startAddr + i));
            indicator->setToolTip(getFormatted(mwidget, mwidget.tooltip, i + 1, 0));
            hlyout->addWidget(indicator);
        }
        hlyout->addStretch(100);
        vlyout->addLayout(hlyout);
    }

    auto dataUpdater = widget->engine();
    connect(dataUpdater, &ModuleDataUpdater::itsTimeToUpdateSinglePointSignal, this,
        [mwidget, widget, this](const DataTypes::SinglePointWithTimeStruct &sp) -> void //
        { updatePixmap(mwidget, sp, widget); });

    spWidget->setLayout(vlyout);
    widgetLayout->addWidget(spWidget);
    return widgetLayout;
}

QGridLayout *CheckDialog::setupCommandWidget(const Device::XmlDataTypes::MWidget &mwidget, const int wCount, bool type)
{
    auto gridLayout = new QGridLayout;
    auto count = mwidget.count;
    auto itemsOneLine = StdFunc::goldenRatio(count);
    for (auto i = 0; i < count; ++i)
    {
        int realAddr = mwidget.startAddr + i;
        QBoxLayout *layout = nullptr;
        if (wCount == 1)
            layout = new QVBoxLayout;
        else
            layout = new QHBoxLayout;
        auto textLabel = new QLabel(getFormatted(mwidget, mwidget.desc, i), this);
        layout->addWidget(textLabel);
        if (!type)
        {
            QCheckBox *cb = ChBFunc::NewChB(this, QString::number(realAddr), "");
            if (!mwidget.tooltip.isEmpty())
                cb->setToolTip(getFormatted(mwidget, mwidget.tooltip, i));
            layout->addWidget(cb);
        }
        layout->addWidget(PBFunc::NewPB(this, "", "Послать",
            [&, realAddr, type]()
            {
                bool cbdata = false;
                DataTypes::SingleCommand cmd;
                cmd.addr = realAddr;
                if (!type)
                    cbdata = ChBFunc::ChBData(this, QString::number(realAddr));
                cmd.value = cbdata;
            }));

        gridLayout->addLayout(layout, i / itemsOneLine, i % itemsOneLine);
    }
    return gridLayout;
}

void CheckDialog::setYellow(quint32 reg)
{
    if (m_curHighlight[reg] != Highlights::RED)
        m_curHighlight[reg] = Highlights::YELLOW;
}

void CheckDialog::clearYellow(quint32 reg)
{
    switch (m_curHighlight[reg])
    {
    case Highlights::RED:
        break;
    default:
        m_curHighlight[reg] = Highlights::CLEAN;
    }
}

void CheckDialog::setRed(quint32 reg)
{
    m_curHighlight[reg] = Highlights::RED;
}

void CheckDialog::clearRed(quint32 reg)
{
    if (m_curHighlight[reg] == Highlights::RED)
        m_curHighlight[reg] = Highlights::REDQ;
}

void CheckDialog::setHighlights()
{
    for (auto [key, value] : m_curHighlight.asKeyValueRange())
    {
        auto label = findChild<QLabel *>(QString::number(key));
        if (!label)
            continue;
        if ((value == Highlights::RED) || (value == Highlights::REDQ))
            label->setStyleSheet(c_errorStyle);
        else if ((value == Highlights::YELLOW) || (value == Highlights::YELLOWQ))
            label->setStyleSheet(c_warnStyle);
        else
            label->setStyleSheet(c_defaultStyle);
    }
}
