#include "configdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../module/board.h"
#include "../module/configstorage.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <gen/datamanager/typesproxy.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <gen/timefunc.h>
#include <set>

namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "confHash";
}

ConfigDialog::ConfigDialog(S2BoardConfig &boardConf, bool prereadConf, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent)
    , boardConfig(boardConf)
    , m_prereadConf(prereadConf)
    // , configV(nullptr)
    , factory(boardConfig.workingConfig)
    , proxyDRL(new DataTypesProxy)
    , errConfState(new ErrConfState())
{
    proxyDRL->RegisterType<QByteArray>();
    connect(proxyDRL.get(), &DataTypesProxy::DataStorable, this, //
        [this](const QVariant &var) { configReceived(var.value<QByteArray>()); });
}

void ConfigDialog::readConfig()
{
    setSuccessMsg(tr("Конфигурация прочитана успешно"));
    BaseInterface::iface()->reqFile(quint32(S2::FilesEnum::Config), DataTypes::FileFormat::DefaultS2);
}

void ConfigDialog::writeConfig()
{
    setSuccessMsg(tr("Конфигурация записана успешно"));
    if (checkPassword())
    {
        if (prepareConfigToWrite())
        {
            auto s2file = boardConfig.workingConfig.toByteArray();
            auto fileType = std_ext::to_underlying(S2::FilesEnum::Config);
            BaseInterface::iface()->writeFile(fileType, s2file);
        }
        else
            qCritical("Ошибка чтения конфигурации");
    }
}

void ConfigDialog::checkForDiff()
{
    const auto diffItems = boardConfig.defaultConfig.checkDiff(boardConfig.workingConfig);
    if (!diffItems.empty())
        qDebug() << diffItems;
}

bool ConfigDialog::isVisible(const quint16 id) const
{
    const auto &detailMap = S2::ConfigStorage::GetInstance().getWidgetDetailMap();
    auto search = detailMap.find(id);
    if (search != detailMap.cend())
        return search->second.first;
    else
        return false;
}

void ConfigDialog::configReceived(const QByteArray &rawData)
{
    using namespace S2;
    auto &workConfig = boardConfig.workingConfig;
    if (workConfig.updateByRawData(rawData))
    {
        const auto s2typeB = workConfig["MTypeB_ID"].value<DWORD>();
        const auto typeB = Board::GetInstance().typeB();
        if (s2typeB != typeB)
        {
            qCritical() << "Conflict typeB, module: " << QString::number(typeB, 16)
                        << " config: " << QString::number(s2typeB, 16);
            workConfig["MTypeB_ID"].setData(DWORD(typeB));
        }

        const auto s2typeM = workConfig["MTypeE_ID"].value<DWORD>();
        const auto typeM = Board::GetInstance().typeM();
        if (s2typeM != typeM)
        {
            qCritical() << "Conflict typeB, module: " << QString::number(typeM, 16)
                        << " config: " << QString::number(s2typeM, 16);
            workConfig["MTypeE_ID"].setData(DWORD(typeM));
        }

        checkForDiff();
        fill();
        EMessageBox::information(this, "Конфигурация прочитана успешно");
    }
    else
        EMessageBox::warning(this, "Ошибка чтения конфигурации, проверьте лог");
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
    QByteArray file = boardConfig.workingConfig.toByteArray();
    quint32 length = *reinterpret_cast<quint32 *>(&file.data()[4]);
    length += sizeof(S2::S2FileHeader);
    Q_ASSERT(length == quint32(file.size()));

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
    configReceived(file);
    EMessageBox::information(this, "Загрузка прошла успешно!");
}

QWidget *ConfigDialog::ConfButtons()
{
    auto wdgt = new QWidget;
    auto wdgtlyout = new QGridLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    auto button = new QPushButton("Прочитать из " + tmps);
    connect(button, &QPushButton::clicked, this, &ConfigDialog::readConfig);
    wdgtlyout->addWidget(button, 0, 0, 1, 1);
    tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    button = new QPushButton("Записать в " + tmps);
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

QWidget *widgetAt(QTabWidget *tabWidget, int index)
{
    for (int i = 0; i != tabWidget->count(); ++i)
        if (tabWidget->widget(i)->objectName().toInt() == index)
            return tabWidget->widget(i);
    return nullptr;
}

delegate::WidgetGroup groupForId(quint16 id)
{
    auto &widgetMap = S2::ConfigStorage::GetInstance().getWidgetMap();
    auto search = widgetMap.find(id);
    if (search == widgetMap.end())
    {
        qWarning() << "Not found" << id;
        return 0;
    }
    const auto var = search->second;

    delegate::WidgetGroup group = 0;
    std::visit([&](const auto &arg) { group = arg.group; }, var);
    return group;
}

void ConfigDialog::setupUI()
{
    auto vlyout = new QVBoxLayout;
    auto ConfTW = new QTabWidget(this);
    createTabs(ConfTW);

    for (const auto &record : boardConfig.defaultConfig)
    {
        const auto id = record.first;
        if (isVisible(id))
        {
            auto widget = factory.createWidget(id, this);
            if (widget)
            {
                auto group = groupForId(id);
                auto child = widgetAt(ConfTW, group);
                QGroupBox *subBox = qobject_cast<QGroupBox *>(child->findChild<QGroupBox *>());
                Q_ASSERT(subBox);
                if (!subBox)
                    widget->deleteLater();
                else
                {
                    auto lyout = subBox->layout();
                    lyout->addWidget(widget);
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

void ConfigDialog::createTabs(QTabWidget *tabWidget)
{
    std::set<delegate::WidgetGroup> currentCategories, intersection;
    auto &tabs = S2::ConfigStorage::GetInstance().getConfigTabs();

    for (const auto &record : boardConfig.defaultConfig)
    {
        auto tab = groupForId(record.first);
        auto search = tabs.find(tab);
        if (search != tabs.cend())
            intersection.insert(tab);
        else
            qDebug() << "Undefined tab ID" << tab;
    }

    for (const auto &group : intersection)
    {
        auto &tabName = tabs.at(group);
        auto subBox = new QGroupBox("Группа " + tabName, this);
        auto subvlyout = new QVBoxLayout;
        subvlyout->setAlignment(Qt::AlignTop);
        subvlyout->setSpacing(0);
        subvlyout->setContentsMargins(0, 0, 0, 0);
        subBox->setLayout(subvlyout);
        auto scrollArea = new QScrollArea;
        scrollArea->setObjectName(QString::number(group));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(subBox);
        tabWidget->addTab(scrollArea, tabName);
    }
}

void ConfigDialog::fill()
{
    for (const auto &defRecord : boardConfig.workingConfig)
    {
        const auto id = defRecord.first;
        if (isVisible(id))
        {
            const auto record = defRecord.second;
            std::visit(
                [=](const auto &&value) {
                    bool status = factory.fillWidget(this, id, value);
                    if (!status)
                        qWarning() << "Couldnt fill widget for item: " << id;
                },
                record.getData());
        }
    }
}

void ConfigDialog::prereadConfig()
{
    if (Board::GetInstance().noConfig()) // если в модуле нет конфигурации, заполнить поля по умолчанию
    {
        setDefaultConfig();
        EMessageBox::information(this, "Задана конфигурация по умолчанию");
    }
    else
        readConfig();
}

void ConfigDialog::fillBack() const
{
    for (const auto &record : boardConfig.workingConfig)
    {
        const auto id = record.first;
        if (isVisible(id))
        {
            auto status = factory.fillBack(id, this);
            if (!status)
                qWarning() << "Couldnt fill back item from widget: " << id;
        }
    }
}

void ConfigDialog::setDefaultConfig()
{
    boardConfig.workingConfig = boardConfig.defaultConfig;
    fill();
}

void ConfigDialog::showConfigErrState()
{
    errConfState->show();
}

bool ConfigDialog::prepareConfigToWrite()
{
    fillBack();
    checkConfig();
    if (CheckConfErrors.isEmpty())
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
    if (m_prereadConf)
        prereadConfig();
}

void ConfigDialog::checkConfig()
{
    CheckConfErrors.clear();
    /// TODO: А как проверять конфигурацию?
}
