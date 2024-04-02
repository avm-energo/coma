#include "dialogs/configdialog.h"

#include "../../widgets/epopup.h"
#include "../../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <device/current_device.h>
#include <dialogs/keypressdialog.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <set>

ConfigDialog::ConfigDialog(Device::CurrentDevice *device, const S2BoardType boardType, QWidget *parent)
    : UDialog(device, parent)
    , m_datamanager(*m_device->getS2Datamanager())
    , m_boardConfig(m_datamanager.getConfiguration(boardType))
    , m_factory(m_boardConfig.m_workingConfig, device)
    , m_errConfState(new ErrConfState(device))
{
    connect(&m_datamanager, &S2DataManager::parseStatus, this, &ConfigDialog::parseStatusHandle);
    connect(m_device->getFileProvider(), &Device::FileProvider::noConfigurationError, //
        this, &ConfigDialog::noConfigurationHandle);
}

void ConfigDialog::readConfig()
{
    setSuccessMsg(tr("Конфигурация прочитана успешно"));
    m_device->getFileProvider()->request(S2::FilesEnum::Config);
}

void ConfigDialog::writeConfig()
{
    setSuccessMsg(tr("Конфигурация записана успешно"));
    if (checkPassword())
    {
        if (prepareConfigToWrite())
        {
            fillBack();
            auto s2file = m_datamanager.getBinaryConfiguration();
            engine()->currentConnection()->writeConfiguration(s2file);
        }
        else
            qCritical("Ошибка чтения конфигурации");
    }
}

void ConfigDialog::checkForDiff()
{
    const auto diffItems = m_boardConfig.m_defaultConfig.checkDiff(m_boardConfig.m_workingConfig);
    if (!diffItems.empty())
        qDebug() << diffItems;
}

bool ConfigDialog::isVisible(const quint16 id) const
{
    const auto &detailMap = m_datamanager.getStorage().getWidgetDetailMap();
    auto search = detailMap.find(id);
    if (search != detailMap.cend())
        return search->second.first;
    else
        return false;
}

void ConfigDialog::saveConfigToFile()
{
    auto filepath = WDFunc::ChooseFileForSave(this, "Config files (*.cf)", "cf");
    if (filepath.isEmpty())
        return;

    if (!prepareConfigToWrite())
    {
        qCritical("Ошибка чтения конфигурации");
        return;
    }
    fillBack();
    QByteArray file = m_datamanager.getBinaryConfiguration();
    Q_ASSERT(file.size() > 8);
    quint32 length = *reinterpret_cast<quint32 *>(&file.data()[4]);
    length += sizeof(S2::S2FileHeader);
    Q_ASSERT(length == static_cast<quint32>(file.size()));

    Error::Msg res = Files::SaveToFile(filepath, file);
    switch (res)
    {
    case Error::Msg::NoError:
        EMessageBox::information(this, "Записано успешно!");
        break;
    case Error::Msg::FileWriteError:
        qCritical("Ошибка при записи файла!");
        break;
    case Error::Msg::FileNameError:
        qCritical("Пустое имя файла!");
        break;
    case Error::Msg::FileOpenError:
        qCritical("Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void ConfigDialog::loadConfigFromFile()
{
    auto filepath = WDFunc::ChooseFileForOpen(this, "Config files (*.cf)");
    if (filepath.isEmpty())
        return;

    QByteArray file;
    Error::Msg res = Files::LoadFromFile(filepath, file);
    if (res != Error::Msg::NoError)
    {
        qCritical("Ошибка при загрузке файла конфигурации");
        return;
    }
    m_datamanager.parseS2File(file);
    EMessageBox::information(this, "Загрузка прошла успешно!");
}

QWidget *ConfigDialog::ConfButtons()
{
    auto wdgt = new QWidget;
    auto wdgtlyout = new QGridLayout;
    auto button = new QPushButton("Прочитать из модуля");
    connect(button, &QPushButton::clicked, this, &ConfigDialog::readConfig);
    wdgtlyout->addWidget(button, 0, 0, 1, 1);
    button = new QPushButton("Записать в модуль");
    button->setObjectName("WriteConfPB");
    connect(button, &QPushButton::clicked, this, &ConfigDialog::writeConfig);
    wdgtlyout->addWidget(button, 0, 1, 1, 1);
    button = new QPushButton("Прочитать из файла");
    connect(button, &QPushButton::clicked, this, &ConfigDialog::loadConfigFromFile);
    wdgtlyout->addWidget(button, 1, 0, 1, 1);
    button = new QPushButton("Записать в файл");
    connect(button, &QPushButton::clicked, this, &ConfigDialog::saveConfigToFile);
    wdgtlyout->addWidget(button, 1, 1, 1, 1);

    button = new QPushButton("Взять конфигурацию по умолчанию");
    connect(button, &QPushButton::clicked, this, &ConfigDialog::setDefaultConfig);
    wdgtlyout->addWidget(button, 2, 0, 1, 2);

    button = new QPushButton("Показать статус конфигурирования");
    connect(button, &QPushButton::clicked, this, [this] { showConfigErrState(); });
    wdgtlyout->addWidget(button, 3, 0, 1, 2);

    wdgt->setLayout(wdgtlyout);
    return wdgt;
}

quint32 ConfigDialog::tabForId(quint16 id)
{
    auto &widgetMap = m_datamanager.getStorage().getWidgetMap();
    auto search = widgetMap.find(id);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << id;
        return 0;
    }
    const auto var = search->second;
    delegate::WidgetGroup tab = 0;
    std::visit([&](const auto &arg) { tab = arg.group; }, var);
    return tab;
}

void ConfigDialog::createTabs(QTabWidget *tabWidget)
{
    std::set<delegate::WidgetGroup> uniqueTabs;
    auto &tabs = m_datamanager.getStorage().getConfigTabs();
    for (const auto &record : m_boardConfig.m_defaultConfig)
    {
        auto tab = tabForId(record.first);
        auto search = tabs.find(tab);
        if (search != tabs.cend())
            uniqueTabs.insert(tab);
        else
            qDebug() << "Undefined tab ID" << tab;
    }

    for (const auto &tab : uniqueTabs)
    {
        auto &tabName = tabs.at(tab);
        auto scrollArea = new QScrollArea(this);
        scrollArea->setObjectName(QString::number(tab));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        auto subBox = new QGroupBox(scrollArea);
        auto subLayout = new QVBoxLayout;
        subLayout->setAlignment(Qt::AlignTop);
        subLayout->setSpacing(0);
        subLayout->setContentsMargins(0, 0, 0, 0);
        subBox->setLayout(subLayout);
        scrollArea->setWidget(subBox);
        tabWidget->addTab(scrollArea, tabName);
    }
}

QWidget *widgetAt(QTabWidget *tabWidget, int tab)
{
    for (int i = 0; i != tabWidget->count(); ++i)
        if (tabWidget->widget(i)->objectName().toInt() == tab)
            return tabWidget->widget(i);
    return nullptr;
}

void ConfigDialog::setupUI()
{
    auto vlyout = new QVBoxLayout;
    auto ConfTW = new QTabWidget(this);
    createTabs(ConfTW);

    for (const auto &record : m_boardConfig.m_defaultConfig)
    {
        const auto id = record.first;
        if (isVisible(id))
        {
            auto widget = m_factory.createWidget(id, this);
            if (widget)
            {
                auto tab = tabForId(id);
                auto child = widgetAt(ConfTW, tab);
                Q_ASSERT(child);
                if (child)
                {
                    auto subBox = child->findChild<QGroupBox *>();
                    Q_ASSERT(subBox);
                    if (!subBox)
                        widget->deleteLater();
                    else
                    {
                        auto subLayout = subBox->layout();
                        subLayout->addWidget(widget);
                    }
                }
            }
            else
                qWarning() << "Bad config widget for item: " << id;
        }
    }
    vlyout->addWidget(ConfTW);
    vlyout->addWidget(ConfButtons());
    setLayout(vlyout);
}

void ConfigDialog::fill()
{
    for (const auto &[id, record] : m_boardConfig.m_workingConfig)
    {
        if (isVisible(id))
        {
            std::visit(
                // thanx to https://stackoverflow.com/a/46115028
                // in C++20 lambdas could capture structured binding
                [=, id = id](const auto &&value) {
                    bool status = m_factory.fillWidget(this, id, value);
                    if (!status)
                        qWarning() << "Couldnt fill widget for item: " << id;
                },
                record.getData());
        }
    }
}

void ConfigDialog::fillBack()
{
    for (const auto &[id, _] : m_boardConfig.m_workingConfig)
    {
        if (isVisible(id))
        {
            auto status = m_factory.fillBack(id, this);
            if (!status)
                qWarning() << "Couldnt fill back item from widget: " << id;
        }
    }
    checkConfig();
}

void ConfigDialog::setDefaultConfig()
{
    m_boardConfig.m_workingConfig = m_boardConfig.m_defaultConfig;
    fill();
}

void ConfigDialog::showConfigErrState()
{
    m_errConfState->show();
}

bool ConfigDialog::prepareConfigToWrite()
{
    if (m_confErrors.isEmpty())
        return true;
    else
    {
        EMessageBox::warning(this, "В конфигурации есть ошибки, проверьте и исправьте");
        return false;
    }

    //    auto dlg = new QDialog;
    //    dlg->setAttribute(Qt::WA_DeleteOnClose);
    //    auto vlyout = new QVBoxLayout;
    //    auto lbl = new QLabel("В конфигурации есть ошибки, проверьте и исправьте");
    //    vlyout->addWidget(lbl, 0, Qt::AlignLeft);
    //    auto te = new QTextEdit;
    //    te->setPlainText(CheckConfErrors.join("\n"));
    //    vlyout->addWidget(te, 0, Qt::AlignCenter);
    //    auto pb = new QPushButton("Хорошо");
    //    connect(pb, &QAbstractButton::clicked, dlg, &QWidget::close);
    //    vlyout->addWidget(pb);
    //    dlg->setLayout(vlyout);
    //    dlg->show();
    //    return false;
}

void ConfigDialog::uponInterfaceSetting()
{
    setupUI();
}

void ConfigDialog::checkConfig()
{
    m_confErrors.clear();
    /// TODO: А как проверять конфигурацию?
}

void ConfigDialog::parseStatusHandle(const Error::Msg status)
{
    if (status != Error::Msg::NoError)
    {
        EMessageBox::warning(this, "Ошибка чтения конфигурации, проверьте лог");
        switch (status)
        {
        case Error::Msg::HeaderSizeError:
            qWarning() << "Размер файла меньше установленного размера заголовка.";
            break;
        case Error::Msg::WrongFileError:
            qWarning() << "Получен файл, не являющийся конфигурацией.";
            break;
        case Error::Msg::SizeError:
            qWarning() << "Ошибка размера: выход за границу принятых байт.";
            break;
        case Error::Msg::CrcError:
            qWarning() << "Получена некорректная контрольная сумма.";
            break;
        default:
            // ignore other cases
            break;
        }
    }
    else
    {
        fill();
        EMessageBox::information(this, "Конфигурация прочитана успешно");
    }
}

void ConfigDialog::noConfigurationHandle()
{
    setDefaultConfig();
    EMessageBox::warning(this, "Задана конфигурация по умолчанию");
}
