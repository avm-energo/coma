//#include "../gen/maindef.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/wd_func.h"
#include "getoscpbdelegate.h"
#include "switchjournaldialog.h"
#include "trendviewdialog.h"

SwitchJournalDialog::SwitchJournalDialog(EOscillogram *osc, QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    SWJDOscFunc = osc;
}

void SwitchJournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Получить журналы переключений");
    connect(pb, SIGNAL(clicked()), this, SLOT(LoadJournals()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb, SIGNAL(clicked()), this, SLOT(EraseJournals()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    SwjTableView = new ETableView;
    TableModel = new ETableModel;
    SwjTableView->setModel(TableModel);
    SwjTableView->horizontalHeader()->setVisible(false);
    SwjTableView->verticalHeader()->setVisible(false);
    lyout->addWidget(SwjTableView, 89);
    setLayout(lyout);
}

void SwitchJournalDialog::ProcessSWJournal(QByteArray &ba)
{
    S2DataTypes::SWJINFStruct tmpswj;
    int SWJRecordSize = sizeof(S2DataTypes::SWJINFStruct);
    int BaSize = ba.size();
    int BaPos = 0;
    int CurRow = 1;
    SWJMap.clear();
    while ((BaPos + SWJRecordSize) <= BaSize)
    {
        size_t tmpt = static_cast<size_t>(SWJRecordSize);
        memcpy(&tmpswj, &(ba.data()[BaPos]), tmpt);
        int tmpi = static_cast<int>(tmpswj.Num);
        if ((tmpswj.Num != 0) && (!SWJMap.keys().contains(tmpi))) // пропуск пустых записей
        {
            SWJMap[tmpi] = tmpswj;
            TableModel->addRow();
            TableModel->setData(TableModel->index(CurRow, 0, QModelIndex()), QVariant(tmpswj.FileNum), Qt::EditRole);
            TableModel->setData(TableModel->index(CurRow, 1, QModelIndex()), QVariant(tmpswj.Num), Qt::EditRole);
            TableModel->setData(TableModel->index(CurRow, 2, QModelIndex()),
                QVariant(TimeFunc::UnixTime64ToString(tmpswj.Time)), Qt::EditRole);
            QStringList tmpsl = QStringList() << "D"
                                              << "G"
                                              << "CB";
            QString tmps = (tmpswj.TypeA < tmpsl.size()) ? tmpsl.at(tmpswj.TypeA) : "N/A";
            tmps += QString::number(tmpswj.NumA);
            TableModel->setData(TableModel->index(CurRow, 3, QModelIndex()), QVariant(tmps), Qt::EditRole);
            tmps = (tmpswj.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
            TableModel->setData(TableModel->index(CurRow, 4, QModelIndex()), QVariant(tmps), Qt::EditRole);
            if (SWJMap.keys().contains(tmpswj.Time))
                tmps = "images/osc.png";
            else
                tmps = "images/hr.png";
            TableModel->setData(TableModel->index(CurRow, 5, QModelIndex()), QVariant(QIcon(tmps)), Qt::DecorationRole);
            TableModel->setData(TableModel->index(CurRow, 6, QModelIndex()), QVariant("Далее"), Qt::EditRole);
            ++CurRow;
        }
        BaPos += SWJRecordSize;
    }
    GetOscPBDelegate *dg = new GetOscPBDelegate(this);
    connect(dg, SIGNAL(clicked(QModelIndex)), this, SLOT(ShowJournal(QModelIndex)));
    SwjTableView->setItemDelegateForColumn(5, dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    SwjTableView->resizeRowsToContents();
    SwjTableView->resizeColumnsToContents();
}

#if PROGSIZE != PROGSIZE_EMUL
void SwitchJournalDialog::ProcessOscillograms()
{
    QByteArray OscInfo;
    int OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    int RecordSize = sizeof(EOscillogram::GBoStruct); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    if ((Commands::GetBt(TECH_Bo, &(OscInfo.data()[0]), OscInfoSize)) != Error::ER_NOERROR)
    {
        WARNMSG("Ошибка при приёме буфера осциллограмм");
        return;
    }
    for (int i = 0; i < OscInfoSize; i += RecordSize)
    {
        EOscillogram::GBoStruct gbos;
        size_t tmpt = static_cast<size_t>(RecordSize);
        memcpy(&gbos, &(OscInfo.data()[i]), tmpt);
        OscMap[gbos.UnixTime] = gbos;
    }
}

void SwitchJournalDialog::LoadJournals()
{
    TableModel->ClearModel();
    TableModel->addColumn("#");
    TableModel->addColumn("Time");
    TableModel->addColumn("TypeANumA");
    TableModel->addColumn("Options");
    TableModel->addColumn("Osc");
    TableModel->addColumn("Next");
    TableModel->addRow();
    TableModel->SetRowTextAlignment(0, Qt::AlignVCenter | Qt::AlignCenter);
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), QVariant("Номер файла"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), QVariant("Номер переключения"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), QVariant("Дата, время"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 3, QModelIndex()), QVariant("Аппарат"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), QVariant("Переключение"), Qt::EditRole);
    // TableModel->setData(TableModel->index(0, 5, QModelIndex()), QVariant("Осц"), Qt::EditRole);
    // SwjTableView->setSpan(0, 3, 1, 2); // объединение 3 и 4 столбцов в 0 ряду
    QByteArray SWJournals_INF;
    int SWJINFSIZE = sizeof(SWJDialog::SWJINFStruct) * MAXSWJNUM;
    SWJournals_INF.resize(SWJINFSIZE);
    Commands::GetBt(TECH_SWJ, &(SWJournals_INF.data()[0]), SWJINFSIZE); // в SWJSize - реальная длина в байтах
    SWJournals_INF.resize(SWJINFSIZE);
    // ProcessOscillograms();
    ProcessSWJournal(SWJournals_INF);
}

void SwitchJournalDialog::ShowJournal(QModelIndex idx)
{
    bool ok;
    int SWJNum = TableModel->data(idx.sibling(idx.row(), 1), Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    SWJDialog::SWJINFStruct swjr = SWJMap[SWJNum];
    // EOscillogram::GBoStruct gbos;
    if (swjr.FileNum)
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

void SwitchJournalDialog::EraseJournals()
{
    if (Commands::EraseTechBlock(TECH_SWJ) == Error::ER_NOERROR)
        EMessageBox::information(this, "Внимание", "Стёрто успешно");
    else
        EMessageBox::information(this, "Внимание", "Ошибка при стирании");
}
#endif
