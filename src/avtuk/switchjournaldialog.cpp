#include "switchjournaldialog.h"

#include "../gen/datamanager.h"
#include "../gen/timefunc.h"
#include "../models/etablemodel.h"
#include "../widgets/wd_func.h"
#include "pushbuttondelegate.h"
#include "swjdialog.h"

#include <QHeaderView>
constexpr int MAXSWJNUM = 262144;

constexpr unsigned char TECH_SWJ = 0x04;

SwitchJournalDialog::SwitchJournalDialog(std::unique_ptr<EOscillogram> osc, QWidget *parent) : UDialog(parent)
{

    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    SWJDOscFunc = std::move(osc);
}

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
        currentRow = 0;
        SWJMap.clear();
        BaseInterface::iface()->writeCommand(Queries::QUSB_ReqBlkDataTech, TECH_SWJ);
    });

    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb, &QAbstractButton::clicked, this, &SwitchJournalDialog::eraseJournals);

    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    SwjTableView = new ETableView;
    TableModel = new ETableModel;
    SwjTableView->setModel(TableModel);

    TableModel->setHorizontalHeaderLabels(
        { "#", "Номер переключения", "Дата/Время", "Аппарат", "Переключение", "Скачать" });
    PushButtonDelegate *dg = new PushButtonDelegate(this);
    connect(dg, &PushButtonDelegate::clicked, this, &SwitchJournalDialog::getSwJ);
    SwjTableView->setItemDelegateForColumn(5, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    lyout->addWidget(SwjTableView);
    setLayout(lyout);
}

void SwitchJournalDialog::processSWJournal(QByteArray &ba)
{
    S2DataTypes::SwitchJourInfo tmpswj;
    auto SWJRecordSize = sizeof(S2DataTypes::SwitchJourInfo);
    size_t BaSize = ba.size();
    size_t BaPos = 0;
    size_t CurRow = 1;
    SWJMap.clear();
    while ((BaPos + SWJRecordSize) <= BaSize)
    {
        memcpy(&tmpswj, &(ba.data()[BaPos]), SWJRecordSize);
        const auto recordNum = tmpswj.num;
        if ((tmpswj.num != 0) && (!SWJMap.contains(recordNum))) // пропуск пустых записей
        {
            SWJMap.insert(recordNum, tmpswj);
            QString craftType = (tmpswj.typeA < craftTypeList.size()) ? craftTypeList.at(tmpswj.typeA) : "N/A";
            craftType += QString::number(tmpswj.numA);
            QVector<QVariant> lsl { QVariant(tmpswj.fileNum),   //
                tmpswj.num,                                     //
                TimeFunc::UnixTime64ToString(tmpswj.time),      //
                craftType,                                      //
                (tmpswj.options & 0x00000001) ? "ВКЛ" : "ОТКЛ", //
                "Скачать" };
            TableModel->addRowWithData(lsl);
            ++CurRow;
        }
        BaPos += SWJRecordSize;
    }
    PushButtonDelegate *dg = new PushButtonDelegate(this);
    connect(dg, &PushButtonDelegate::clicked, this, &SwitchJournalDialog::showJournal);
    SwjTableView->setItemDelegateForColumn(5, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    SwjTableView->resizeRowsToContents();
    SwjTableView->resizeColumnsToContents();
}

void SwitchJournalDialog::processOscillograms()
{
    QByteArray OscInfo;
    int OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    int RecordSize = sizeof(S2DataTypes::OscInfo); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);

    for (int i = 0; i < OscInfoSize; i += RecordSize)
    {
        S2DataTypes::OscInfo gbos;
        size_t tmpt = static_cast<size_t>(RecordSize);
        memcpy(&gbos, &(OscInfo.data()[i]), tmpt);
        OscMap[gbos.unixtime] = gbos;
    }
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
        auto swj = swjManager.load(fs);
        QVBoxLayout *vlyout = new QVBoxLayout;

        auto tableView = new QTableView(this);
        tableView->setModel(swj.commonModel.get());
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
        tableView->setModel(swj.detailModel.get());
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
    if (SWJMap.contains(swjInfo.num))
        return;

    SWJMap.insert(swjInfo.num, swjInfo);
    QString craftType = (swjInfo.typeA < craftTypeList.size()) ? craftTypeList.at(swjInfo.typeA) : "N/A";
    craftType += QString::number(swjInfo.numA);

    QVector<QVariant> lsl { QVariant(swjInfo.fileNum),   //
        swjInfo.num,                                     //
        TimeFunc::UnixTime64ToString(swjInfo.time),      //
        craftType,                                       //
        (swjInfo.options & 0x00000001) ? "ВКЛ" : "ОТКЛ", //
        "Скачать" };
    TableModel->addRowWithData(lsl);

    ++currentRow;
}

void SwitchJournalDialog::getSwJ(const QModelIndex &idx)
{
    bool ok = false;
    auto fileNum
        = idx.model()->data(idx.sibling(idx.row(), Column::number), Qt::DisplayRole).toUInt(&ok); // номер файла
    reqSwJNum
        = idx.model()->data(idx.sibling(idx.row(), Column::switchNumber), Qt::DisplayRole).toUInt(&ok); // номер файла
    if (!ok)
    {
        qWarning("Cannot convert");
        return;
    }
    if (!SWJMap.contains(reqSwJNum))
    {
        qWarning("Cannot find");
        return;
    }
    quint32 size = SWJMap.value(reqSwJNum).fileLength;
    BaseInterface::iface()->reqFile(fileNum, Queries::FileFormat::CustomS2, size + sizeof(S2DataTypes::DataRecHeader));
}

void SwitchJournalDialog::loadJournals()
{
    TableModel->clearModel();
    TableModel->addColumn("#");
    TableModel->addColumn("Time");
    TableModel->addColumn("TypeANumA");
    TableModel->addColumn("Options");
    TableModel->addColumn("Osc");
    TableModel->addColumn("Next");
    TableModel->addRow();
    TableModel->setRowTextAlignment(0, Qt::AlignVCenter | Qt::AlignCenter);
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), QVariant("Номер файла"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), QVariant("Номер переключения"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), QVariant("Дата, время"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 3, QModelIndex()), QVariant("Аппарат"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), QVariant("Переключение"), Qt::EditRole);
    // TableModel->setData(TableModel->index(0, 5, QModelIndex()), QVariant("Осц"), Qt::EditRole);
    // SwjTableView->setSpan(0, 3, 1, 2); // объединение 3 и 4 столбцов в 0 ряду
    QByteArray SWJournals_INF;
    int SWJINFSIZE = sizeof(S2DataTypes::SwitchJourInfo) * MAXSWJNUM;
    SWJournals_INF.resize(SWJINFSIZE);
    //   Commands::GetBt(TECH_SWJ, &(SWJournals_INF.data()[0]), SWJINFSIZE); // в SWJSize - реальная длина в байтах
    SWJournals_INF.resize(SWJINFSIZE);
    // ProcessOscillograms();
    processSWJournal(SWJournals_INF);
}

void SwitchJournalDialog::showJournal(QModelIndex idx)
{
    bool ok;
    int SWJNum = TableModel->data(idx.sibling(idx.row(), 1), Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    if (!ok)
    {
        qWarning("");
        return;
    }
    S2DataTypes::SwitchJourInfo swjr = SWJMap[SWJNum];

    if (swjr.fileNum)
    {
        SWJDialog *dlg = new SWJDialog(std::move(SWJDOscFunc));
        dlg->setModal(false);
        dlg->Init(swjr);
        dlg->show();
    }
}

void SwitchJournalDialog::eraseJournals()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, TECH_SWJ);
}
