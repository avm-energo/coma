#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include "../widgets/emessagebox.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "switchjournaldialog.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "trendviewdialog.h"
#include "../gen/timefunc.h"

SwitchJournalDialog::SwitchJournalDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void SwitchJournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Получить журналы переключений");
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadJournals()));
    if (StdFunc::IsInEmulateMode())
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseJournals()));
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
    SWJDialog::SWJournalRecordStruct tmpswj;
    int SWJRecordSize = sizeof(SWJDialog::SWJournalRecordStruct);
    int BaSize = ba.size();
    int BaPos = 0;
    int CurRow = 1;
    SWJMap.clear();
    while ((BaPos + SWJRecordSize) < BaSize)
    {
        memcpy(&tmpswj, &(ba.data()[BaPos]), SWJRecordSize);
        if ((tmpswj.Num != 0) && (!SWJMap.keys().contains(tmpswj.Num))) // пропуск пустых записей
        {
            SWJMap[tmpswj.Num] = tmpswj;
            TableModel->addRow();
            TableModel->setData(TableModel->index(CurRow, 0, QModelIndex()), QVariant(tmpswj.Num), Qt::EditRole);
            TableModel->setData(TableModel->index(CurRow, 1, QModelIndex()), QVariant(TimeFunc::UnixTime64ToString(tmpswj.Time)), Qt::EditRole);
            QStringList tmpsl = QStringList() << "D" << "G" << "CB";
            QString tmps = (tmpswj.TypeA < tmpsl.size()) ? tmpsl.at(tmpswj.TypeA) : "N/A";
            tmps += QString::number(tmpswj.NumA);
            TableModel->setData(TableModel->index(CurRow, 2, QModelIndex()), QVariant(tmps), Qt::EditRole);
            tmps = (tmpswj.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
            TableModel->setData(TableModel->index(CurRow, 3, QModelIndex()), QVariant(tmps), Qt::EditRole);
            if (OscMap.keys().contains(tmpswj.OscTime))
                tmps = "images/oscillogramm.png";
            else
                tmps = "images/hr.png";
            TableModel->setData(TableModel->index(CurRow, 4, QModelIndex()), QVariant(QIcon(tmps)), Qt::DecorationRole);
            TableModel->setData(TableModel->index(CurRow, 5, QModelIndex()), QVariant("Далее"), Qt::EditRole);
            ++CurRow;
        }
        BaPos += SWJRecordSize;
    }
    GetOscPBDelegate *dg = new GetOscPBDelegate(this);
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(ShowJournal(QModelIndex)));
    SwjTableView->setItemDelegateForColumn(5,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    SwjTableView->resizeRowsToContents();
    SwjTableView->resizeColumnsToContents();
}

#if PROGSIZE != PROGSIZE_EMUL
void SwitchJournalDialog::ProcessOscillograms()
{
    QByteArray OscInfo;
    quint32 OscInfoSize; // размер считанного буфера с информацией об осциллограммах
    quint32 RecordSize = sizeof(EOscillogram::GBoStruct); // GBo struct size
    OscInfoSize = MAXOSCBUFSIZE;
    OscInfo.resize(OscInfoSize);
    if ((Commands::GetBt(TECH_Bo, &(OscInfo.data()[0]), OscInfoSize)) != NOERROR)
    {
        WARNMSG("Ошибка при приёме буфера осциллограмм");
        return;
    }
    for (quint32 i = 0; i < OscInfoSize; i+= RecordSize)
    {
        EOscillogram::GBoStruct gbos;
        memcpy(&gbos, &(OscInfo.data()[i]), RecordSize);
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
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), QVariant("#"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), QVariant("Дата, время"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), QVariant("Аппарат"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 3, QModelIndex()), QVariant("Переключение"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), QVariant("Осц"), Qt::EditRole);
//    SwjTableView->setSpan(0, 3, 1, 2); // объединение 3 и 4 столбцов в 0 ряду
    QByteArray SWJournal;
    quint32 SWJSize = sizeof(SWJDialog::SWJournalRecordStruct) * MAXSWJNUM;
    SWJournal.resize(SWJSize);
    Commands::GetBt(TECH_SWJ, &(SWJournal.data()[0]), SWJSize); // в SWJSize - реальная длина в байтах
    SWJournal.resize(SWJSize);
    ProcessOscillograms();
    ProcessSWJournal(SWJournal);
}

void SwitchJournalDialog::ShowJournal(QModelIndex idx)
{
    bool ok;
    quint32 SWJNum = TableModel->data(idx.sibling(idx.row(),0),Qt::DisplayRole).toInt(&ok); // номер осциллограммы
    if (!ok)
    {
        WARNMSG("");
        return;
    }
    SWJDialog::SWJournalRecordStruct swjr = SWJMap[SWJNum];
    EOscillogram::GBoStruct gbos;
    bool oscexist = OscMap.keys().contains(swjr.OscTime);
    if (oscexist)
        gbos = OscMap[swjr.OscTime];
    SWJDialog *dlg = new SWJDialog;
    dlg->setModal(false);
    dlg->Init(swjr, oscexist, gbos);
    dlg->show();
}

void SwitchJournalDialog::EraseJournals()
{
    if (Commands::EraseTechBlock(TECH_SWJ) == NOERROR)
        EMessageBox::information(this, "Внимание", "Стёрто успешно");
    else
        EMessageBox::information(this, "Внимание", "Ошибка при стирании");
}
#endif
