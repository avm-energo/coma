#include "newhiddendialog.h"

#include "../widgets/epopup.h"
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
    : UDialog(crypto::hash, crypto::name, parent), m_settings(settings.getHiddenSettings()), m_isGodMode(false)
{
    m_dataUpdater->disableUpdates();
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
                HiddenWidget { "basetype", "Тип платы", 1, 1, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseserial", "Серийный номер платы", 10, 2, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseversion", "Версия платы", 3, 3, //
                    BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "moduleserial", "Серийный номер модуля", 13, 4, //
                    BinaryType::uint32, ViewType::LineEdit, true }             //
            } },                                                               //
        HiddenTab { "Мезонинная плата", ":/images/BnM.svg", "mezz",            //
            {
                HiddenWidget { "mezztype", "Тип платы", 2, 5, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzserial", "Серийный номер платы", 11, 6, //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzversion", "Версия платы", 12, 7, //
                    BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "reserved", "Резерв", 4, 8,          //
                    BinaryType::uint32, ViewType::LineEdit, false } //
            } }                                                     //
    };
}

void NewHiddenDialog::prepareInternalData(const ModuleTypes::SignalMap &sigMap)
{
    std::set<quint32> uniqueSignalIds;
    for (auto &&tabSettings : m_settings)
    {
        for (auto &&widgetSettings : tabSettings.widgets)
        {
            auto search = std::find_if(sigMap.cbegin(), sigMap.cend(), //
                [start = widgetSettings.address](const SigMapValue &element) -> bool {
                    auto &signal = element.second;
                    auto acceptStart = signal.startAddr;
                    auto acceptEnd = acceptStart + signal.count;
                    return (start >= acceptStart && start < acceptEnd);
                });
            if (search != sigMap.cend())
                uniqueSignalIds.insert(search->first);
            m_srcAddrStates.insert({ widgetSettings.address, false });
        }
    }
    for (auto unique_id : uniqueSignalIds)
    {
        auto &signal = sigMap.at(unique_id);
        m_dataUpdater->addBs({ signal.startAddr, signal.count });
    }
}

void NewHiddenDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    auto tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("background-color: transparent;"); // tabWidget прозрачный
    mainLayout->addWidget(WDFunc::NewLBLAndLE(this, "Имя модуля:", "modulename"));

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
    mainLayout->addWidget(tabWidget);
    auto btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignRight);

    auto modeChangeBtn = new QPushButton("Режим Д'Артяньян", this);
    connect(modeChangeBtn, &QAbstractButton::clicked, this, [this] {
        if (!checkPassword())
            return;
        m_isGodMode = true;
        updateUI();
    });
    btnLayout->addWidget(modeChangeBtn);

    auto updateBtn = new QPushButton("Обновить данные", this);
    connect(updateBtn, &QAbstractButton::clicked, this, [this] {
        fill();
        EMessageBox::information(this, "Успешно обновлено!");
    });
    btnLayout->addWidget(updateBtn);

    auto writeBtn = new QPushButton("Записать", this);
    connect(writeBtn, &QAbstractButton::clicked, this, [this] { fillBack(); });
    btnLayout->addWidget(writeBtn);
    mainLayout->addLayout(btnLayout, 1);
    setLayout(mainLayout);
}

QGroupBox *NewHiddenDialog::setupGroupBox(const ModuleTypes::HiddenTab &hiddenTab)
{
    auto tabGroupBox = new QGroupBox(hiddenTab.title, this);
    tabGroupBox->setStyleSheet("background-color: white;"); // tabGroupBox непрозрачный
    tabGroupBox->setObjectName(hiddenTab.prefix + "tab");
    auto gbLayout = new QVBoxLayout;
    auto enableCheckBox = WDFunc::NewChB2(this, hiddenTab.prefix + "enable", "Установлена");
    enableCheckBox->setChecked(true);
    connect(enableCheckBox, &QCheckBox::stateChanged, this, [this](int) { updateUI(); });
    gbLayout->addWidget(enableCheckBox);
    for (auto &&widget : hiddenTab.widgets)
    {
        if (widget.visibility)
        {
            auto title = widget.title + ':';
            if (widget.view == ModuleTypes::ViewType::Version)
            {
                auto hLayout = new QHBoxLayout;
                WDFunc::AddLabelAndLineeditH(this, hLayout, title, widget.name + 'm', false);
                WDFunc::AddLabelAndLineeditH(this, hLayout, ".", widget.name + 'l', false);
                WDFunc::AddLabelAndLineeditH(this, hLayout, ".", widget.name + 's', false);
                gbLayout->addLayout(hLayout);
            }
            else
                gbLayout->addWidget(WDFunc::NewLBLAndLE(this, title, widget.name, false));
        }
    }
    tabGroupBox->setLayout(gbLayout);
    return tabGroupBox;
}

void NewHiddenDialog::updateUI()
{
    if (!m_isGodMode)
        return;

    // Проверяем чек-бокс на каждой вкладке и на
    // основе этой информации обновляем все её виджеты
    for (auto &&tabSettings : m_settings)
    {
        bool enabled = false;
        if (!WDFunc::ChBData(this, tabSettings.prefix + "enable", enabled))
            continue;

        for (auto &&widget : tabSettings.widgets)
            updateWidget(enabled, widget);
    }
}

void NewHiddenDialog::updateWidget(const bool enabled, const ModuleTypes::HiddenWidget &widget)
{
    if (widget.view == ModuleTypes::ViewType::Version)
    {
        WDFunc::SetEnabled(this, widget.name + 'm', enabled);
        WDFunc::SetEnabled(this, widget.name + 'l', enabled);
        WDFunc::SetEnabled(this, widget.name + 's', enabled);
    }
    else
        WDFunc::SetEnabled(this, widget.name, enabled);
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
            if (widget.address == addr)
                return &widget;
        }
    }
    return nullptr;
}

void NewHiddenDialog::verifyFilling() noexcept
{
    // Проверяем состояния m_srcAddrStates
    for (const auto [_, state] : m_srcAddrStates)
    {
        if (!state)
            return;
    }

    // Если пришли сюда, то все элементы m_srcAddrStates установлены в true
    // Посылаем сигнал для выключения обновлений
    emit fillingFinished();

    // Сбрасываем состояния m_srcAddrStates
    for (auto &iter : m_srcAddrStates)
        iter.second = false;
}

void NewHiddenDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    auto search = m_srcAddrStates.find(bs.sigAdr);
    if (search != m_srcAddrStates.cend())
    {
        auto widgetData = findWidgetByAddress(bs.sigAdr);
        if (widgetData != nullptr)
            if (widgetData->visibility)
                fillWidget(bs.sigVal, *widgetData);
        search->second = true;
        verifyFilling();
    }
}

void NewHiddenDialog::fillWidget(const quint32 value, const ModuleTypes::HiddenWidget &widgetData)
{
    if (widgetData.view == ModuleTypes::ViewType::Version)
    {
        QString tmps = QString::number(static_cast<quint8>(value >> 24), 16);
        WDFunc::SetLEData(this, widgetData.name + 'm', tmps, "^[a-fA-F0-9]$");
        tmps = QString::number(static_cast<quint8>((value & 0x00FF0000) >> 16), 16);
        WDFunc::SetLEData(this, widgetData.name + 'l', tmps, "^[a-fA-F0-9]$");
        tmps = QString::number(static_cast<quint16>(value & 0x0000FFFF), 16);
        WDFunc::SetLEData(this, widgetData.name + 's', tmps, "^[a-fA-F0-9]{0,2}$");
    }
    else
        WDFunc::SetLEData(this, widgetData.name, QString::number(value, 16), "^[a-fA-F0-9]{1,8}$");
}

void NewHiddenDialog::fill()
{
    QEventLoop loop;
    connect(this, &NewHiddenDialog::fillingFinished, &loop, &QEventLoop::quit);
    m_dataUpdater->enableBitStringDataUpdates();
    m_dataUpdater->setUpdatesEnabled(true);
    m_dataUpdater->requestUpdates();
    loop.exec();
    m_dataUpdater->setUpdatesEnabled(false);
    m_dataUpdater->disableUpdates();
}

void NewHiddenDialog::fillBack()
{
    using namespace ModuleTypes;
    // Кладём описания всех виджетов в один временный вектор
    std::vector<HiddenWidget> temp;
    temp.reserve(16);
    for (auto &&tabSettings : m_settings)
        temp.insert(temp.end(), tabSettings.widgets.cbegin(), tabSettings.widgets.cend());
    // Сортировка временного вектора
    std::sort(temp.begin(), temp.end(), //
        [](const HiddenWidget &lhs, const HiddenWidget &rhs) { return lhs.index < rhs.index; });

    auto conn = m_dataUpdater->currentConnection();
    /// TODO
}

void NewHiddenDialog::setModuleName(const QString &moduleName)
{
    WDFunc::SetLEData(this, "modulename", moduleName);
}
