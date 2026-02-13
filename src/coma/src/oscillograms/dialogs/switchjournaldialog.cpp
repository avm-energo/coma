#include "oscillograms/dialogs/switchjournaldialog.h"

#include <device/current_device.h>
#include <avm-gen/files.h>
#include <avm-gen/settings.h>
#include <avm-gen/timefunc.h>
#include <models/etablemodel.h>
#include <oscillograms/dialogs/trendviewdialog.h>
#include <oscillograms/swjmanager.h>
#include <s2/s2util.h>
#include <avm-widgets/etableview.h>
#include <avm-widgets/filefunc.h>
#include <avm-widgets/pbfunc.h>
#include <avm-widgets/pushbuttondelegate.h>
#include <avm-widgets/wdfunc.h>

#include <QHeaderView>
#include <QLayout>
#include <QMainWindow>
#include <QMessageBox>

// constexpr int MAXSWJNUM = 262144;
constexpr unsigned char TECH_SWJ = 0x04;

SwitchJournalDialog::SwitchJournalDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    enableResponseConnections();
    setupUI();
}

void SwitchJournalDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(PBFunc::New(this, "", "Получить журналы переключений", this,
        [&]
        {
            m_swjMap.clear();
            m_tableModel = UniquePointer<ETableModel>(new ETableModel);
            m_tableModel->setHorizontalHeaderLabels(
                { "#", "Номер переключения", "Дата/Время", "Аппарат", "Переключение", "Скачать" });
            m_swjTableView->setModel(m_tableModel.get());

            engine()->currentConnection()->writeCommand(Commands::C_ReqBlkDataTech, TECH_SWJ);
        }));

    hlyout->addWidget(PBFunc::New(this, "", "Стереть журнал переключений", this, &SwitchJournalDialog::eraseJournals));
    lyout->addLayout(hlyout);
    m_swjTableView = new ETableView;

    PushButtonDelegate *getDelegate = new PushButtonDelegate(this);
    connect(getDelegate, &PushButtonDelegate::clicked, this, &SwitchJournalDialog::getSwJ);
    // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    m_swjTableView->setItemDelegateForColumn(5, getDelegate);

    lyout->addWidget(m_swjTableView);
    setLayout(lyout);
}

void SwitchJournalDialog::fillJour(const S2::FileStruct &fs)
{
    if (!updatesEnabled())
        return;

    m_fileBuffer.push_back(fs);

    switch (fs.ID)
    {
    case MT_HEAD_ID:
    {
        auto header = m_oscManager.loadCommon(fs);
        m_oscManager.setHeader(header);
        break;
    }
    case AVTUK_85::SWJ_ID:
    {
        SwjManager swjManager;
        m_swjModel = swjManager.load(fs);
        auto dialog = new SwitchJournalViewDialog(m_swjModel, m_oscModel, m_oscManager, this);
        dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(dialog, &SwitchJournalViewDialog::exportJour, this, [&, swjNum = m_reqSwJNum] { exportSwJ(swjNum); });
        dialog->show();
        dialog->setMinimumHeight(WDFunc::getMainWindow()->height());
        dialog->setMinimumWidth(WDFunc::getMainWindow()->width());
        dialog->adjustSize();
        break;
    }
    default:
    {
        m_oscModel = m_oscManager.load(fs);

        if (!m_oscModel)
        {
            qWarning() << Error::ReadError;
            return;
        }
    }
    }
    // header, swj, osc
    if (m_fileBuffer.size() == 3)
    {
        QByteArray ba;
        S2Util::StoreDataMem(ba, m_fileBuffer, m_reqSwJNum);
        auto time = m_swjMap.value(m_reqSwJNum).time;
        QString file = filename(time);
        if (Files::SaveToFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Файл swj сохранён: " << file;
        }
        else
        {
            qWarning() << "Ошибка при сохранении swj-файла: " << file;
        }
    }
}

void SwitchJournalDialog::fillSwJInfo(const S2::SwitchJourInfo &swjInfo)
{
    if (swjInfo.num == 0)
        return;
    if (m_swjMap.contains(swjInfo.num))
        return;

    m_swjMap.insert(swjInfo.num, swjInfo);

    QVector<QVariant> lsl {
        QVariant(swjInfo.fileNum),                                            //
        swjInfo.num,                                                          //
        TimeFunc::UnixTime64ToInvStringFractional(swjInfo.time),              //
        SwjManager::craftType(swjInfo.typeA) + QString::number(swjInfo.numA), //
        SwjManager::switchType(swjInfo.options),                              //
        tr("Скачать"),                                                        //
    };
    m_tableModel->addRowWithData(lsl);
}

void SwitchJournalDialog::disableResponseConnections()
{
    QObject::disconnect(m_fillJourConnection);
    QObject::disconnect(m_fillSwJConnection);
}

void SwitchJournalDialog::enableResponseConnections()
{
    if (!m_fillSwJConnection)
        m_fillSwJConnection = m_dataUpdater->currentConnection()->connection(this, &SwitchJournalDialog::fillSwJInfo);
    if (!m_fillJourConnection)
        m_fillJourConnection = m_dataUpdater->currentConnection()->connection(this, &SwitchJournalDialog::fillJour);
}

void SwitchJournalDialog::getSwJ(const QModelIndex &idx)
{
    m_fileBuffer.clear();

    bool ok = false;
    auto model = idx.model();

    if (!model)
        return;
    // номер файла
    auto fileNum = model->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toUInt(&ok);
    // номер файла
    m_reqSwJNum = model->data(idx.sibling(idx.row(), Column::switchNumber), Qt::DisplayRole).toUInt(&ok);
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }
    if (!m_swjMap.contains(m_reqSwJNum))
    {
        qWarning("Cannot find");
        return;
    }
    quint32 size = m_swjMap.value(m_reqSwJNum).fileLength;

    if (!loadIfExist(size))
        engine()->currentConnection()->reqFile(
            fileNum, DataTypes::FileFormat::CustomS2, size + 2 * sizeof(S2::DataRecHeader));
}

void SwitchJournalDialog::exportSwJ(uint32_t swjNum)
{

    if (!m_swjMap.contains(swjNum))
    {
        qWarning("Cannot find");
        return;
    }
    quint32 size = m_swjMap.value(swjNum).fileLength;

    auto time = m_swjMap.value(swjNum).time;
    auto currentFile = filename(time);

    QFile swjFile(currentFile);

    if (!swjFile.exists() || (swjFile.size() < size))
        QMessageBox::information(this, "Сохранение", "Скачайте перед сохранением");

    auto newFile = FileFunc::chooseFileForSave(this, "Файлы журналов (*.swj)", "swj", getFilenameForDevice());
    if (!QFile::copy(currentFile, newFile))
    {
        QMessageBox::warning(this, "Сохранение", "Скачайте перед сохранением");
    }
}

void SwitchJournalDialog::eraseJournals()
{
    if (checkPassword())
        engine()->currentConnection()->writeCommand(Commands::C_EraseTechBlock, TECH_SWJ);
}

bool SwitchJournalDialog::loadIfExist(quint32 size)
{
    auto time = m_swjMap.value(m_reqSwJNum).time;
    auto file = filename(time);

    QFile swjFile(file);

    if (!swjFile.exists())
        return false;

    if (swjFile.size() < size)
        return false;

    int ret = QMessageBox::question(this, "Кэширование", "Прочитать из кэша");
    if ((ret == QMessageBox::StandardButton::Ok) || (ret == QMessageBox::StandardButton::Yes))
    {
        QByteArray ba;
        if (Files::LoadFromFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Файл swj загружен: " << file;
            DataTypes::S2FilePack outlist;
            S2Util::RestoreData(ba, outlist);
            for (auto &&swjFileIn : outlist)
            {
                S2::FileStruct resp { swjFileIn.ID, swjFileIn.data };
                fillJour(resp);
            }
            return true;
        }
        else
        {
            qWarning() << "Failed to load swj from file: " << file;
        }
    }
    return false;
}

QString SwitchJournalDialog::filename(quint64 time) const
{
    // const auto &board = Board::GetInstance();
    QString filename = Settings::dataDir();
    filename.push_back(m_device->getUID());
    // filename.push_back(board.UID());
    filename.push_back("-");
    filename.push_back(QString::number(m_device->getDeviceType(), 16));
    // filename.push_back(QString::number(board.type(), 16));
    filename.push_back("-");
    filename.push_back(QString::number(m_reqSwJNum));
    filename.push_back("-");
    filename.push_back(QString::number(time));
    filename.push_back(".swj");
    return filename;
}

SwitchJournalViewDialog::SwitchJournalViewDialog(
    SwjModel &swjModel, TrendViewModel *oscModel, OscManager &oscManager, QWidget *parent)
    : QDialog(parent)

{
    setupUI(swjModel, oscModel, oscManager);
}

void inline static prepareView(QTableView *view)
{
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->resizeColumnsToContents();
    view->setShowGrid(false);
    view->horizontalHeader()->hide();
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    view->verticalHeader()->hide();
    view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void SwitchJournalViewDialog::setupUI(SwjModel &swjModel, TrendViewModel *oscModel, OscManager &oscManager)
{
    QVBoxLayout *vlyout = new QVBoxLayout;

    auto tableView = new QTableView(this);
    tableView->setModel(swjModel.commonModel);
    prepareView(tableView);

    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(tableView);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    auto pb = new QPushButton(tableView);
    pb->setIcon(QIcon(":/icons/tnosc.svg"));
    pb->setToolTip("Открыть");
    pb->setMinimumSize(50, 50);
    pb->setIconSize(QSize(50, 50));
    connect(pb, &QPushButton::clicked, this,
        [&]
        {
            if (oscModel)
            {
                TrendViewDialog *dlg = new TrendViewDialog;
                dlg->setModel(oscModel);
                dlg->run();
            }
        });
    buttonLayout->addWidget(pb);
    auto swjDialog = qobject_cast<SwitchJournalDialog *>(parent());
    if (swjDialog)
    {
        pb = new QPushButton(this);
        pb->setIcon(QIcon(":/icons/tnsave.svg"));
        pb->setToolTip("Сохранить на диск");
        pb->setMinimumSize(50, 50);
        pb->setIconSize(QSize(50, 50));

        connect(pb, &QPushButton::clicked, this, &SwitchJournalViewDialog::exportJour);
        buttonLayout->addWidget(pb);
    }
    hlyout->addSpacing(50);
    hlyout->addLayout(buttonLayout);
    hlyout->addSpacing(50);
    vlyout->addLayout(hlyout);

    tableView = new QTableView(this);
    tableView->setModel(swjModel.detailModel);
    prepareView(tableView);

    vlyout->addWidget(tableView);

    setLayout(vlyout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_DeleteOnClose);
}
