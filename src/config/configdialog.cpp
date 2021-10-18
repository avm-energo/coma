#include "configdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/configv.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QTextEdit>
#include <set>
namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "confHash";
}
ConfigDialog::ConfigDialog(ConfigV *config, const QList<DataTypes::RecordPair> &defaultConfig, QWidget *parent)
    : UDialog(crypto::hash, crypto::name, parent), m_defaultValues(defaultConfig), configV(config)
{

    const auto &manager = DataManager::GetInstance();
    connect(&manager, &DataManager::dataRecVListReceived, this, &ConfigDialog::confReceived);
}

void ConfigDialog::ReadConf()
{
    setSuccessMsg(tr("Конфигурация прочитана успешно"));
    BaseInterface::iface()->reqFile(DataTypes::Config, Queries::FileFormat::DefaultS2);
}

void ConfigDialog::WriteConf()
{
    setSuccessMsg(tr("Конфигурация записана успешно"));
    if (!checkPassword())
        return;
    if (!PrepareConfToWrite())
    {
        qCritical("Ошибка чтения конфигурации");
        return;
    }
    S2DataTypes::S2ConfigType buffer;

    std::transform(configV->getConfig().cbegin(), configV->getConfig().cend(), std::back_inserter(buffer),
        [](const auto &record) -> S2DataTypes::DataRec { return record.serialize(); });
    S2::tester(buffer);

    buffer.push_back({ { S2DataTypes::dummyElement, 0 }, nullptr });
    BaseInterface::iface()->writeS2File(DataTypes::Config, &buffer);
}

// thanx to P0471R0
template <class Container> auto sinserter(Container &c)
{
    using std::end;
    return std::inserter(c, end(c));
}

bool operator<(const BciNumber &number, const DataTypes::RecordPair &record)
{
    return number < static_cast<BciNumber>(record.record.getId());
}

bool operator<(const DataTypes::RecordPair &record, const BciNumber &number)
{
    return number < static_cast<BciNumber>(record.record.getId());
}

void ConfigDialog::checkForDiff(const QList<DataTypes::DataRecV> &list)
{
    std::set<BciNumber> receivedItems;
    std::transform(list.cbegin(), list.cend(), sinserter(receivedItems),
        [](const DataTypes::DataRecV &record) { return static_cast<BciNumber>(record.getId()); });

    std::vector<BciNumber> diffItems;
    std::set_difference(receivedItems.cbegin(), receivedItems.cend(), m_defaultValues.cbegin(), m_defaultValues.cend(),
        std::back_inserter(diffItems));

    if (!diffItems.empty())
    {
        qDebug() << diffItems;
    }
}

void ConfigDialog::confReceived(const QList<DataTypes::DataRecV> &list)
{
    configV->setConfig(list);

    using namespace DataTypes;
    const auto s2typeB = configV->getRecord(BciNumber::MTypeB_ID).value<DWORD>();
    if (s2typeB != Board::GetInstance().typeB())
    {
        qCritical() << "Conflict typeB, module: " <<                //
            QString::number(Board::GetInstance().typeB(), 16)       //
                    << " config: " << QString::number(s2typeB, 16); //
        configV->setRecordValue({ BciNumber::MTypeB_ID, DWORD(Board::GetInstance().typeB()) });
    }

    const auto s2typeM = configV->getRecord(BciNumber::MTypeE_ID).value<DWORD>();
    if (s2typeM != Board::GetInstance().typeM())
    {
        qCritical() << "Conflict typeB, module: " <<                //
            QString::number(Board::GetInstance().typeM(), 16)       //
                    << " config: " << QString::number(s2typeM, 16); //
        configV->setRecordValue({ BciNumber::MTypeE_ID, DWORD(Board::GetInstance().typeM()) });
    }

    checkForDiff(list);
    Fill();
}

void ConfigDialog::SaveConfToFile()
{
    QByteArray ba;
    if (!PrepareConfToWrite())
    {
        qCritical("Ошибка чтения конфигурации");
        return;
    }
    S2::StoreDataMem(ba, configV->getConfig(), DataTypes::Config);
    quint32 length = *reinterpret_cast<quint32 *>(&ba.data()[4]);
    length += sizeof(S2DataTypes::FileHeader);
    Q_ASSERT(length == quint32(ba.size()));

    Error::Msg res = Files::SaveToFile(WDFunc::ChooseFileForSave(this, "Config files (*.cf)", "cf"), ba);
    switch (res)
    {
    case Error::Msg::NoError:
        QMessageBox::information(this, "Внимание", "Записано успешно!");
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

void ConfigDialog::LoadConfFromFile()
{
    QByteArray ba;
    Error::Msg res = Files::LoadFromFile(WDFunc::ChooseFileForOpen(this, "Config files (*.cf)"), ba);
    if (res != Error::Msg::NoError)
    {
        qCritical("Ошибка при загрузке файла конфигурации");
        return;
    }
    QList<DataTypes::DataRecV> outlistV;
    S2::RestoreData(ba, outlistV);

    confReceived(outlistV);
    QMessageBox::information(this, "Успешно", "Загрузка прошла успешно!");
}

QWidget *ConfigDialog::ConfButtons()
{
    QWidget *wdgt = new QWidget;
    QGridLayout *wdgtlyout = new QGridLayout;
    QString tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуля" : "прибора");
    QPushButton *pb = new QPushButton("Прочитать из " + tmps);
    connect(pb, &QAbstractButton::clicked, this, &ConfigDialog::ReadConf);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 0, 1, 1);
    tmps = ((DEVICETYPE == DEVICETYPE_MODULE) ? "модуль" : "прибор");
    pb = new QPushButton("Записать в " + tmps);
    pb->setObjectName("WriteConfPB");
    connect(pb, &QAbstractButton::clicked, this, &ConfigDialog::WriteConf);
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    wdgtlyout->addWidget(pb, 0, 1, 1, 1);
    pb = new QPushButton("Прочитать из файла");
    // pb->setIcon(QIcon(":/icons/tnload.svg"));
    connect(pb, &QAbstractButton::clicked, this, &ConfigDialog::LoadConfFromFile);
    wdgtlyout->addWidget(pb, 1, 0, 1, 1);
    pb = new QPushButton("Записать в файл");
    // pb->setIcon(QIcon(":/icons/tnsave.svg"));
    connect(pb, &QAbstractButton::clicked, this, &ConfigDialog::SaveConfToFile);
    wdgtlyout->addWidget(pb, 1, 1, 1, 1);
    pb = new QPushButton("Взять конфигурацию по умолчанию");
    connect(pb, &QAbstractButton::clicked, this, [this] { SetDefConf(); });
    wdgtlyout->addWidget(pb, 2, 0, 1, 2);
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

delegate::WidgetGroup groupForId(BciNumber id)
{
    const auto widgetMap = WidgetFactory::getWidgetMap();
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

void ConfigDialog::SetupUI()
{

    QVBoxLayout *vlyout = new QVBoxLayout;
    QTabWidget *ConfTW = new QTabWidget(this);

    WidgetFactory factory(configV);
    createTabs(ConfTW);

    for (const auto &record : qAsConst(m_defaultValues))
    {
        if (!record.visibility)
            continue;
        BciNumber id = static_cast<BciNumber>(record.record.getId());
        QWidget *widget = factory.createWidget(id, this);
        if (!widget)
        {
            qWarning() << "Bad config widget for item: " << QString::number(id);
            continue;
        }

        auto group = groupForId(id);
        auto child = widgetAt(ConfTW, group);

        QGroupBox *subBox = qobject_cast<QGroupBox *>(child->findChild<QGroupBox *>());
        Q_ASSERT(subBox);
        if (!subBox)
        {
            widget->deleteLater();
            continue;
        }
        auto *lyout = subBox->layout();
        lyout->addWidget(widget);
    }
    vlyout->addWidget(ConfTW);
    vlyout->addWidget(ConfButtons());
    setLayout(vlyout);
}

void ConfigDialog::createTabs(QTabWidget *tabWidget)
{
    std::set<delegate::WidgetGroup> currentCategories, intersection;
    const auto categories = WidgetFactory::getCategoryMap();
    for (const auto &record : qAsConst(m_defaultValues))
    {
        auto group = groupForId(static_cast<BciNumber>(record.record.getId()));
        if (categories.contains(group))
            intersection.insert(group);
    }
    for (const auto &group : intersection)
    {
        QGroupBox *subBox = nullptr;

        subBox = new QGroupBox("Группа " + WidgetFactory::getCategory(group), this);
        QVBoxLayout *subvlyout = new QVBoxLayout;
        subvlyout->setAlignment(Qt::AlignTop);
        subvlyout->setSpacing(0);

        subvlyout->setContentsMargins(0, 0, 0, 0);

        subBox->setLayout(subvlyout);

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setObjectName(QString::number(group));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(subBox);

        tabWidget->addTab(scrollArea, WidgetFactory::getCategory(group));
    }
}

void ConfigDialog::Fill()
{
    for (const auto &defRecord : m_defaultValues)
    {
        if (!defRecord.visibility)
            continue;
        BciNumber id = static_cast<BciNumber>(defRecord.record.getId());
        const auto record = configV->getRecord(id);
        std::visit(
            [=](const auto &&value) {
                WidgetFactory factory(configV);
                bool status = factory.fillWidget(this, static_cast<BciNumber>(record.getId()), value);
                if (!status)
                {
                    qWarning() << "Couldnt fill widget for item: " << record.getId();
                }
            },
            record.getData());
    }
}

void ConfigDialog::PrereadConf()
{
    if (Board::GetInstance().noConfig()) // если в модуле нет конфигурации, заполнить поля по умолчанию
    {
        SetDefConf();
        QMessageBox::information(this, "Успешно", "Задана конфигурация по умолчанию", QMessageBox::Ok);
    }
    else
        ReadConf();
}

void ConfigDialog::FillBack() const
{
    WidgetFactory factory(configV);
    for (const auto &record : m_defaultValues)
    {
        if (!record.visibility)
            continue;
        BciNumber id = static_cast<BciNumber>(record.record.getId());
        bool status = factory.fillBack(id, this);
        if (!status)
        {
            qWarning() << "Couldnt fill back item from widget: " << id;
        }
    }
}

void ConfigDialog::SetDefConf()
{
    for (const auto &record : m_defaultValues)
        configV->setRecordValue(record.record);
    Fill();
}

bool ConfigDialog::PrepareConfToWrite()
{
    FillBack();
    CheckConfErrors.clear();
    CheckConf();
    if (CheckConfErrors.isEmpty())
        return true;

    QDialog *dlg = new QDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *vlyout = new QVBoxLayout;
    QLabel *lbl = new QLabel("В конфигурации есть ошибки, проверьте и исправьте");
    vlyout->addWidget(lbl, 0, Qt::AlignLeft);
    QTextEdit *te = new QTextEdit;
    te->setPlainText(CheckConfErrors.join("\n"));
    vlyout->addWidget(te, 0, Qt::AlignCenter);
    QPushButton *pb = new QPushButton("Хорошо");
    connect(pb, &QAbstractButton::clicked, dlg, &QWidget::close);
    vlyout->addWidget(pb);
    dlg->setLayout(vlyout);
    dlg->show();
    return false;
}

void ConfigDialog::uponInterfaceSetting()
{
    SetupUI();
    PrereadConf();
}

void ConfigDialog::CheckConf()
{
}
