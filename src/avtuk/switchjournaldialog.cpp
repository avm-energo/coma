#include "switchjournaldialog.h"

#include "../gen/datamanager.h"
#include "../gen/timefunc.h"
#include "../models/etablemodel.h"
#include "../widgets/wd_func.h"
#include "pushbuttondelegate.h"

#include <QHeaderView>
constexpr int MAXSWJNUM = 262144;

constexpr unsigned char TECH_SWJ = 0x04;

SwitchJournalDialog::SwitchJournalDialog(QWidget *parent) : UDialog(parent)
{
    connect(&DataManager::GetInstance(), &DataManager::fileReceived, this, &SwitchJournalDialog::FillJour);
    connect(&DataManager::GetInstance(), &DataManager::swjInfoReceived, this, &SwitchJournalDialog::fillSwJInfo);
    SetupUI();
}

void SwitchJournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Получить журналы переключений");
    connect(pb, &QAbstractButton::clicked, [&] {
        swjMap.clear();
        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqBlkDataTech, TECH_SWJ);
    });

    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb, &QAbstractButton::clicked, this, &SwitchJournalDialog::eraseJournals);

    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    SwjTableView = new ETableView;
    TableModel = new ETableModel(SwjTableView);
    SwjTableView->setModel(TableModel);

    TableModel->setHorizontalHeaderLabels(
        { "#", "Номер переключения", "Дата/Время", "Аппарат", "Переключение", "Скачать" });
    PushButtonDelegate *dg = new PushButtonDelegate(this);
    connect(dg, &PushButtonDelegate::clicked, this, &SwitchJournalDialog::getSwJ);
    SwjTableView->setItemDelegateForColumn(5, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    lyout->addWidget(SwjTableView);
    setLayout(lyout);
}

void SwitchJournalDialog::FillJour(const DataTypes::FileStruct &fs)
{
    if (!updatesEnabled())
        return;

    switch (std_ext::to_underlying(fs.filenum))
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
        QVBoxLayout *vlyout = new QVBoxLayout;

        auto tableView = new QTableView(this);
        tableView->setModel(swjModel.commonModel.get());
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tableView->resizeColumnsToContents();
        tableView->setShowGrid(false);
        tableView->horizontalHeader()->hide();
        tableView->verticalHeader()->hide();

        auto pb = new QPushButton(QIcon(":/icons/osc.svg"), "Открыть", tableView);
        connect(pb, &QPushButton::clicked, this, [&] {
            if (oscModel)
            {

                oscManager.loadOsc(oscModel.get());
            }
        });
        tableView->setIndexWidget(tableView->model()->index(9, 1), pb);
        vlyout->addWidget(tableView);

        tableView = new QTableView(this);
        tableView->setModel(swjModel.detailModel.get());
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tableView->setShowGrid(false);
        tableView->resizeColumnsToContents();
        tableView->horizontalHeader()->hide();
        tableView->verticalHeader()->hide();
        vlyout->addWidget(tableView);
        QDialog *dialog = new QDialog;
        dialog->setLayout(vlyout);
        dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
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
}

void SwitchJournalDialog::fillSwJInfo(S2DataTypes::SwitchJourInfo swjInfo)
{
    if (swjInfo.num == 0)
        return;
    if (swjMap.contains(swjInfo.num))
        return;

    swjMap.insert(swjInfo.num, swjInfo);

    QVector<QVariant> lsl { QVariant(swjInfo.fileNum),                        //
        swjInfo.num,                                                          //
        TimeFunc::UnixTime64ToString(swjInfo.time),                           //
        SwjManager::craftType(swjInfo.typeA) + QString::number(swjInfo.numA), //
        SwjManager::switchType(swjInfo.options),                              //
        "Скачать" };
    TableModel->addRowWithData(lsl);
}

void SwitchJournalDialog::getSwJ(const QModelIndex &idx)
{
    bool ok = false;
    auto model = idx.model();

    if (!model)
        return;

    auto fileNum = model->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toUInt(&ok); // номер файла
    reqSwJNum = model->data(idx.sibling(idx.row(), Column::switchNumber), Qt::DisplayRole).toUInt(&ok); // номер файла
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
    BaseInterface::iface()->reqFile(fileNum, Queries::FileFormat::CustomS2, size + sizeof(S2DataTypes::DataRecHeader));
}

void SwitchJournalDialog::eraseJournals()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, TECH_SWJ);
}
