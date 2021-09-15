#include "switchjournaldialog.h"

#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "pushbuttondelegate.h"
#include "trendviewdialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
constexpr int MAXSWJNUM = 262144;

constexpr unsigned char TECH_SWJ = 0x04;

SwitchJournalDialog::SwitchJournalDialog(EOscillogram *osc, QWidget *parent) : UDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    SWJDOscFunc = osc;
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
    int SWJRecordSize = sizeof(S2DataTypes::SwitchJourInfo);
    int BaSize = ba.size();
    int BaPos = 0;
    int CurRow = 1;
    SWJMap.clear();
    while ((BaPos + SWJRecordSize) <= BaSize)
    {
        size_t tmpt = static_cast<size_t>(SWJRecordSize);
        memcpy(&tmpswj, &(ba.data()[BaPos]), tmpt);
        int tmpi = static_cast<int>(tmpswj.num);
        if ((tmpswj.num != 0) && (!SWJMap.contains(tmpi))) // пропуск пустых записей
        {
            SWJMap[tmpi] = tmpswj;
            TableModel->addRow();
            TableModel->setData(
                TableModel->index(CurRow, Column::number, QModelIndex()), QVariant(tmpswj.fileNum), Qt::EditRole);
            TableModel->setData(
                TableModel->index(CurRow, Column::switchNumber, QModelIndex()), QVariant(tmpswj.num), Qt::EditRole);
            TableModel->setData(TableModel->index(CurRow, Column::datetime, QModelIndex()),
                QVariant(TimeFunc::UnixTime64ToString(tmpswj.time)), Qt::EditRole);
            QStringList tmpsl { "D", "G", "CB" };
            QString tmps = (tmpswj.typeA < tmpsl.size()) ? tmpsl.at(tmpswj.typeA) : "N/A";
            tmps += QString::number(tmpswj.numA);
            TableModel->setData(TableModel->index(CurRow, Column::id, QModelIndex()), QVariant(tmps), Qt::EditRole);
            tmps = (tmpswj.options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
            TableModel->setData(
                TableModel->index(CurRow, Column::switchType, QModelIndex()), QVariant(tmps), Qt::EditRole);
            if (SWJMap.contains(tmpswj.time))
                tmps = ":/icons/osc.svg";
            else
                tmps = ":/icons/hr.png";
            TableModel->setData(
                TableModel->index(CurRow, Column::download, QModelIndex()), QVariant(QIcon(tmps)), Qt::DecorationRole);
            TableModel->setData(TableModel->index(CurRow, 6, QModelIndex()), QVariant("Далее"), Qt::EditRole);
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
    int RecordSize = sizeof(DataTypes::OscInfo); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    //  if ((Commands::GetBt(TECH_Bo, &(OscInfo.data()[0]), OscInfoSize)) != Error::ER_NOERROR)
    //  {
    //      qWarning("Ошибка при приёме буфера осциллограмм");
    //      return;
    //  }
    for (int i = 0; i < OscInfoSize; i += RecordSize)
    {
        DataTypes::OscInfo gbos;
        size_t tmpt = static_cast<size_t>(RecordSize);
        memcpy(&gbos, &(OscInfo.data()[i]), tmpt);
        OscMap[gbos.unixtime] = gbos;
    }
}

void SwitchJournalDialog::FillJour(const DataTypes::FileStruct &fs)
{
}

void SwitchJournalDialog::fillSwJInfo(S2DataTypes::SwitchJourInfo swjInfo)
{
    if (swjInfo.num == 0)
        return;
    if (SWJMap.contains(swjInfo.num))
        return;

    QString craftType = (swjInfo.typeA < craftTypeList.size()) ? craftTypeList.at(swjInfo.typeA) : "N/A";
    craftType += QString::number(swjInfo.numA);
    SWJMap.insert(swjInfo.num, swjInfo);
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
    // EOscillogram::GBoStruct gbos;
    if (swjr.fileNum)
    {
        SWJDialog *dlg = new SWJDialog(SWJDOscFunc);
        dlg->setModal(false);
        dlg->Init(swjr);
        dlg->show();
    }

    // bool oscexist = OscMap.keys().contains(swjr.Time);
    // if (oscexist)
    //     gbos = OscMap[swjr.Time];
}

void SwitchJournalDialog::eraseJournals()
{
    if (checkPassword())
        BaseInterface::iface()->writeCommand(Queries::QC_EraseTechBlock, TECH_SWJ);
}
