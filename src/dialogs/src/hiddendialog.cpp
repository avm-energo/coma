#include "dialogs/hiddendialog.h"

#include <QtSvg/QSvgRenderer>
#include <device/current_device.h>
#include <widgets/chbfunc.h>
#include <widgets/emessagebox.h>
#include <widgets/lefunc.h>
#include <widgets/pbfunc.h>
#include <widgets/wdfunc.h>

#include <QBoxLayout>
#include <QGroupBox>
#include <QPaintEvent>
#include <QPainter>
#include <QTabWidget>
#include <set>

using namespace Device::XmlDataTypes;

HiddenDialog::HiddenDialog(Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent)
    , m_settings(m_device->getConfigStorage()->getDeviceSettings().getHiddenSettings())
    , m_isGodMode(false)
    , m_isAlreadyFilled(false)
    , m_isSendedEnableCmd(false)
    , m_isSendedWritingCmd(false)
{
    m_dataUpdater->disableUpdates();
    if (m_settings.empty())
        generateDefaultSettings();
    prepareInternalData(m_device->getConfigStorage()->getDeviceSettings().getSignals());
    setupUI();
}

void HiddenDialog::generateDefaultSettings()
{
    m_settings = {
        HiddenTab { "Базовая плата", "base", 1,                                //
            {
                HiddenWidget { "basetype", "Тип платы", 1, 1,                  //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseserial", "Серийный номер платы", 10, 2,    //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "baseversion", "Версия платы", 3, 3,            //
                    BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "moduleserial", "Серийный номер модуля", 13, 4, //
                    BinaryType::uint32, ViewType::LineEdit, true }             //
            } },                                                               //
        HiddenTab { "Мезонинная плата", "mezz", 2,                             //
            {
                HiddenWidget { "mezztype", "Тип платы", 2, 5,                  //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzserial", "Серийный номер платы", 11, 6,    //
                    BinaryType::uint32, ViewType::LineEdit, true },
                HiddenWidget { "mezzversion", "Версия платы", 12, 7,           //
                    BinaryType::uint32, ViewType::Version, true },
                HiddenWidget { "reserved", "Резерв", 4, 8,                     //
                    BinaryType::uint32, ViewType::LineEdit, false }            //
            } } //
    };
}

void HiddenDialog::prepareInternalData(const SignalMap &sigMap)
{
    std::set<quint32> uniqueSignalIds;
    // Ищем сигналы, соотвествующие адресам, указанным в настройках
    for (auto &&tabSettings : m_settings)
    {
        for (auto &&widgetSettings : tabSettings.widgets)
        {
            auto search = std::find_if(sigMap.cbegin(), sigMap.cend(), //
                [start = widgetSettings.address](const Device::SigMapValue &element) -> bool
                {
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
    // Добавляем в список запрашиваемых сигналов
    for (auto unique_id : uniqueSignalIds)
    {
        auto &signal = sigMap.at(unique_id);
        m_dataUpdater->addBs({ signal.startAddr, signal.count });
    }
}

void HiddenDialog::setupUI()
{
    auto mainLayout = new QVBoxLayout;
    auto tabWidget = new QTabWidget(this);
    mainLayout->addWidget(LEFunc::NewLBL(this, "Имя модуля:", "modulename"));

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

    mainLayout->addWidget(tabWidget);
    auto btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignRight);

    auto modeChangeBtn = new QPushButton("Режим Д'Артаньян", this);
    connect(modeChangeBtn, &QAbstractButton::clicked, this,
        [this]
        {
            if (checkPassword())
            {
                EMessageBox::warning(this, "Активирован режим Д'Артаньяна");
                m_isGodMode = true;
                fill();
                updateUI();
            }
        });
    btnLayout->addWidget(modeChangeBtn);

    auto updateBtn = new QPushButton("Обновить данные", this);
    connect(updateBtn, &QAbstractButton::clicked, this, &HiddenDialog::fill);
    btnLayout->addWidget(updateBtn);

    auto writeBtn = new QPushButton("Записать", this);
    connect(writeBtn, &QAbstractButton::clicked, this,
        [this]
        {
            if (checkPassword())
                write();
        });
    btnLayout->addWidget(writeBtn);
    mainLayout->addLayout(btnLayout, 1);
    setLayout(mainLayout);
}

QGroupBox *HiddenDialog::setupGroupBox(const HiddenTab &hiddenTab)
{
    auto tabGroupBox = new QGroupBox(hiddenTab.title, this);
    tabGroupBox->setObjectName(hiddenTab.prefix + "tab");
    auto gbLayout = new QVBoxLayout;
    if (hiddenTab.flag != 1)
    {
        auto enableCheckBox = ChBFunc::New(this, hiddenTab.prefix + "enable", "Установлена");
        enableCheckBox->setChecked(true);
#if (QT_VERSION < QT_VERSION_CHECK(6, 8, 2))
        connect(enableCheckBox, &QCheckBox::stateChanged, this, [this](int) { updateUI(); });
#else
        connect(enableCheckBox, &QCheckBox::checkStateChanged, this, [this](int) { updateUI(); });
#endif
        gbLayout->addWidget(enableCheckBox);
    }
    for (auto &&widget : hiddenTab.widgets)
    {
        if (widget.visibility)
        {
            auto title = widget.title + ':';
            if (widget.view == ViewType::Version)
            {
                auto hlyout = new QHBoxLayout;
                hlyout->addWidget(LEFunc::NewLBL(this, title, widget.name + 'm', false));
                hlyout->addWidget(LEFunc::NewLBL(this, ".", widget.name + 'l', false));
                hlyout->addWidget(LEFunc::NewLBL(this, ".", widget.name + 's', false));
                gbLayout->addLayout(hlyout);
            }
            else
                gbLayout->addWidget(LEFunc::NewLBL(this, title, widget.name, false));
        }
    }
    tabGroupBox->setLayout(gbLayout);
    return tabGroupBox;
}

void HiddenDialog::paintEvent(QPaintEvent *e)
{
    e->accept();
}

void HiddenDialog::updateUI()
{
    if (!m_isGodMode)
        return;

    // Проверяем чек-бокс на каждой вкладке и на
    // основе этой информации обновляем все её виджеты
    for (auto &&tabSettings : m_settings)
    {
        bool isEnabled = isTabEnabled(tabSettings);
        for (auto &&widget : tabSettings.widgets)
            updateWidget(isEnabled, widget);
    }
}

bool HiddenDialog::isTabEnabled(const HiddenTab &tabSettings) const noexcept
{
    bool enabled = false;
    if (tabSettings.flag != 1)
        ChBFunc::Data(this, tabSettings.prefix + "enable", enabled);
    else
        enabled = true;
    return enabled;
}

void HiddenDialog::updateWidget(const bool enabled, const HiddenWidget &widget)
{
    if (widget.view == ViewType::Version)
    {
        WDFunc::SetEnabled(this, widget.name + 'm', enabled);
        WDFunc::SetEnabled(this, widget.name + 'l', enabled);
        WDFunc::SetEnabled(this, widget.name + 's', enabled);
    }
    else
        WDFunc::SetEnabled(this, widget.name, enabled);
}

const HiddenWidget *HiddenDialog::findWidgetByAddress(const quint32 addr) const noexcept
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

void HiddenDialog::verifyFilling() noexcept
{
    // Проверяем состояния m_srcAddrStates
    for (const auto [_, state] : m_srcAddrStates)
    {
        if (!state)
            return;
    }

    // Если пришли сюда, то все элементы m_srcAddrStates установлены в true
    // Выключаем обновление данных в диалоге
    m_dataUpdater->setUpdatesEnabled(false);
    m_dataUpdater->disableUpdates();

    // Для повторного запроса
    if (m_isAlreadyFilled)
        EMessageBox::information(this, "Успешно обновлено!");
    else
        m_isAlreadyFilled = true;

    // Сбрасываем состояния m_srcAddrStates
    for (auto &iter : m_srcAddrStates)
        iter.second = false;
}

void HiddenDialog::updateBitStringData(const DataTypes::BitStringStruct &bs)
{
    if (!updatesEnabled())
        return;
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

void HiddenDialog::fillWidget(const quint32 value, const HiddenWidget &widgetData)
{
    if (widgetData.view == ViewType::Version)
    {
        QString tmps = QString::number(static_cast<quint8>((value & 0xFF000000) >> 24), 16);
        LEFunc::SetData(this, widgetData.name + 'm', tmps, "^[a-fA-F0-9]$");
        tmps = QString::number(static_cast<quint8>((value & 0x00FF0000) >> 16), 16);
        LEFunc::SetData(this, widgetData.name + 'l', tmps, "^[a-fA-F0-9]$");
        tmps = QString::number(static_cast<quint16>(value & 0x0000FFFF), 16);
        LEFunc::SetData(this, widgetData.name + 's', tmps, "^[a-fA-F0-9]{0,2}$");
    }
    else
        LEFunc::SetData(this, widgetData.name, QString::number(value, 16), "^[a-fA-F0-9]{1,8}$");
}

quint32 HiddenDialog::getDataFrom(const HiddenWidget &widgetData)
{
    /// TODO: Обрабатывать widgetData.type или убрать совсем
    if (widgetData.visibility)
    {
        QString tmps;
        // Version fill back
        if (widgetData.view == ViewType::Version)
        {
            LEFunc::Data(this, widgetData.name + 'm', tmps);
            quint32 number = static_cast<quint32>(tmps.toInt()) << 24;
            LEFunc::Data(this, widgetData.name + 'l', tmps);
            number |= static_cast<quint32>(tmps.toInt()) << 16;
            LEFunc::Data(this, widgetData.name + 's', tmps);
            number |= static_cast<quint32>(tmps.toInt());
            return number;
        }
        // Line edit fill back
        else
        {
            LEFunc::Data(this, widgetData.name, tmps);
            quint32 number = static_cast<quint32>(tmps.toUInt(nullptr, 16));
            return number;
        }
    }
    else
        return quint32(0xFFFFFFFF);
}

void HiddenDialog::updateGeneralResponse(const DataTypes::GeneralResponseStruct &response)
{
    if (m_isSendedEnableCmd)
    {
        m_isSendedEnableCmd = false;
        if (response.type == DataTypes::Ok)
        {
            auto conn = m_dataUpdater->currentConnection();
            conn->writeCommand(Commands::C_WriteHiddenBlock, QVariant::fromValue(m_hardwareInfo));
            m_isSendedWritingCmd = true;
        }
        else if (response.type == DataTypes::Error)
            EMessageBox::error(this, "Устройство запрещает запись Hidden Block");
    }
    else if (m_isSendedWritingCmd)
    {
        m_isSendedWritingCmd = false;
        m_dataUpdater->setUpdatesEnabled(false);
        if (response.type == DataTypes::Ok)
            EMessageBox::information(this, "Записано успешно\nНеобходимо переподключиться");
        else if (response.type == DataTypes::Error)
            EMessageBox::error(this, "Ошибка записи Hidden Block");
    }
}

void HiddenDialog::setModuleName(const QString &moduleName)
{
    LEFunc::SetData(this, "modulename", moduleName);
}

void HiddenDialog::fill()
{
    m_dataUpdater->enableBitStringDataUpdates();
    m_dataUpdater->setUpdatesEnabled(true);
    m_dataUpdater->requestUpdates();
}

void HiddenDialog::write()
{
    constexpr std::size_t maxWidgetsCount = 16;
    std::vector<HiddenWidget> temp;
    temp.reserve(maxWidgetsCount);
    quint32 id = 0;

    // Кладём описания всех виджетов во временный вектор
    for (auto &&tabSettings : m_settings)
    {
        // Только в том случае, если вкладка в состоянии 'enabled'
        if (isTabEnabled(tabSettings))
        {
            temp.insert(temp.end(), tabSettings.widgets.cbegin(), tabSettings.widgets.cend());
            id |= tabSettings.flag;
        }
    }
    // Сортировка временного вектора
    std::sort(temp.begin(), temp.end(), //
        [](const HiddenWidget &lhs, const HiddenWidget &rhs) { return lhs.index < rhs.index; });

    // Формируем вектор блока данных
    std::vector<quint32> hiddenBlock;
    hiddenBlock.reserve(temp.size());
    for (auto &&widget : temp)
        hiddenBlock.push_back(getDataFrom(widget));
    // Формируем блок на отправку в устройство
    auto bufferSize = hiddenBlock.size() * sizeof(quint32);
    QByteArray buffer(bufferSize, 0);
    auto srcBegin = reinterpret_cast<char *>(hiddenBlock.data());
    auto srcEnd = srcBegin + bufferSize;
    std::copy(srcBegin, srcEnd, buffer.begin());
    m_hardwareInfo = DataTypes::HardwareStruct { id, buffer };

    // Посылаем команду на разрешение записи Hidden Block
    auto conn = m_dataUpdater->currentConnection();
    conn->writeCommand(Commands::C_EnableHiddenBlockWriting, quint16(0x5c5c));
    m_isSendedEnableCmd = true;
    m_dataUpdater->setUpdatesEnabled(true);
}
