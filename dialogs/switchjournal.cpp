#include <QVBoxLayout>
#include <QHeaderView>
#include "../widgets/messagebox.h"
#include "../gen/commands.h"
#include "switchjournal.h"

SwitchJournal::SwitchJournal(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void SwitchJournal::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Получить журнал переключений");
    connect(pb,SIGNAL(clicked()),this,SLOT(LoadJournals()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Стереть журнал переключений");
    connect(pb,SIGNAL(clicked()),this,SLOT(EraseJournals()));
    if (pc.Emul)
        pb->setEnabled(false);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    SwjTableView = new s_tqTableView;
    TableModel = new ETableModel;
    SwjTableView->setModel(TableModel);
    SwjTableView->horizontalHeader()->setVisible(false);
    SwjTableView->verticalHeader()->setVisible(false);
    lyout->addWidget(SwjTableView, 89);
    setLayout(lyout);
}

void SwitchJournal::ProcessSWJournal(QByteArray &ba)
{
    SWJournalRecordStruct tmpswj;
    int SWJRecordSize = sizeof(SWJournalRecordStruct);
    int BaSize = ba.size();
    int BaPos = 0;
    int CurRow = 1;
    while ((BaPos + SWJRecordSize) < BaSize)
    {
        memcpy(&tmpswj, &(ba.data()[BaPos]), SWJRecordSize);
        TableModel->addRow();
        TableModel->setData(TableModel->index(CurRow, 0, QModelIndex()), QVariant(tmpswj.Num), Qt::EditRole);
        QDateTime tmpd = QDateTime::fromString(tmpsl.at(1), "yyyy-MM-dd hh:mm:ss");
        SetModelData(row, 3, QVariant(tmpd.toString("dd-MM-yyyy hh:mm:ss")), Qt::EditRole);
        TableModel->setData(TableModel->index(CurRow, 1, QModelIndex()), "Дата, время", Qt::EditRole);
        TableModel->setData(TableModel->index(CurRow, 2, QModelIndex()), "Аппарат", Qt::EditRole);
        TableModel->setData(TableModel->index(CurRow, 3, QModelIndex()), "Переключение", Qt::EditRole);

    }
}

void SwitchJournal::LoadJournals()
{
    TableModel->addColumn("#");
    TableModel->addColumn("Time");
    TableModel->addColumn("TypeA");
    TableModel->addColumn("NumA");
    TableModel->addColumn("Options");
    TableModel->addRow();
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), "#", Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), "Дата, время", Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), "Аппарат", Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), "Переключение", Qt::EditRole);
    SwjTableView->setSpan(0, 2, 1, 2); // объединение 2 и 3 столбцов в 0 ряду
    QByteArray SWJournal;
    quint32 SWJSize = sizeof(SWJournalRecordStruct) * MAXSWJNUM;
    SWJournal.resize(SWJSize);
    Commands::GetBt(TECH_SWJ, &(SWJournal.data()[0]), SWJSize); // в SWJSize - реальная длина в байтах
    ProcessSWJournal(SWJournal);
}

void SwitchJournal::ShowJournal(QModelIndex idx)
{

}

void SwitchJournal::EraseJournals()
{
    if (Commands::EraseTechBlock(TECH_SWJ) == NOERROR)
        MessageBox2::information(this, "Внимание", "Стёрто успешно");
    else
        MessageBox2::information(this, "Внимание", "Ошибка при стирании");
}
