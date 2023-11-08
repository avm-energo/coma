#include "newhiddendialog.h"

#include "../widgets/wd_func.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QPaintEvent>
#include <QPainter>
#include <QTabWidget>
#include <QtSvg/QSvgRenderer>

namespace crypto
{
static constexpr char hash[] = "fb001dfcffd1c899f3297871406242f097aecf1a5342ccf3ebcd116146188e4b";
static constexpr char name[] = "hiddenHash";
}

NewHiddenDialog::NewHiddenDialog(const ModuleSettings &settings, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), m_settings(settings.getHiddenSettings())
{
    m_dataUpdater->setUpdatesEnabled(false);
    if (m_settings.empty())
        generateDefaultSettings();
    prepareInternalData(settings.getSignals());
    setupUI();
}

void NewHiddenDialog::generateDefaultSettings()
{
    using namespace ModuleTypes;
    m_settings = {
        HiddenTab { "Базовая плата", ":/images/BMn.svg", "base", //
            {
                HiddenWidget { "basetype", "Тип платы", 1, 40010, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseserial", "Серийный номер платы", 10, 40011, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseversion", "Версия платы", 3, 40012, //
                    BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "moduleserial", "Серийный номер модуля", 13, 40013, //
                    BinaryType::uint32, ViewType::LineEdit, true }                 //
            } },                                                                   //
        HiddenTab { "Мезонинная плата", ":/images/BnM.svg", "mezz",                //
            {
                HiddenWidget { "mezztype", "Тип платы", 2, 40014, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzserial", "Серийный номер платы", 11, 40015, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzversion", "Версия платы", 12, 40016, //
                    BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "reserved", "Резерв", 1, 40017,      //
                    BinaryType::uint32, ViewType::LineEdit, false } //
            } }                                                     //
    };
}

void NewHiddenDialog::prepareInternalData(const ModuleTypes::SignalMap &sigMap)
{
    m_requestSignals.reserve(4);
    std::set<quint32> uniqueSignalIds;
    for (auto &&tabSettings : m_settings)
    {
        for (auto &&widgetSettings : tabSettings.widgets)
        {
            auto search = std::find_if(sigMap.cbegin(), sigMap.cend(), //
                [start = widgetSettings.src_addr](const SigMapValue &element) -> bool {
                    auto &signal = element.second;
                    auto acceptStart = signal.startAddr;
                    auto acceptEnd = acceptStart + signal.count;
                    return (start >= acceptStart && start < acceptEnd);
                });
            if (search != sigMap.cend())
                uniqueSignalIds.insert(search->first);
            m_srcAddresses.insert(widgetSettings.src_addr);
        }
    }
    for (auto unique_id : uniqueSignalIds)
        m_requestSignals.push_back(sigMap.at(unique_id));
}

void NewHiddenDialog::setupUI()
{
    auto layout = new QVBoxLayout;
    auto tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("background-color: transparent;"); // tabWidget прозрачный
    layout->addWidget(WDFunc::NewLBLAndLE(this, "Тип модуля:", "moduletype"));

    for (auto &&tabSettings : m_settings)
    {
        auto tab = new QWidget(this);
        auto tabHLayout = new QHBoxLayout;
        auto tabVLayout = new QVBoxLayout;
        tabHLayout->addWidget(setupGroupBox(tabSettings), 10);
        tabHLayout->addStretch(14);
        tabVLayout->addLayout(tabHLayout, 1);
        tabVLayout->addStretch(2);
        tab->setLayout(tabVLayout);
        tabWidget->addTab(tab, tabSettings.title);
    }

    m_currentBackground = m_settings[0].background;
    connect(tabWidget, &QTabWidget::currentChanged, this, //
        [this](int newIndex) {
            if (newIndex >= 0 && newIndex < m_settings.size())
                m_currentBackground = m_settings[newIndex].background;
            update();
        });
    layout->addWidget(tabWidget);
    setLayout(layout);
}

QGroupBox *NewHiddenDialog::setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab)
{
    auto tabGroupBox = new QGroupBox(hiddenTab.title, this);
    tabGroupBox->setStyleSheet("background-color: white;"); // tabGroupBox непрозрачный
    tabGroupBox->setObjectName(hiddenTab.prefix + "tab");
    auto gbLayout = new QVBoxLayout;
    gbLayout->addWidget(WDFunc::NewChB2(this, hiddenTab.prefix + "enable", "Установлена"));
    for (auto &&widget : hiddenTab.widgets)
    {
        if (widget.visibility)
        {
            auto title = widget.title + ':';
            if (widget.view == ModuleTypes::ViewType::Version)
            {
                auto hLayout = new QHBoxLayout;
                WDFunc::AddLabelAndLineeditH(hLayout, title, widget.name + 'm', false);
                WDFunc::AddLabelAndLineeditH(hLayout, ".", widget.name + 'l', false);
                WDFunc::AddLabelAndLineeditH(hLayout, ".", widget.name + 's', false);
                gbLayout->addLayout(hLayout);
            }
            else
                gbLayout->addWidget(WDFunc::NewLBLAndLE(this, title, widget.name, false));
        }
    }
    tabGroupBox->setLayout(gbLayout);
    return tabGroupBox;
}

void NewHiddenDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QSvgRenderer svg(m_currentBackground);
    svg.render(&painter);
    painter.end();
    e->accept();
}

const ModuleTypes::HiddenWidget *NewHiddenDialog::findWidgetByAddress(const quint32 addr) const noexcept
{
    for (auto &&tabSettings : m_settings)
    {
        for (auto &&widget : tabSettings.widgets)
        {
            if (widget.src_addr == addr)
                return &widget;
        }
    }
    return nullptr;
}

void NewHiddenDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (!updatesEnabled())
        return;

    if (m_srcAddresses.find(bs.sigAdr) != m_srcAddresses.cend())
    {
        auto search = findWidgetByAddress(bs.sigAdr);
        Q_ASSERT(search != nullptr);
        if (search->visibility)
            fillWidget(bs.sigVal, *search);
    }
}

void NewHiddenDialog::fillWidget(const quint32 value, const ModuleTypes::HiddenWidget &widget)
{
    if (widget.view == ModuleTypes::ViewType::Version)
    {
        QString tmps = QString::number(static_cast<quint8>(value >> 24), 16);
        WDFunc::SetLEData(this, widget.name + 'm', tmps, "^[a-fA-F0-9]$");
        tmps = QString::number(static_cast<quint8>((value & 0x00FF0000) >> 16), 16);
        WDFunc::SetLEData(this, widget.name + 'l', tmps, "^[a-fA-F0-9]$");
        tmps = QString::number(static_cast<quint16>(value & 0x0000FFFF), 16);
        WDFunc::SetLEData(this, widget.name + 's', tmps, "^[a-fA-F0-9]{0,2}$");
    }
    else
        WDFunc::SetLEData(this, widget.name, QString::number(value, 16), "^[a-fA-F0-9]{1,8}$");
}
