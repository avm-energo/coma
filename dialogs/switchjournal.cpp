#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include "../widgets/messagebox.h"
#include "../gen/commands.h"
#include "../gen/eoscillogram.h"
#include "../widgets/getoscpbdelegate.h"
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
        TableModel->setData(TableModel->index(CurRow, 1, QModelIndex()), QVariant(pc.UnixTime64ToString(tmpswj.Time)), Qt::EditRole);
        QStringList tmpsl = QStringList() << "D" << "G" << "CB";
        QString tmps = (tmpswj.TypeA < tmpsl.size()) ? tmpsl.at(tmpswj.TypeA) : "N/A";
        TableModel->setData(TableModel->index(CurRow, 2, QModelIndex()), QVariant(tmps), Qt::EditRole);
        TableModel->setData(TableModel->index(CurRow, 3, QModelIndex()), QVariant(QString::number(tmpswj.NumA)), Qt::EditRole);
        tmps = (tmpswj.Options & 0x00000001) ? "ВКЛ" : "ОТКЛ";
        TableModel->setData(TableModel->index(CurRow, 4, QModelIndex()), QVariant(tmps), Qt::EditRole);
        if (OscNums.contains(tmpswj.OscTime))
            tmps = "images/oscillogramm.png";
        else
            tmps = "images/hr.png";
        TableModel->setData(TableModel->index(CurRow, 5, QModelIndex()), QVariant(QIcon(tmps)), Qt::DecorationRole);
    }
/*    GetOscPBDelegate *dg = new GetOscPBDelegate;
    connect(dg,SIGNAL(clicked(QModelIndex)),this,SLOT(GetOsc(QModelIndex)));
    tv->setItemDelegateForColumn(4,dg); // устанавливаем делегата (кнопки "Скачать") для соотв. столбца
    tv->resizeRowsToContents();
    tv->resizeColumnsToContents(); */
}

void SwitchJournal::ProcessOscillograms()
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
        OscNums.push_back(gbos.UnixTime);
    }
}

void SwitchJournal::LoadJournals()
{
    TableModel->addColumn("#");
    TableModel->addColumn("Time");
    TableModel->addColumn("TypeA");
    TableModel->addColumn("NumA");
    TableModel->addColumn("Options");
    TableModel->addColumn("Osc");
    TableModel->addRow();
    TableModel->setData(TableModel->index(0, 0, QModelIndex()), QVariant("#"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 1, QModelIndex()), QVariant("Дата, время"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 2, QModelIndex()), QVariant("Аппарат"), Qt::EditRole);
    TableModel->setData(TableModel->index(0, 4, QModelIndex()), QVariant("Переключение"), Qt::EditRole);
    SwjTableView->setSpan(0, 2, 1, 2); // объединение 2 и 3 столбцов в 0 ряду
    SwjTableView->setSpan(0, 4, 1, 2); // объединение 2 и 3 столбцов в 0 ряду
    QByteArray SWJournal;
    quint32 SWJSize = sizeof(SWJournalRecordStruct) * MAXSWJNUM;
    SWJournal.resize(SWJSize);
    Commands::GetBt(TECH_SWJ, &(SWJournal.data()[0]), SWJSize); // в SWJSize - реальная длина в байтах
    ProcessOscillograms();
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
