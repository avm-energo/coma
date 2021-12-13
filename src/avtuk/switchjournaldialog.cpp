#include "switchjournaldialog.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/files.h"
#include "../gen/s2.h"
#include "../gen/timefunc.h"
#include "../models/etablemodel.h"
#include "../widgets/wd_func.h"
#include "pushbuttondelegate.h"
#include "swjmanager.h"

#include <QHeaderView>
#include <QMessageBox>
constexpr int MAXSWJNUM = 262144;

constexpr unsigned char TECH_SWJ = 0x04;

namespace crypto
{
static constexpr char hash[] = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
static constexpr char name[] = "swjourHash";
}

SwitchJournalDialog::SwitchJournalDialog(QWidget *parent) : UDialog(crypto::hash, crypto::name, parent)
{
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, this, &SwitchJournalDialog::fillJour);
    connect(&DataManager::GetInstance(), &DataManager::swjInfoReceived, this, &SwitchJournalDialog::fillSwJInfo);
    setupUI();
}

void SwitchJournalDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Получить журналы переключений");
    connect(pb, &QAbstractButton::clicked, [&] {
        swjMap.clear();
        tableModel = UniquePointer<ETableModel>(new ETableModel);
        tableModel->setHorizontalHeaderLabels(
            { "#", "Номер переключения", "Дата/Время", "Аппарат", "Переключение", "Скачать" });
        swjTableView->setModel(tableModel.get());

        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqBlkDataTech, TECH_SWJ);
    });

    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb, &QAbstractButton::clicked, this, &SwitchJournalDialog::eraseJournals);

    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    swjTableView = new ETableView;

    PushButtonDelegate *getDelegate = new PushButtonDelegate(this);
    connect(getDelegate, &PushButtonDelegate::clicked, this, &SwitchJournalDialog::getSwJ);
    // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    swjTableView->setItemDelegateForColumn(5, getDelegate);

    lyout->addWidget(swjTableView);
    setLayout(lyout);
}

void SwitchJournalDialog::fillJour(const DataTypes::FileStruct &fs)
{
    if (!updatesEnabled())
        return;
    fileBuffer.push_back(fs);

    switch (fs.ID)
    {
    case MT_HEAD_ID:
    {
        auto header = oscManager.loadCommon(fs);
        oscManager.setHeader(header);
        break;
    }
    case AVTUK_85::SWJ_ID:
    {
        SwjManager swjManager;
        swjModel = swjManager.load(fs);
        auto dialog = new SwitchJournalViewDialog(swjModel, oscModel, oscManager, this);
        dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(dialog, &SwitchJournalViewDialog::exportJour, this, [&, swjNum = reqSwJNum] { exportSwJ(swjNum); });
        dialog->show();
        dialog->setMinimumHeight(WDFunc::getMainWindow()->height());
        dialog->setMinimumWidth(WDFunc::getMainWindow()->width());
        dialog->adjustSize();
        break;
    }
    default:
    {

        oscModel = oscManager.load(fs);

        if (!oscModel)
        {
            qWarning() << Error::ReadError;
            return;
        }
    }
    }
    // header, swj, osc
    if (fileBuffer.size() == 3)
    {
        QByteArray ba;
        S2::StoreDataMem(ba, fileBuffer, reqSwJNum);
        auto time = swjMap.value(reqSwJNum).time;
        QString file = filename(time);
        if (Files::SaveToFile(file, ba) == Error::Msg::NoError)
        {
            qInfo() << "Swj saved: " << file;
        }
        else
        {
            qWarning() << "Fail to save swj: " << file;
        }
    }
}

void SwitchJournalDialog::fillSwJInfo(S2DataTypes::SwitchJourInfo swjInfo)
{
    if (swjInfo.num == 0)
        return;
    if (swjMap.contains(swjInfo.num))
        return;

    swjMap.insert(swjInfo.num, swjInfo);

    QVector<QVariant> lsl {
        QVariant(swjInfo.fileNum),                                            //
        swjInfo.num,                                                          //
        TimeFunc::UnixTime64ToString(swjInfo.time),                           //
        SwjManager::craftType(swjInfo.typeA) + QString::number(swjInfo.numA), //
        SwjManager::switchType(swjInfo.options),                              //
        tr("Скачать"),                                                        //
    };
    tableModel->addRowWithData(lsl);
}

void SwitchJournalDialog::getSwJ(const QModelIndex &idx)
{
    fileBuffer.clear();

    bool ok = false;
    auto model = idx.model();

    if (!model)
        return;
    // номер файла
    auto fileNum = model->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toUInt(&ok);
    // номер файла
    reqSwJNum = model->data(idx.sibling(idx.row(), Column::switchNumber), Qt::DisplayRole).toUInt(&ok);
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }
    if (!swjMap.contains(reqSwJNum))
    {
        qWarning("Cannot find");
        return;
    }
    quint32 size = swjMap.value(reqSwJNum).fileLength;

    if (!loadIfExist(size))
        BaseInterface::iface()->reqFile(
            fileNum, Queries::FileFormat::CustomS2, size + 2 * sizeof(S2DataTypes::DataRecHeader));
}

void SwitchJournalDialog::exportSwJ(uint32_t swjNum)
{

    if (!swjMap.contains(swjNum))
    {
        qWarning("Cannot find");
        return;
    }
    quint32 size = swjMap.value(swjNum).fileLength;

    auto time = swjMap.value(swjNum).time;
    auto currentFile = filename(time);

    QFile swjFile(currentFile);

    if (!swjFile.exists() || (swjFile.size() < size))
        QMessageBox::information(this, "Сохранение", "Скачайте перед сохранением");

    auto newFile = WDFunc::ChooseFileForSave(this, "Файлы журналов (*.swj)", "swj");
    if (!QFile::copy(currentFile, newFile))
    {
        QMessageBox::warning(this, "Сохранение", "Скачайте перед сохранением");
    }
}

void SwitchJournalDialog::eraseJournals()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, TECH_SWJ);
}

bool SwitchJournalDialog::loadIfExist(quint32 size)
{
    auto time = swjMap.value(reqSwJNum).time;
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
            qInfo() << "Swj loaded from file: " << file;
            DataTypes::S2FilePack outlist;
            S2::RestoreData(ba, outlist);
            for (auto &&file : outlist)
            {
                DataTypes::FileStruct resp { DataTypes::FilesEnum(file.ID), file.data };
                DataManager::addSignalToOutList(DataTypes::SignalTypes::File, resp);
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
    const auto &board = Board::GetInstance();
    QString filename = StdFunc::GetSystemHomeDir();
    filename.push_back(board.UID());
    filename.push_back("-");
    filename.push_back(QString::number(board.type(), 16));
    filename.push_back("-");
    filename.push_back(QString::number(reqSwJNum));
    filename.push_back("-");
    filename.push_back(QString::number(time));
    filename.push_back(".swj");
    return filename;
}

SwitchJournalViewDialog::SwitchJournalViewDialog(
    SwjModel &swjModel, TrendViewModelCRef oscModel, OscManager &oscManager, QWidget *parent)
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

void SwitchJournalViewDialog::setupUI(SwjModel &swjModel, TrendViewModelCRef oscModel, OscManager &oscManager)
{
    QVBoxLayout *vlyout = new QVBoxLayout;

    auto tableView = new QTableView(this);
    tableView->setModel(swjModel.commonModel.get());
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
    connect(pb, &QPushButton::clicked, this, [&] {
        if (oscModel)
        {
            oscManager.loadOsc(oscModel.get());
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
    tableView->setModel(swjModel.detailModel.get());
    prepareView(tableView);

    vlyout->addWidget(tableView);

    setLayout(vlyout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_DeleteOnClose);
}
